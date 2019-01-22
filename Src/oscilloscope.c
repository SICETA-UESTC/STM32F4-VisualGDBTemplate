#include "oscilloscope.h"
#include "tim.h"
//#include "adc.h"
#include "ads8694.h"
#include "zlg7290.h"
#include "lcd.h"
#include "curve_chart.h"
#include "colors.h"
#include "pattern.h"
#include "fsmc.h"
#include <stdio.h>
#include <string.h>
#include <arm_math.h>

//绘图相关
static uint8_t str_buffer[16];
static uint16_t display_values[GRID_WIDTH];
static CurveChartTypeDef graph;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

//采样
extern _Bool is_extra_gain;
//extern ADC_HandleTypeDef hadc1;
//extern DMA_HandleTypeDef hdma_adc1;
//extern TIM_HandleTypeDef htim3;
static int32_t adc_sample_buffer[SAMPLE_COUNT];

//示波器参数结构体
static OscArgs_TypeDef osc_args;

//频率计
extern TIM_HandleTypeDef htim5;
uint32_t freqmeter_clk_count;
_Bool is_freq_captured;

void Oscilloscope_Init(void)
{
    /* ADC初始化*/
    ADS8694_Init();
    ADS8694_ConfigSampling(&adc_sample_buffer, SAMPLE_COUNT, ADS8694_CHANNEL_0, INPUT_RANGE_BIPOLAR_0_625x);
    //ADC1_Init();
    /* 频率计定时器初始化*/
    TIM5_Init();
    /* 显示用定时器初始化 */
    TIM6_Init();
    TIM7_Init();
    /* 矩阵键盘驱动初始化*/
    //ZLG7290_Init();

    //osc_args.Coupling = DC_Coupling;
    //osc_args.ProbeAttenuation = X1;
    osc_args.TimeBase = DIV_1ms;
    osc_args.VoltBase = DIV_1V;
    osc_args.DisplayScaleFactor = VOLT_FACTOR * 0.05f;
    osc_args.VoltOffset = 0;
    osc_args.TriggerVolt = 0;

    /* GUI 初始化 */
    LCD_Clear(BLACK);

    graph.X = GRID_X;
    graph.Y = GRID_Y;

    graph.Width = GRID_WIDTH;
    graph.Height = GRID_HEIGHT;
    graph.CoarseGridWidth = 100;
    graph.CoarseGridHeight = 50;
    graph.FineGridWidth = 10;
    graph.FineGridHeight = 10;

    graph.BorderColor = WHITE;
    graph.BackgroudColor = BLACK;
    graph.CoarseGridColor = GRAY;
    graph.FineGridColor = DARKGRAY;
    CurveChart_Init(&graph);

    LCD_DrawRect(TIMEBOX_X, TIMEBOX_Y, TIMEBOX_WIDTH, TIMEBOX_HEIGHT, WHITE);
    LCD_DrawString("水平时基档位", 24, TIMEBOX_X + 12, TIMEBOX_Y + 6, WHITE);
    LCD_DrawString(time_base_tag[osc_args.TimeBase], 24, TIMEBOX_X + (TIMEBOX_WIDTH - strlen(time_base_tag[osc_args.TimeBase]) * 12) / 2, TIMEBOX_Y + 36, YELLOW);

    LCD_DrawString("水平偏移", 24, TIMEBOX_X + 36, TIMEBOX_Y + 66, WHITE);
    LCD_DrawBitmapStream(down_triangle_pattern, GRID_X + GRID_WIDTH / 2 + osc_args.TimeOffset - 5, GRID_Y - 12, 11, 11);
    UpdateHorizontalPosInfo();

    LCD_DrawRect(VOLTBOX_X, VOLTBOX_Y, VOLTBOX_WIDTH, VOLTBOX_HEIGHT, WHITE);
    LCD_DrawString("垂直电流档位", 24, VOLTBOX_X + 12, VOLTBOX_Y + 6, WHITE);
    LCD_DrawString(volt_base_tag[osc_args.VoltBase], 24, VOLTBOX_X + (VOLTBOX_WIDTH - strlen(volt_base_tag[osc_args.VoltBase]) * 12) / 2, VOLTBOX_Y + 36, YELLOW);

    LCD_DrawString("垂直偏移", 24, VOLTBOX_X + 36, VOLTBOX_Y + 66, WHITE);
    LCD_DrawBitmapStream(right_triangle_pattern, GRID_X - 12, GRID_Y + GRID_HEIGHT / 2 - osc_args.VoltOffset - 5, 11, 11);
    UpdateVerticalPosInfo();

    /*
    LCD_DrawRect(TRIGBOX_X, TRIGBOX_Y, TRIGBOX_WIDTH, TRIGBOX_HEIGHT, WHITE);
    LCD_DrawString("触发电平", 24, TRIGBOX_X + 36, TRIGBOX_Y + 6, WHITE);

    LCD_DrawRect(INPUTBOX_X, INPUTBOX_Y, INPUTBOX_WIDTH, INPUTBOX_HEIGHT, WHITE);
    LCD_DrawString("耦合", 32, INPUTBOX_X + 8, INPUTBOX_Y + 8, WHITE);
    LCD_DrawString("倍率", 32, INPUTBOX_X + 8, INPUTBOX_Y + 56, WHITE);

    LCD_DrawString("DC", 32, INPUTBOX_X + 96, INPUTBOX_Y + 8, CYAN);
    LCD_DrawString("1x", 32, INPUTBOX_X + 96, INPUTBOX_Y + 56, CYAN);
    */

    LCD_DrawString(GRID_Y + GRID_HEIGHT + 16, GRID_X, "频率", 24, WHITE);
    LCD_DrawString("峰峰值", 24, GRID_X + 192, GRID_Y + GRID_HEIGHT + 16, WHITE);
    LCD_DrawString("有效值", 24, GRID_X + 384, GRID_Y + GRID_HEIGHT + 16, WHITE);

    LCD_DrawImageFromFile(VOLTBOX_X + 16, VOLTBOX_Y + VOLTBOX_HEIGHT + 10, 128, 128, "0:TigerHead.rgb16");
    LCD_DrawString("LG", 24, 770, 450, STEELBLUE);
}

