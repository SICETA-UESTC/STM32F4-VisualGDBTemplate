#include "frequency_sweep.h"
#include "number_input.h"
#include "lcd.h"
#include "curve_chart.h"
#include "colors.h"
#include "zlg7290.h"
#include "lmh6518.h"

#include <arm_math.h>

//绘图相关
static uint8_t str_buffer[32];
static uint16_t display_values[GRID_WIDTH];
static CurveChartTypeDef chart;

static _Bool is_cursor_select_A;
static int16_t cursor_XA, cursor_XB;

//输出幅值控制
static uint8_t output_amp;
static uint8_t amp_step;
//static uint8_t pe4302_2x_loss;	// dB

//扫频控制
static uint32_t sweep_freq[3];		// 0:起始频率, 1:终止频率, 2:频率步进

//检波电平采样
extern ADC_HandleTypeDef hadc1;
static uint16_t adc_sampling_values[ADC_SAMPLE_COUNT];
static uint16_t data_values[MAX_SAMPLE_COUNT];
static uint16_t normalize_values[MAX_SAMPLE_COUNT];
static uint16_t sample_count;

void FreqSweep_Init(void)
{
    //硬件外设初始化
    PE4302_Init();
    AD9959_Init();
    ADC1_Init();

    AD9959_SetFreq(OUTPUT_CHANNEL, 10000000U);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

    //设置各项初始参数
    output_amp = 50;
    amp_step = 1;

    sweep_freq[0] = 1000;
    sweep_freq[1] = 50000;
    sweep_freq[2] = 100;
    sample_count = (sweep_freq[1] - sweep_freq[0]) / sweep_freq[2];

    //GUI 初始化
    chart.X = GRID_X;
    chart.Y = GRID_Y;
    chart.Width = GRID_WIDTH;
    chart.Height = GRID_HEIGHT;
    chart.BorderColor = WHITE;
    chart.BackgroudColor = BLACK;
    chart.CoarseGridColor = GRAY;
    chart.FineGridColor = DARKGRAY;

    CurveChart_Init(&chart);

    /* Interp Test */
    /*
    uint16_t size = 128;
    float delta = 0.12f;

    uint16_t sample_data[128];
    uint16_t origin_data[GRID_WIDTH];
    uint16_t interp_data[GRID_WIDTH];

    for (uint16_t i = 0; i < size; i++) {
        sample_data[i] = (2.0f + arm_sin_f32(delta * i)) * 100U;
    }

    for (uint16_t i = 0; i < GRID_WIDTH; i++) {
        //Linear Interpolation
        uint32_t x = (i << 20) / GRID_WIDTH * size;
        interp_data[i] = arm_linear_interp_q15(sample_data, x, size);
        //For Contrast
        origin_data[i] = (2.0f + arm_sin_f32(delta * size / GRID_WIDTH * i)) * 100U;
    }

    CurveChart_DrawCurve(&chart, origin_data, BLUE);
    CurveChart_DrawCurve(&chart, interp_data, RED);
    */
    //LCD_BackBuffer_Update();

    int i;

    //纵坐标-幅度
    LCD_DrawCharASCII('0', 16, GRID_X - 15, GRID_Y + 192, WHITE);
    for (i = 0; i < 4; i++) {
        LCD_DrawNumber((4 - i) * 10, 16, GRID_X - 20, GRID_Y - 8 + i * 50, WHITE);
    }
    for (i = 0; i < 3; i++) {
        LCD_DrawNumber((i + 1) * 10, 16, GRID_X - 20, GRID_Y + 242 + i * 50, WHITE);
        LCD_DrawCharASCII('-', 16, GRID_X - 30, GRID_Y + 242 + i * 50, WHITE);
    }
    //单位：dB
    LCD_DrawString("dB", 16, GRID_X - 20, GRID_Y + GRID_HEIGHT - 16, WHITE);

    //横坐标-频率
    UpdateFreqInfoDispaly();
    //单位：MHz
    LCD_DrawString("MHz", 16, GRID_X + GRID_WIDTH - 12, GRID_Y + GRID_HEIGHT + 20, WHITE);

    //扫频信息窗    
    LCD_DrawRect(FREQBOX_X, FREQBOX_Y, FREQBOX_WIDTH, FREQBOX_HEIGHT, WHITE);
    LCD_DrawString("起始频率:", 16, FREQBOX_X + 8, FREQBOX_Y + 8, WHITE);
    LCD_DrawString("终止频率:", 16, FREQBOX_X + 8, FREQBOX_Y + 32, WHITE);
    LCD_DrawString("频率步进:", 16, FREQBOX_X + 8, FREQBOX_Y + 56, WHITE);
    LCD_DrawString("采样点数:", 16, FREQBOX_X + 8, FREQBOX_Y + 80, WHITE);

    //输出幅度信息窗
    LCD_DrawRect(AMPBOX_X, AMPBOX_Y, AMPBOX_WIDTH, AMPBOX_HEIGHT, WHITE);
    LCD_DrawString("当前输出幅度:", 16, AMPBOX_X + 8, AMPBOX_Y + 8, WHITE);
    LCD_DrawString("幅度调节步进:", 16, AMPBOX_X + 8, AMPBOX_Y + 32, WHITE);

    sprintf(str_buffer, "%-3u mV", (amp_step + 1) * 2);
    LCD_DrawString(str_buffer, 16, AMPBOX_X + 118, AMPBOX_Y + 32, LIGHTGRAY);
    UpdateOutputAmp();

    //光标读数窗
    LCD_DrawRect(CURSORBOX_X, CURSORBOX_Y, CURSORBOX_WIDTH, CURSORBOX_HEIGHT, WHITE);
    LCD_DrawString("光标选择:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 8, WHITE);

    LCD_DrawString("A - 频率:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 40, WHITE);
    LCD_DrawString("B - 频率:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 64, WHITE);
    LCD_DrawString("Δ- 频率:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 88, WHITE);

    LCD_DrawString("A - 增益:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 120, WHITE);
    LCD_DrawString("B - 增益:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 144, WHITE);
    LCD_DrawString("Δ- 增益:", 16, CURSORBOX_X + 8, CURSORBOX_Y + 168, WHITE);

    //光标初始化
    is_cursor_select_A = 1;
    cursor_XA = GRID_WIDTH / 3;
    cursor_XB = GRID_WIDTH * 2 / 3;
    CursorParametersDisplay();
}

void FreqSweep_Start()
{
    for (;;)
    {
        switch (ZLG7290_ReadKey())
        {
            case 9:
                //归一化校准
                FreqSweepAndSampling();
                for (uint16_t i = 0; i < sample_count; i++) {
                    normalize_values[i] = data_values[i] - 1241;
                }
                break;

            case 17:
                SetFreqParameters();
                break;

            case 25:
                ++amp_step;
                amp_step %= 5;

                sprintf(str_buffer, "%-3u mV", (amp_step + 1) * 2);

                LCD_FillRect(AMPBOX_X + 118, AMPBOX_Y + 32, 48, 16, BLACK);
                LCD_DrawString(str_buffer, 16, AMPBOX_X + 118, AMPBOX_Y + 32, LIGHTGRAY);
                break;

            case 26:
                if (output_amp > amp_step + 2) {
                    output_amp -= amp_step + 1;
                }

                UpdateOutputAmp();
                break;

            case 27:
                if (output_amp + amp_step < 50) {
                    output_amp += amp_step + 1;
                }

                UpdateOutputAmp();
                break;

            case 33:
                is_cursor_select_A = !is_cursor_select_A;

                CurveChart_RecoverLineX(&chart, cursor_XA);
                CurveChart_RecoverLineX(&chart, cursor_XB);
                CursorParametersDisplay();
                break;

            case 34:
                CurveChart_RecoverLineX(&chart, cursor_XA);
                CurveChart_RecoverLineX(&chart, cursor_XB);

                if (is_cursor_select_A) {
                    cursor_XA -= 2;
                    cursor_XA = (cursor_XA <= 0) ? GRID_WIDTH - 1 : cursor_XA;
                }
                else {
                    cursor_XB -= 2;
                    cursor_XB = (cursor_XB <= 0) ? GRID_WIDTH - 1 : cursor_XB;
                }

                CursorParametersDisplay();
                break;

            case 35:
                CurveChart_RecoverLineX(&chart, cursor_XA);
                CurveChart_RecoverLineX(&chart, cursor_XB);

                if (is_cursor_select_A) {
                    cursor_XA += 2;
                    cursor_XA %= GRID_WIDTH;
                }
                else {
                    cursor_XB += 2;
                    cursor_XB %= GRID_WIDTH;
                }

                CursorParametersDisplay();
                break;
        }

        FreqSweepAndSampling();

        CurveChart_RecoverLineX(&chart, cursor_XA);
        CurveChart_RecoverLineX(&chart, cursor_XB);
        CurveChart_RecoverGrid(&chart, display_values);

        //将采样数据减去归一化值
        arm_sub_q15(data_values, normalize_values, data_values, sample_count);
        //将采样数据缩放到图表区域范围中
        for (size_t i = 0; i < sample_count; i++) {
            data_values[i] *= 0.161133f;
        }

        //将采样数据线性插值到图表区相同的宽度以便于显示
        for (uint16_t i = 0; i < GRID_WIDTH; i++) {
            //uint32_t x = (i << 20) * sample_count / GRID_WIDTH; //Watch for overflow dude!
            uint32_t x = (i << 20) / GRID_WIDTH * sample_count;
            display_values[i] = arm_linear_interp_q15(data_values, x, sample_count);
        }
        //arm_scale_q15(display_values, 165, -10, display_values, GRID_WIDTH);
        //arm_shift_q15(display_values, -4, display_values, GRID_WIDTH);

        if (is_cursor_select_A) {
            CurveChart_DrawDashedLineX(&chart, cursor_XA, YELLOW);
            CurveChart_DrawDashedLineX(&chart, cursor_XB, BROWN);
        }
        else {
            CurveChart_DrawDashedLineX(&chart, cursor_XA, BROWN);
            CurveChart_DrawDashedLineX(&chart, cursor_XB, YELLOW);
        }

        CurveChart_DrawCurve(&chart, display_values, RED);

#if GRAPH_USE_BACKBUFFER
        LCD_BackBuffer_Update();
#endif // GRAPH_USE_BACKBUFFER

        HAL_Delay(33);
    }
}

static void FreqSweepAndSampling(void)
{
    uint32_t output_freq = sweep_freq[0];

    //ADC开始采样对数检波电平
    HAL_ADC_Start_DMA(&hadc1, adc_sampling_values, ADC_SAMPLE_COUNT);

    for (uint16_t i = 0; i < sample_count; i++)
    {
        //更新DDS输出频率
        AD9959_SetFreq(OUTPUT_CHANNEL, output_freq * 1000U);
        //等待检波电平稳定
        DelayUs(320);
        //记录采样点（带均值滤波）
        arm_mean_q15(adc_sampling_values, ADC_SAMPLE_COUNT, &data_values[i]);
        //频率递进
        output_freq += sweep_freq[2];
    }

    //ADC停止采样
    HAL_ADC_Stop_DMA(&hadc1);
}

static void UpdateFreqInfoDispaly(void)
{
    //使用大黑块进行[数据删除]
    LCD_FillRect(GRID_X - 12, GRID_Y + GRID_HEIGHT + 3, GRID_WIDTH + 36, 16, BLACK);
    LCD_FillRect(FREQBOX_X + 85, FREQBOX_Y + 8, 80, 88, BLACK);

    //更新频率轴坐标值
    for (uint8_t i = 0; i <= 10; i++) {

        sprintf(str_buffer, "%-4.1f", (sweep_freq[0] + i * (sweep_freq[1] - sweep_freq[0]) / 10) * 0.001f);
        LCD_DrawString(str_buffer, 16, GRID_X - 12 + i * 50, GRID_Y + GRID_HEIGHT + 2, WHITE);
    }

    //更新扫频信息窗显示数值
    for (uint8_t i = 0; i < 3; i++) {
        sprintf(str_buffer, "%-6.3f MHz", sweep_freq[i] * 0.001f);
        LCD_DrawString(str_buffer, 16, FREQBOX_X + 85, FREQBOX_Y + 8 + 24 * i, LIGHTGRAY);
    }

    sprintf(str_buffer, "%u 点", sample_count);
    LCD_DrawString(str_buffer, 16, FREQBOX_X + 85, FREQBOX_Y + 80, LIGHTGRAY);
}

static inline void UpdateOutputAmp(void)
{
    //查表并设置衰减值
    PE4302_SetAttenuation(amp_table[50 - output_amp][0]);
    AD9959_SetAmp(OUTPUT_CHANNEL, amp_table[50 - output_amp][1]);

    //使用大黑块进行[数据删除]
    LCD_FillRect(AMPBOX_X + 118, AMPBOX_Y + 8, 48, 16, BLACK);

    //更新输出幅度息窗显示数值
    sprintf(str_buffer, "%-3u mV", output_amp * 2);
    LCD_DrawString(str_buffer, 16, AMPBOX_X + 118, AMPBOX_Y + 8, LIGHTGRAY);
}

static inline void FreqParametersDisplay(uint8_t i, _Bool is_selected)
{
    uint16_t back_color = (is_selected) ? LIGHTGRAY : BLACK;
    uint16_t font_color = (is_selected) ? BLACK : LIGHTGRAY;

    LCD_FillRect(FREQBOX_X + 85, FREQBOX_Y + 8 + 24 * i, 80, 16, back_color);
    sprintf(str_buffer, "%-6.3f MHz", sweep_freq[i] * 0.001f);
    LCD_DrawString(str_buffer, 16, FREQBOX_X + 85, FREQBOX_Y + 8 + 24 * i, font_color);
}

static void SetFreqParameters(void)
{
    uint8_t i = 0;
    float input_val;

    uint16_t backup_sweep_freq[3];
    backup_sweep_freq[0] = sweep_freq[0];
    backup_sweep_freq[1] = sweep_freq[1];
    backup_sweep_freq[2] = sweep_freq[2];

    FreqParametersDisplay(i, 1);

    for (;;)
    {
        switch (ZLG7290_ReadKey())
        {
            case 18:

                FreqParametersDisplay(i, 0);

                if (i == 0) {
                    i = 2;
                }
                else {
                    --i;
                }

                FreqParametersDisplay(i, 1);
                break;

            case 19:

                FreqParametersDisplay(i, 0);

                if (i == 2) {
                    i = 0;
                }
                else {
                    ++i;
                }

                FreqParametersDisplay(i, 1);
                break;

            case 20:

                if (GetInputFloat(&input_val)) {
                    sweep_freq[i] = input_val * 1000U;
                    FreqParametersDisplay(i, 1);
                }
                break;

            case 17:
                //输入的扫频范围无效 还原原来的范围
                if (sweep_freq[0] > sweep_freq[1] || sweep_freq[1] < 100U || sweep_freq[1] > 200000U) {
                    sweep_freq[0] = backup_sweep_freq[0];
                    sweep_freq[1] = backup_sweep_freq[1];
                }

                sample_count = (sweep_freq[1] - sweep_freq[0]) / sweep_freq[2];

                //采样点太多或太少 还原原来的频率步进
                if (sample_count < MIN_SAMPLE_COUNT || sample_count > MAX_SAMPLE_COUNT) {
                    sweep_freq[2] = backup_sweep_freq[2];
                    sample_count = (sweep_freq[1] - sweep_freq[0]) / sweep_freq[2];
                }

                UpdateFreqInfoDispaly();
                return;
        }

        HAL_Delay(33);
    }
}
/*
static void GetCodeTable(void)
{
    extern UART_HandleTypeDef huart1;
    uint16_t amp_ = 50;
    uint16_t code;
    char strbuff[1024] = { 0 };

    AD9959_SetFreq(OUTPUT_CHANNEL, 20000000U);
    HAL_ADC_Start_DMA(&hadc1, adc_sampling_values, ADC_SAMPLE_COUNT);

    for (uint16_t i = 0; i < 49; i++)
    {
        PE4302_SetLoss(amp_table[50 - amp_][0]);
        AD9959_SetAmp(OUTPUT_CHANNEL, amp_table[50 - amp_][1]);

        DelayUs(5000);
        arm_mean_q15(adc_sampling_values, ADC_SAMPLE_COUNT, &code);

        sprintf(strbuff+ 6 * i, "%u, ", code);

        --amp_;
    }

    HAL_ADC_Stop_DMA(&hadc1);
    HAL_UART_Transmit(&huart1, strbuff, strlen(strbuff), 0xFFFF);
}
*/

static inline void CursorParametersDisplay(void)
{
    //用大黑块进行[数据删除]
    LCD_FillRect(CURSORBOX_X + 85, CURSORBOX_Y + 8, 88, 180, BLACK);

    uint8_t mark = (is_cursor_select_A) ? 'A' : 'B';
    LCD_DrawCharASCII(mark, 16, CURSORBOX_X + 85, CURSORBOX_Y + 8, YELLOW);

    float freq_A = (sweep_freq[0] + (sweep_freq[1] - sweep_freq[0]) * cursor_XA / GRID_WIDTH) * 0.001f;
    float freq_B = (sweep_freq[0] + (sweep_freq[1] - sweep_freq[0]) * cursor_XB / GRID_WIDTH) * 0.001f;

    sprintf(str_buffer, "%-6.3f MHz", freq_A);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 40, LIGHTGRAY);

    sprintf(str_buffer, "%-6.3f MHz", freq_B);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 64, LIGHTGRAY);

    sprintf(str_buffer, "%-6.3f MHz", freq_B - freq_A);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 88, LIGHTGRAY);

    float gain_A = (display_values[cursor_XA] - 200) * 0.2f;
    float gain_B = (display_values[cursor_XB] - 200) * 0.2f;

    sprintf(str_buffer, "%-6.3f dB", gain_A);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 120, LIGHTGRAY);

    sprintf(str_buffer, "%-6.3f dB", gain_B);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 144, LIGHTGRAY);

    sprintf(str_buffer, "%-6.3f dB", gain_B - gain_A);
    LCD_DrawString(str_buffer, 16, CURSORBOX_X + 85, CURSORBOX_Y + 168, LIGHTGRAY);
}
