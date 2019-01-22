#include "ads8694.h"
#include "spi.h"
#include "tim.h"
#include "lcd.h"

extern TIM_HandleTypeDef htim2;
extern SPI_HandleTypeDef hspi2;

int32_t *sample_buffer;
uint32_t sample_count;
uint32_t sample_index;

void ADS8694_Init(void)
{
    /* PCLK1 / 4 = 10.5 MHz */
    SPI2_Init();
    SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_4);
    GPIO_InitTypeDef GPIO_InitStruct;
    /* Chip Select GPIO Init */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* Reset GPIO Init */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    ADS8694_Reset();
}

void ADS8694_ConfigSampling(int32_t *pBuffer, uint32_t count, uint8_t channel, uint8_t inputRange)
{
    // Set buffer pointer
    sample_buffer = pBuffer;
    sample_count = count;
    // Unused channels power down
    ADS8694_WriteReg(0x02, (~channel) & 0x0F);

    // Set input range
    for (uint8_t i = 0; i < 4; i++) {
        if (channel & 1) {
            ADS8694_WriteReg(0x05 + i, inputRange);
        }
        channel >>= 1;
    }

    if (channel & (channel - 1) == 0) {
        // Set single channel manual selection
        switch (channel)
        {
            case ADS8694_CHANNEL_0: ADS8694_SendCmd(MAN_CH0); break;
            case ADS8694_CHANNEL_1: ADS8694_SendCmd(MAN_CH1); break;
            case ADS8694_CHANNEL_2: ADS8694_SendCmd(MAN_CH2); break;
            case ADS8694_CHANNEL_3: ADS8694_SendCmd(MAN_CH3); break;
            default:
                break;
        }
    }
    else {
        //Set auto scan sequence
        ADS8694_WriteReg(0x01, channel);
        ADS8694_SendCmd(AUTO_RST);
    }

    TIM2_Init();
}

void ADS8694_SetSamplingRate(uint32_t samplingRate)
{
    __HAL_TIM_SET_AUTORELOAD(&htim2, (12000000U / samplingRate) - 1);
}

void ADS8694_StartSampling(void)
{
    /* 点个LED压压惊 */
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, SET);
    sample_index = 0;
    /* 产生CS信号开始采样 */
    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);
    /* 就...等着它采完啊<CPU:好无聊QAQ> */
    while (sample_index < sample_count) {
        __NOP();
    }
    /* 灭掉LED */
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, RESET);
}

/*
void ADS8694_StartSampling(int32_t *pBuffer, uint32_t count)
{
    int32_t adc_code = 0;
    for (uint32_t i = 0; i < count; i++)
    {
        //Start data frame
        ADS8694_CS0;
        //Send 0x0000 for continuous mode
        SPI_ReadWriteByte(&hspi2, 0x00);
        SPI_ReadWriteByte(&hspi2, 0x00);
        //Read 18-bit ADC Code
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF) << 10;
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF) << 2;
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF);
        pBuffer[i] = adc_code;
        //Pull CS to HIGH for at least 30ns
        ADS8694_CS1;
        __NOP();
    }
}*/

static void ADS8694_SendCmd(uint8_t cmd)
{
    /* Start command frame */
    ADS8694_CS0;
    /* Send command */
    SPI_ReadWriteByte(&hspi2, cmd);
    SPI_ReadWriteByte(&hspi2, 0x00);
    /* Dummy read to form a complete frame */
    SPI_ReadWriteByte(&hspi2, 0xFF);
    SPI_ReadWriteByte(&hspi2, 0xFF);
    SPI_ReadWriteByte(&hspi2, 0xFF);
    /* Pull CS to HIGH for at least 30ns */
    ADS8694_CS1;
    __NOP();
}

static void ADS8694_WriteReg(uint8_t addr, uint8_t data)
{
    union {
        struct {
            unsigned Data : 8;
            _Bool WR : 1;
            unsigned Addr : 7;
        } Reg;
        uint8_t Bytes[2];
    } write_data;

    write_data.Reg.WR = 1;
    write_data.Reg.Addr = addr;
    write_data.Reg.Data = data;

    /* Start write frame */
    ADS8694_CS0;
    /* Write program register */
    SPI_ReadWriteByte(&hspi2, write_data.Bytes[1]);
    SPI_ReadWriteByte(&hspi2, write_data.Bytes[0]);
    /* Read the data just been written for check */
    uint8_t echo_data = SPI_ReadWriteByte(&hspi2, 0xFF);
    /* Pull CS to HIGH for at least 30ns */
    ADS8694_CS1;
    __NOP();
}

static uint8_t ADS8694_ReadReg(uint8_t addr)
{
    /* Start read frame */
    ADS8694_CS0;
    /* Write program register */
    SPI_ReadWriteByte(&hspi2, addr << 1);
    /* Dummy write */
    SPI_ReadWriteByte(&hspi2, 0xFF);
    /* Read the data just been written for check */
    uint8_t read_data = SPI_ReadWriteByte(&hspi2, 0xFF);
    /* Pull CS to HIGH for at least 30ns */
    ADS8694_CS1;
    __NOP();
    return read_data;
}


static inline void ADS8694_Reset(void)
{
    ADS8694_RST0;
    __NOP();
    ADS8694_RST1;
}

void TIM2_IRQHandler(void)
{
    int32_t adc_code = 0;
    if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_CC4) != RESET) {
        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC4);
        // Send 0x0000 for continuous mode
        SPI_ReadWriteByte(&hspi2, 0x00);
        SPI_ReadWriteByte(&hspi2, 0x00);
        // Read 18-bit ADC Code
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF) << 10;
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF) << 2;
        adc_code |= SPI_ReadWriteByte(&hspi2, 0xFF);
        sample_buffer[sample_index++] = adc_code;

        if (sample_index >= sample_count) {
            HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_4);
        }
    }
}