void Oscilloscope_Start(void)
{
    /* 启动ADC采样 */
    //HAL_TIM_Base_Start(&htim3);
    ConfigSamplingArgs();

    /* 启动频率计 */
    HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_4);
    /* 用于定时更新频率与峰峰值显示 */
    HAL_TIM_Base_Start(&htim7);

    //HAL_ADC_Start_DMA(&hadc1, adc_sample_buffer, SAMPLE_COUNT);

    for (;;)
    {
        uint16_t trigger_pos = SAMPLE_COUNT / 2 - 128;
        int32_t delta_volt_a, delta_volt_b;

        ADS8694_StartSampling();
        arm_offset_q31(adc_sample_buffer, -(1 << 17), adc_sample_buffer, SAMPLE_COUNT);

        //HAL_DMA_PollForTransfer(&hdma_adc1, HAL_DMA_FULL_TRANSFER, 0xFFFF);
        //HAL_ADC_Stop_DMA(&hadc1);

        for (uint16_t i = 0; i < 512; i++)
        {
            delta_volt_a = adc_sample_buffer[trigger_pos] - osc_args.TriggerVolt;
            delta_volt_b = adc_sample_buffer[trigger_pos + 1] - osc_args.TriggerVolt;
            /* 过零触发 */
            if (delta_volt_a < 0 && delta_volt_b > 0) {
                break;
            }
            ++trigger_pos;
        }
        int32_t *adc_samples_begin =
            adc_sample_buffer + trigger_pos - GRID_WIDTH / 2 - osc_args.TimeOffset;

        int32_t max_val, min_val;
        uint32_t max_index, min_index;
        float volt_pp;

        arm_max_q31(adc_samples_begin, GRID_WIDTH, &max_val, &max_index);
        arm_min_q31(adc_samples_begin, GRID_WIDTH, &min_val, &min_index);
        //4点平均数字滤波
        arm_mean_q31(adc_samples_begin + max_index - 2, 4, &max_val);
        arm_mean_q31(adc_samples_begin + min_index - 2, 4, &min_val);

        volt_pp = (max_val - min_val) * VOLT_FACTOR;
        if (is_extra_gain) {
            volt_pp *= EXTRA_GAIN_FACTOR;
        }

        //HAL_ADC_Start_DMA(&hadc1, adc_sample_buffer, SAMPLE_COUNT);
        CurveChart_RecoverGrid(&graph, display_values);

        for (uint16_t i = 0; i < GRID_WIDTH; i++) {
            display_values[i] = adc_samples_begin[i] * osc_args.DisplayScaleFactor * (is_extra_gain ? EXTRA_GAIN_FACTOR : 1.0f)
                + GRID_HEIGHT / 2 + osc_args.VoltOffset;
        }

        CurveChart_DrawCurve(&graph, display_values, RED);

        if (__HAL_TIM_GET_COUNTER(&htim7) > 10000) {
            __HAL_TIM_SET_COUNTER(&htim7, 0);

            /* 频率计显示 */
            LCD_FillRect(GRID_X + 64, GRID_Y + GRID_HEIGHT + 16, 108, 24, BLACK);
            if (is_freq_captured && freqmeter_clk_count > 600) {
                if (freqmeter_clk_count > 60000) {
                    sprintf(str_buffer, "%.3fHz", 6000000.0f / freqmeter_clk_count);
                }
                else {
                    sprintf(str_buffer, "%.2fHz", 6000000.0f / freqmeter_clk_count);
                }
                LCD_DrawString(str_buffer, 24, GRID_X + 64, GRID_Y + GRID_HEIGHT + 16, PURPLE);
                is_freq_captured = 0;
            }
            else {
                LCD_DrawString("------", 24, GRID_X + 64, GRID_Y + GRID_HEIGHT + 16, PURPLE);
            }
            /* 峰峰值显示 */
            if (volt_pp < 1000.0f) {
                sprintf(str_buffer, "%.1fmA", volt_pp);
            }
            else {
                sprintf(str_buffer, "%.3fA", volt_pp * 0.001f);
            }
            LCD_FillRect(GRID_X + 280, GRID_Y + GRID_HEIGHT + 16, 108, 24, BLACK);
            LCD_DrawString(str_buffer, 24, GRID_X + 280, GRID_Y + GRID_HEIGHT + 16, PURPLE);

            /* 有效值显示 */
            volt_pp *= 0.3535534f;
            if (volt_pp < 1000.0f) {
                sprintf(str_buffer, "%.2fmA", volt_pp);
            }
            else {
                sprintf(str_buffer, "%.3fA", volt_pp * 0.001f);
            }
            LCD_FillRect(GRID_X + 472, GRID_Y + GRID_HEIGHT + 16, 108, 24, BLACK);
            LCD_DrawString(str_buffer, 24, GRID_X + 472, GRID_Y + GRID_HEIGHT + 16, PURPLE);
#if DEBUG
            printf("Extra Gain = %u, ADC Code diff = %u\n", is_extra_gain, max_val - min_val);
#endif
        }

#if CurveChart_USE_BACKBUFFER
        LCD_BackBuffer_Update();
#endif // CurveChart_USE_BACKBUFFER

        switch (ZLG7290_ReadKey())
        {
            /* 水平时基选择 */
            case 1:
                osc_args.TimeBase = (osc_args.TimeBase + 1) % 3;
                LCD_FillRect(TIMEBOX_X + 12, TIMEBOX_Y + 36, 150, 24, BLACK);
                LCD_DrawString(time_base_tag[osc_args.TimeBase], 24, TIMEBOX_X + (TIMEBOX_WIDTH - strlen(time_base_tag[osc_args.TimeBase]) * 12) / 2, TIMEBOX_Y + 36, YELLOW);
                UpdateHorizontalPosInfo();
                ConfigSamplingArgs();
                break;

                /* 垂直电压档选择 */
            case 2:
                osc_args.VoltBase = (osc_args.VoltBase + 1) % 6;
                LCD_FillRect(VOLTBOX_X + 12, VOLTBOX_Y + 36, 150, 24, BLACK);
                LCD_DrawString(volt_base_tag[osc_args.VoltBase], 24, VOLTBOX_X + (VOLTBOX_WIDTH - strlen(volt_base_tag[osc_args.VoltBase]) * 12) / 2, VOLTBOX_Y + 36, YELLOW);

                switch (osc_args.VoltBase)
                {
                    case DIV_5mV: osc_args.DisplayScaleFactor = VOLT_FACTOR * 10.0f; break;
                    case DIV_10mV: osc_args.DisplayScaleFactor = VOLT_FACTOR * 5.0f; break;
                    case DIV_50mV: osc_args.DisplayScaleFactor = VOLT_FACTOR; break;
                    case DIV_100mV: osc_args.DisplayScaleFactor = VOLT_FACTOR * 0.5f; break;
                    case DIV_500mV: osc_args.DisplayScaleFactor = VOLT_FACTOR * 0.1f; break;
                    case DIV_1V: osc_args.DisplayScaleFactor = VOLT_FACTOR * 0.05f; break;
                    default:
                        break;
                }

                UpdateVerticalPosInfo();
                break;
                /* AC-DC耦合选择 */
                /*
            case 9:
                osc_args.Coupling = !osc_args.Coupling;
                LCD_FillRect(INPUTBOX_X + 96, INPUTBOX_Y + 8, 32, 32, BLACK);
                LCD_DrawString((osc_args.Coupling) ? "AC" : "DC", 32, INPUTBOX_X + 96, INPUTBOX_Y + 8, CYAN);
                break;
                */
                /* 探头倍率选择 */
                /*
            case 10:
                osc_args.ProbeAttenuation = !osc_args.ProbeAttenuation;
                LCD_FillRect(INPUTBOX_X + 96, INPUTBOX_Y + 56, 48, 32, BLACK);
                LCD_DrawString((osc_args.ProbeAttenuation) ? "10x" : "1x", 32, INPUTBOX_X + 96, INPUTBOX_Y + 56, CYAN);
                break;
                */
                /* 水平位置调整 */
            case 3:
                AdjustHorizontalPos(1);
                break;

            case 11:
                AdjustHorizontalPos(0);
                break;

                /* 垂直位置调整 */
            case 4:
                AdjustVerticalPos(1);
                break;

            case 12:
                AdjustVerticalPos(0);
                break;

                /* 触发电平调整 */
            case 5:
                AdjustTriggerVoltage(1);
                break;

            case 13:
                AdjustTriggerVoltage(0);
                break;
                /* ADC重置 */
            case 33:
                ADS8694_Init();
                ADS8694_ConfigSampling(&adc_sample_buffer, SAMPLE_COUNT, ADS8694_CHANNEL_0, INPUT_RANGE_BIPOLAR_0_625x);
                ConfigSamplingArgs();
                break;

            case 34:
                is_extra_gain = !is_extra_gain;
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, !is_extra_gain);

                LCD_FillRect(770, 450, 24, 24, BLACK);
                if (is_extra_gain) {
                    LCD_DrawString("HG", 24, 770, 450, RED);
                }
                else {
                    LCD_DrawString("LG", 24, 770, 450, STEELBLUE);
                }

                break;

            case 37:
                return;

            default:
                break;
        }
        HAL_Delay(24);
    }
}

