#include "spectrum.h"
#include "lcd.h"
#include "curve_chart.h"
#include "colors.h"
//#include "adc.h"
#include "ads8694.h"
#include "tim.h"

#include <arm_math.h>

extern TIM_HandleTypeDef htim3;

//采样及数据处理
extern _Bool is_extra_gain;
//extern ADC_HandleTypeDef hadc1;
//extern DMA_HandleTypeDef hdma_adc1;
static uint16_t sample_count;
static int32_t adc_sample_buffer[MAX_SAMPLE_COUNT];
float window_func[MAX_SAMPLE_COUNT];
float sample_values[MAX_SAMPLE_COUNT];
float fft_input[MAX_SAMPLE_COUNT];
float fft_output[MAX_SAMPLE_COUNT];
int16_t fft_mag[MAX_SAMPLE_COUNT / 2];
arm_rfft_fast_instance_f32 rfft;

// 绘图相关
static uint8_t str_buffer[16];
static int16_t display_values[GRID_WIDTH];
static CurveChartTypeDef chart;

static float scale_factor = 10.0f;
static uint8_t freq_base;
static uint8_t energy_base;
static int16_t cursor_pos = 50;

void SpectrumDisplay_Init(void)
{
    //TIM3_Init();
    //ADC1_Init();
    freq_base = DIV_1kHz;
    energy_base = 1;

    /* ADC初始化*/
    ADS8694_Init();
    /* 矩阵键盘驱动初始化*/
    //ZLG7290_Init();

    //GUI 初始化
    LCD_Clear(BLACK);
    chart.X = GRID_X;
    chart.Y = GRID_Y;

    chart.Width = GRID_WIDTH;
    chart.Height = GRID_HEIGHT;
    chart.CoarseGridWidth = 50;
    chart.CoarseGridHeight = 50;
    chart.FineGridWidth = 10;
    chart.FineGridHeight = 10;

    chart.BorderColor = WHITE;
    chart.BackgroudColor = BLACK;
    chart.CoarseGridColor = GRAY;
    chart.FineGridColor = DARKGRAY;
    CurveChart_Init(&chart);

    LCD_DrawRect(BASEFREQBOX_X, BASEFREQBOX_Y, BASEFREQBOX_WIDTH, BASEFREQBOX_HEIGHT, WHITE);
    LCD_DrawString("基波频率", 24, BASEFREQBOX_X + 36, BASEFREQBOX_Y + 6, WHITE);

    LCD_DrawRect(AMPBOX_X, AMPBOX_Y, AMPBOX_WIDTH, AMPBOX_HEIGHT, WHITE);
    LCD_DrawString("各次谐波幅度", 24, AMPBOX_X + 12, AMPBOX_Y + 6, WHITE);

    LCD_DrawString("一次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 38, LIGHTGRAY);
    LCD_DrawString("二次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 60, LIGHTGRAY);
    LCD_DrawString("三次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 82, LIGHTGRAY);
    LCD_DrawString("四次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 104, LIGHTGRAY);
    LCD_DrawString("五次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 126, LIGHTGRAY);
    LCD_DrawString("六次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 148, LIGHTGRAY);
    LCD_DrawString("七次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 170, LIGHTGRAY);
    LCD_DrawString("八次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 192, LIGHTGRAY);
    LCD_DrawString("九次谐波", 16, AMPBOX_X + 8, AMPBOX_Y + 214, LIGHTGRAY);

    LCD_DrawString("水平档位", 24, GRID_X, GRID_Y + GRID_HEIGHT + 24, WHITE);
    LCD_DrawString("光标位置", 24, GRID_X + 216, GRID_Y + GRID_HEIGHT + 24, WHITE);

    LCD_DrawImageFromFile(AMPBOX_X + 16, AMPBOX_Y + AMPBOX_HEIGHT + 10, 128, 128, "0:TigerHead.rgb16");

    LCD_DrawString("LG", 24, 770, 450, STEELBLUE);

    UpdateFrequencyInfo();
    UpdateCursorInfo();
    UpdateSamplingArgs();
}

void SpectrumDisplay_Start(void)
{
    //HAL_TIM_Base_Start(&htim3);
    //HAL_ADC_Start_DMA(&hadc1, adc_sample_buffer, sample_count);

    for (;;)
    {
        /* Wait for sampling period to complete */
        //HAL_DMA_PollForTransfer(&hdma_adc1, HAL_DMA_FULL_TRANSFER, 0xFFFF);

        /* Stop sampling while conversion */
        //HAL_ADC_Stop_DMA(&hadc1);
        ADS8694_StartSampling();
        arm_offset_q31(adc_sample_buffer, -(1 << 17), adc_sample_buffer, sample_count);
        arm_q31_to_float(adc_sample_buffer, sample_values, sample_count);
        //HAL_ADC_Start_DMA(&hadc1, adc_sample_buffer, sample_count);
        /* Apply window function and FFT */
        arm_mult_f32(sample_values, window_func, fft_input, sample_count);
        arm_rfft_fast_f32(&rfft, fft_input, fft_output, 0);
        /* Compute spectrum magnitude */
        arm_cmplx_mag_f32(fft_output, fft_output, sample_count / 2);
        /* Scale and convert back to int16_t values */
        arm_scale_f32(fft_output, scale_factor, fft_output, sample_count / 2);
        arm_float_to_q15(fft_output, fft_mag, sample_count / 2);

        //float energy_sum = fft_output[66] + fft_output[67] + fft_output[68] + fft_output[69] + fft_output[70];

        CurveChart_RecoverGrid(&chart, display_values);
        CurveChart_RecoverRect(&chart, cursor_pos - 8, display_values[cursor_pos] + 16, 16, 16);

        /* Linear interpolation for display values */
        for (uint16_t i = 0; i < GRID_WIDTH; i++) {
            uint32_t x = (i << 20) / GRID_WIDTH * sample_count / 2;
            display_values[i] = arm_linear_interp_q15(fft_mag, x, sample_count / 2);
        }

        /* Draw spectrum curve */
        CurveChart_DrawCurve(&chart, display_values, YELLOW);
        CurveChart_DrawImage(&chart, cursor_pos - 8, display_values[cursor_pos] + 16, 16, 16, arrow_pattern);

#ifdef GRAPH_USE_BACKBUFFER
        LCD_BackBuffer_Update();
#endif // GRAPH_USE_BACKBUFFER

        switch (ZLG7290_ReadKey())
        {
            case 1:
                freq_base = (freq_base + 1) % 4;
                UpdateSamplingArgs();
                UpdateFrequencyInfo();
                UpdateCursorInfo();
                break;

            case 2:
                energy_base = (energy_base + 1) % 4;
                scale_factor = pow10(energy_base) * 0.1f;
                break;

            case 3:
                CurveChart_RecoverRect(&chart, cursor_pos - 8, display_values[cursor_pos] + 16, 16, 16);
                cursor_pos = GRID_WIDTH / 2;
                break;

            case 4:
                MoveCursor(0);
                break;

            case 5:
                MoveCursor(1);
                break;

            case 9:
                LCD_DrawString("正在测量谐波", 24, GRID_X + 450, GRID_Y + GRID_HEIGHT + 24, RED);
                MeasureHarmonics();
                LCD_FillRect(GRID_X + 450, GRID_Y + GRID_HEIGHT + 24, 144, 24, BLACK);
                UpdateSamplingArgs();
                break;

            case 12:
                scale_factor -= pow10(energy_base) * 0.002f;
                break;

            case 13:
                scale_factor += pow10(energy_base) * 0.002f;
                break;

            case 33:
                ADS8694_Init();
                UpdateSamplingArgs();
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

void MeasureHarmonics(void)
{
    ADS8694_Init();
    sample_count = 2048;
    ADS8694_ConfigSampling(&adc_sample_buffer, sample_count, ADS8694_CHANNEL_0, INPUT_RANGE_BIPOLAR_0_625x);

    /** 1st Harmonic Frqeuency Measure **/
    /* Set sampling rate to 600Hz */
    ADS8694_SetSamplingRate(600);
    /* Get adc samples */
    ADS8694_StartSampling();

    /* Initialize FFT */
    arm_rfft_fast_init_f32(&rfft, sample_count);
    /* Using hamming window for frequency measure */
    GenerateWindowFunction(&window_func, sample_count, HAMMING_WINDOW);
    /* Offset and convert to float values */
    arm_offset_q31(adc_sample_buffer, -(1 << 17), adc_sample_buffer, sample_count);
    arm_q31_to_float(adc_sample_buffer, sample_values, sample_count);
    /* Apply window function */
    arm_mult_f32(sample_values, window_func, fft_input, sample_count);
    /* Apply FFT */
    arm_rfft_fast_f32(&rfft, fft_input, fft_output, 0);
    /* Compute spectrum magnitude */
    arm_cmplx_mag_f32(fft_output, fft_output, sample_count / 2);

    float threshold, max_val;
    uint32_t base_peak_index = 0;

    /* Set magnitude threshold */
    arm_mean_f32(fft_output + 4, 50, &threshold);
    threshold *= 100.0f;

    for (size_t i = 4; i < sample_count / 2 - 4; i++)
    {
        if (fft_output[i] > threshold) {
            arm_max_f32(fft_output + i, 32, &max_val, &base_peak_index);
            base_peak_index += i;
            break;
        }
    }

    //arm_max_f32(fft_output + 8, sample_count / 2 - 8, &max_val, &base_peak_index);

    /* Roughly estimate 1st harmonic frequency from the maximum value point */
    float base_freq = base_peak_index * 600.0f / sample_count;
    /* Compute amplitude scale factor (1 Vpp sinusoid in time domain ---> 1.0f) */
    float amp_factor = 0.013353f / (max_val
                                    + fft_output[base_peak_index - 2]
                                    + fft_output[base_peak_index - 1]
                                    + fft_output[base_peak_index + 1]
                                    + fft_output[base_peak_index + 2]);
    /* Frequency compensation using 4 neighbouring points around max value */
    base_freq += (fft_output[base_peak_index + 2] - fft_output[base_peak_index - 2]) * 39.43882f * amp_factor
        + (fft_output[base_peak_index + 1] - fft_output[base_peak_index - 1]) * 23.11291f * amp_factor;

    /* Update display value */
    if (base_freq < 100.0f) {
        sprintf(str_buffer, "%6.3fHz", base_freq);
    }
    else {
        sprintf(str_buffer, "%6.2fHz", base_freq);
    }
    LCD_FillRect(BASEFREQBOX_X + 20, BASEFREQBOX_Y + 36, 96, 32, BLACK);
    LCD_DrawString(str_buffer, 32, BASEFREQBOX_X + 20, BASEFREQBOX_Y + 36, LIGHTGRAY);

    //printf("%f Hz, Amp factor = %f\n", base_freq, amp_factor);

    /** Harmonics Amplitude Measure **/
    /* Set sampling rate to 2400Hz */
    ADS8694_SetSamplingRate(2400);
    /* Using flat top window for amplitude measure */
    GenerateWindowFunction(&window_func, sample_count, FLATTOP_WINDOW);
    /* Get adc samples */
    ADS8694_StartSampling();
    /* Offset and convert to float values */
    arm_offset_q31(adc_sample_buffer, -(1 << 17), adc_sample_buffer, sample_count);
    arm_q31_to_float(adc_sample_buffer, sample_values, sample_count);
    /* Apply window function */
    arm_mult_f32(sample_values, window_func, fft_input, sample_count);
    /* Apply FFT */
    arm_rfft_fast_f32(&rfft, fft_input, fft_output, 0);
    /* Compute spectrum magnitude */
    arm_cmplx_mag_f32(fft_output, fft_output, sample_count / 2);

    uint32_t peak_index;
    float peak_amp;
    uint16_t harmonic_colors[] = {
        RED, ORANGE, YELLOW, GREEN, OLIVE, PERRY, DODGERBLUE, MAGENTA, BROWN
    };

    /* Remap the first harmonic index under the higher sampling rate */
    base_peak_index /= 4;

    for (size_t i = 1; i <= 9; i++)
    {
        peak_amp = 0.0f;

        if (base_peak_index * i < sample_count / 2 - 4) {
            /* Search for harmonic peaks */
            arm_max_f32(fft_output + base_peak_index * i - 8, 16, &max_val, &peak_index);
            peak_index += base_peak_index * i - 8;
            /* Sum the neighboring values for total peak amplitude */
            for (size_t j = peak_index - 8; j < peak_index + 8; j++) {
                peak_amp += fft_output[j];
            }
#if DEBUG
            printf("Extra Gain = %u, Harmonic[%d] amplitude = %f\n", is_extra_gain, i, peak_amp);
#endif
            /* Map to time domain Vp */
            peak_amp *= 13288.666f;
            if (is_extra_gain) {
                peak_amp *= EXTRA_GAIN_FACTOR;
            }
        }

        /* Update display values */
        if (peak_amp < 1000.0f) {
            sprintf(str_buffer, "%7.3fmAp", peak_amp);
        }
        else {
            sprintf(str_buffer, "%7.5fAp", peak_amp * 0.001f);
        }
        LCD_FillRect(AMPBOX_X + 80, AMPBOX_Y + 16 + 22 * i, 80, 16, BLACK);
        LCD_DrawString(str_buffer, 16, AMPBOX_X + 80, AMPBOX_Y + 16 + 22 * i, harmonic_colors[i - 1]);
        //printf("harmonic%u_amp = %f\n", i, peak_amp);
    }

}

static void MoveCursor(_Bool right_left_select)
{
    static int16_t hit_count;

    CurveChart_RecoverRect(&chart, cursor_pos - 8, display_values[cursor_pos] + 16, 16, 16);

    if ((hit_count < 0 && right_left_select) || (hit_count > 0 && !right_left_select)) {
        hit_count = 0;
    }

    hit_count += (right_left_select) ? 1 : -1;
    cursor_pos += ((right_left_select) ? 1 : -1) + hit_count / 10;

    cursor_pos = (cursor_pos < 0) ? GRID_WIDTH - 1 : cursor_pos;
    cursor_pos = (cursor_pos >= GRID_WIDTH) ? 0 : cursor_pos;
    UpdateCursorInfo();
}

static inline void UpdateCursorInfo(void)
{
    switch (freq_base)
    {
        case DIV_50Hz: sprintf(str_buffer, "%3.1fHz", cursor_pos * 1.0f); break;
        case DIV_100Hz: sprintf(str_buffer, "%3.0fHz", cursor_pos * 2.0f); break;
        case DIV_500Hz: sprintf(str_buffer, "%3.1fkHz", cursor_pos * 0.01f); break;
        case DIV_1kHz: sprintf(str_buffer, "%3.1fkHz", cursor_pos * 0.02f); break;
        default:
            break;
    }

    LCD_FillRect(GRID_X + 320, GRID_Y + GRID_HEIGHT + 24, 160, 24, BLACK);
    LCD_DrawString(str_buffer, 24, GRID_X + 320, GRID_Y + GRID_HEIGHT + 24, GREEN);
}

static void UpdateSamplingArgs(void)
{
    sample_count = sample_count_values[freq_base];

    arm_rfft_fast_init_f32(&rfft, sample_count);
    GenerateWindowFunction(&window_func, sample_count, HAMMING_WINDOW);

    ADS8694_ConfigSampling(&adc_sample_buffer, sample_count, ADS8694_CHANNEL_0, INPUT_RANGE_BIPOLAR_0_625x);

    switch (freq_base)
    {
        case DIV_50Hz:
            ADS8694_SetSamplingRate(1200);
            break;
        case DIV_100Hz:
            ADS8694_SetSamplingRate(2400);
            break;
        case DIV_500Hz:
            ADS8694_SetSamplingRate(12000);
            break;
        case DIV_1kHz:
            ADS8694_SetSamplingRate(24000);
            break;
        default:
            break;
    }
}

static inline void UpdateFrequencyInfo(void)
{
    LCD_FillRect(GRID_X + 100, GRID_Y + GRID_HEIGHT + 24, 108, 24, BLACK);
    LCD_DrawString(freq_base_tag[freq_base], 24, GRID_X + 100, GRID_Y + GRID_HEIGHT + 24, CYAN);
}

static void GenerateWindowFunction(float *window, uint16_t length, uint8_t type)
{
    switch (type)
    {
        case RECTANGLE_WINDOW:
            for (uint16_t i = 0; i < length; i++) {
                window[i] = 1.0f;
            }
            break;

        case BARTLETT_WINDOW:
            for (uint16_t i = 0; i < length / 2; i++) {
                window[i] = 2.0f * i / length;
                window[i + length / 2] = 1.0f - 2.0f * i / length;
            }
            break;

        case HAMMING_WINDOW:
            for (uint16_t i = 0; i < length; i++) {
                window[i] = 0.54f - 0.46f * arm_cos_f32(2.0f * PI * i / length);
            }
            break;

        case BLACKMAN_WINDOW:
            for (uint16_t i = 0; i < length; i++) {
                window[i] = 0.42f
                    - 0.5f * arm_cos_f32(2.0f * PI * i / (length - 1))
                    + 0.08f * arm_cos_f32(4.0f * PI * i / (length - 1));
            }
            break;

        case FLATTOP_WINDOW:
            for (uint16_t i = 0; i < length; i++) {
                window[i] = 0.21557895f
                    - 0.41663158f * arm_cos_f32(2.0f * PI * i / (length - 1))
                    + 0.27726316f * arm_cos_f32(4.0f * PI * i / (length - 1))
                    - 0.08357895f * arm_cos_f32(6.0f * PI * i / (length - 1))
                    + 0.00694737f * arm_cos_f32(8.0f * PI * i / (length - 1));
            }
            break;
        default:
            break;
    }
}

static inline float pow10(uint8_t n)
{
    float val = 1.0f;
    while (n--) {
        val *= 10;
    }
    return val;
}