static void AdjustVerticalPos(_Bool up_down_select)
{
    static _Bool previous_direction;
    static int16_t delta;

    CurveChart_RecoverLineY(&graph, osc_args.TriggerVolt * osc_args.DisplayScaleFactor + GRID_HEIGHT / 2 + osc_args.VoltOffset);

    if (previous_direction == up_down_select) {
        delta += (up_down_select) ? 1 : -1;
        delta = CLAMP(delta, -10, 10);
    }
    else {
        delta = (up_down_select) ? 1 : -1;
    }
    previous_direction = up_down_select;

    LCD_FillRect(GRID_X - 12, GRID_Y + GRID_HEIGHT / 2 - osc_args.VoltOffset - 5, 11, 11, BLACK);

    osc_args.VoltOffset += delta;
    osc_args.VoltOffset = CLAMP(osc_args.VoltOffset, -GRID_HEIGHT / 2, GRID_HEIGHT / 2);

    LCD_DrawBitmapStream(right_triangle_pattern, GRID_X - 12, GRID_Y + GRID_HEIGHT / 2 - osc_args.VoltOffset - 5, 11, 11);
    UpdateVerticalPosInfo();
}

static inline void UpdateVerticalPosInfo(void)
{
    LCD_FillRect(VOLTBOX_X + 44, VOLTBOX_Y + 96, 84, 24, BLACK);
    switch (osc_args.VoltBase)
    {
        case DIV_5mV: sprintf(str_buffer, "%+3.1fmA", osc_args.VoltOffset * 0.1f); break;
        case DIV_10mV: sprintf(str_buffer, "%+3.1fmA", osc_args.VoltOffset * 0.2f); break;
        case DIV_50mV: sprintf(str_buffer, "%+3.0fmA", osc_args.VoltOffset * 1.0f); break;
        case DIV_100mV: sprintf(str_buffer, "%+3.0fmA", osc_args.VoltOffset * 2.0f); break;
        case DIV_500mV: sprintf(str_buffer, "%+3.2fA", osc_args.VoltOffset * 0.01f); break;
        case DIV_1V: sprintf(str_buffer, "%+3.2fA", osc_args.VoltOffset * 0.02f); break;
        default: return;
    }
    LCD_DrawString(str_buffer, 24, VOLTBOX_X + 44, VOLTBOX_Y + 96, YELLOW);
}

static void AdjustHorizontalPos(_Bool right_left_select)
{
    static _Bool previous_direction;
    static int16_t delta;

    if (previous_direction == right_left_select) {
        delta += (right_left_select) ? 1 : -1;
        delta = CLAMP(delta, -10, 10);
    }
    else {
        delta = (right_left_select) ? 1 : -1;
    }
    previous_direction = right_left_select;

    LCD_FillRect(GRID_X + GRID_WIDTH / 2 + osc_args.TimeOffset - 5, GRID_Y - 12, 11, 11, BLACK);

    osc_args.TimeOffset += delta;
    osc_args.TimeOffset = CLAMP(osc_args.TimeOffset, -GRID_WIDTH / 2, GRID_WIDTH / 2);

    LCD_DrawBitmapStream(down_triangle_pattern, GRID_X + GRID_WIDTH / 2 + osc_args.TimeOffset - 5, GRID_Y - 12, 11, 11);
    UpdateHorizontalPosInfo();
}

static inline void UpdateHorizontalPosInfo(void)
{
    LCD_FillRect(TIMEBOX_X + 44, TIMEBOX_Y + 96, 84, 24, BLACK);
    switch (osc_args.TimeBase)
    {
        case DIV_1ms: sprintf(str_buffer, "%+3.2fms", osc_args.TimeOffset * 0.02f); break;
        case DIV_5ms: sprintf(str_buffer, "%+3.2fms", osc_args.TimeOffset * 0.1f); break;
        case DIV_10ms: sprintf(str_buffer, "%+3.1fms", osc_args.TimeOffset * 0.2f); break;
        case DIV_50ms: sprintf(str_buffer, "%+3.1fms", osc_args.TimeOffset); break;
        default: return;
    }
    LCD_DrawString(str_buffer, 24, TIMEBOX_X + 44, TIMEBOX_Y + 96, YELLOW);
}

static void AdjustTriggerVoltage(_Bool up_down_select)
{
    static _Bool previous_direction;
    static int16_t delta;
    int16_t setp;

    switch (osc_args.VoltBase)
    {
        case DIV_5mV: setp = 2; break;
        case DIV_10mV: setp = 4; break;
        case DIV_50mV: setp = 20; break;
        case DIV_100mV: setp = 50; break;
        case DIV_500mV: setp = 100; break;
        case DIV_1V: setp = 200; break;
        default:
            break;
    }

    if (previous_direction == up_down_select) {
        delta += (up_down_select) ? setp : -setp;
        delta = CLAMP(delta, -1000, 1000);
    }
    else {
        delta = (up_down_select) ? setp : -setp;
    }
    previous_direction = up_down_select;

    uint16_t line_pos_Y = 0;

    CurveChart_RecoverLineY(&graph, osc_args.TriggerVolt * osc_args.DisplayScaleFactor + GRID_HEIGHT / 2 + osc_args.VoltOffset);

    osc_args.TriggerVolt += delta;
    osc_args.TriggerVolt = CLAMP(osc_args.TriggerVolt, -(1 << 17), (1 << 17) - 1);

    CurveChart_DrawDashedLineY(&graph, osc_args.TriggerVolt * osc_args.DisplayScaleFactor + GRID_HEIGHT / 2 + osc_args.VoltOffset, AZURE);

    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start_IT(&htim6);
}

void TIM6_DAC_IRQHandler(void)
{
    if (__HAL_TIM_GET_IT_SOURCE(&htim6, TIM_IT_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
        HAL_TIM_Base_Stop_IT(&htim6);
        CurveChart_RecoverLineY(&graph, osc_args.TriggerVolt * osc_args.DisplayScaleFactor + osc_args.VoltOffset + GRID_HEIGHT / 2);
    }
}

static inline void ConfigSamplingArgs(void)
{
    switch (osc_args.TimeBase)
    {
        case DIV_1ms: ADS8694_SetSamplingRate(100000); break;
        case DIV_5ms: ADS8694_SetSamplingRate(20000); break;
        case DIV_10ms: ADS8694_SetSamplingRate(10000); break;
        case DIV_50ms: ADS8694_SetSamplingRate(2000); break;
        default:
            break;
    }
    /*
    HAL_ADC_Stop(&hadc1);

    uint16_t period = 12 * pow10(osc_args.TimeBase) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim3, period);

    uint32_t sampling_time = (osc_args.TimeBase == DIV_100us) ? ADC_SAMPLETIME_3CYCLES : ADC_SAMPLETIME_56CYCLES;
    hadc1.Instance->SMPR1 &= ~ADC_SMPR1(ADC_SMPR1_SMP10, ADC_CHANNEL_5);
    hadc1.Instance->SMPR1 |= ADC_SMPR1(sampling_time, ADC_CHANNEL_5);

    HAL_ADC_Start_DMA(&hadc1, adc_sample_buffer, SAMPLE_COUNT);*/
}

/* 频率计脉宽捕获中断 */
void TIM5_IRQHandler(void)
{
    static uint32_t capture1, capture2;
    static _Bool capture_flag;
    
    if (__HAL_TIM_GET_FLAG(&htim5, TIM_IT_CC4) != RESET) {
        __HAL_TIM_CLEAR_FLAG(&htim5, TIM_IT_CC4);

        if (!capture_flag) {
            capture1 = htim5.Instance->CCR4;
        }
        else {
            capture2 = htim5.Instance->CCR4;

            if (capture2 > capture1) {
                freqmeter_clk_count = capture2 - capture1;
            }
            else {
                freqmeter_clk_count = (__HAL_TIM_GET_AUTORELOAD(&htim5) - capture1 + 1) + capture2;
            }
        }
        capture_flag = !capture_flag;
        is_freq_captured = 1;
    }
}