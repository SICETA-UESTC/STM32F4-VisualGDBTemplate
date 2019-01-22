#include "ad9959.h"

static uint8_t reg_length[25] = { 1,3,2,3,4,2,3,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4 };

void AD9959_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    AD9959_Reset();
    WriteReg(FR1, 0xD00000);
}

void AD9959_Reset(void)
{
    RST_LOW;
    DelayUs(50);
    RST_HIGH;
    DelayUs(50);
    RST_LOW;
}

/*
void AD9959_SingleOutput(uint8_t channel, uint32_t freq, float phase, uint16_t amp)
{
    AD9959_SetFreq(channel, freq);
    AD9959_SetPhase(channel, phase);
    AD9959_SetAmp(channel, amp);
    AD9959_Update();
}*/

void AD9959_SweepFreq(
    uint8_t channel, uint16_t amp,
    uint32_t freqStart, uint32_t freqEnd, uint32_t freqStep,
    uint16_t timeStep)
{
    WriteReg(CSR, channel);
    AD9959_SetAmp(channel, amp);

    WriteReg(CFR, 0x804314);
    WriteReg(FR1, 0xD00000);
    WriteReg(CFTW0, (uint32_t)(freqStart * FREQ_REF));
    WriteReg(CW1, (uint32_t)(freqEnd * FREQ_REF));
    WriteReg(RDW, (uint32_t)(freqStep * FREQ_REF));
    WriteReg(LSRR, (uint16_t)(timeStep * 0.4f));

    AD9959_Update();

    /*
        P3_LOW;
        DelayUs(50);
        P3_HIGH;
        DelayUs(50);
        P3_LOW;
    */
}

void AD9959_SetFreq(uint8_t channel, uint32_t freq)
{
    WriteReg(CSR, channel);
    WriteReg(CFTW0, (uint32_t)(freq * FREQ_REF));
    AD9959_Update();
}

void AD9959_SetPhase(uint8_t channel, float phase)
{
    WriteReg(CSR, channel);
    WriteReg(CPOW0, (uint32_t)(phase * PHASE_REF));
    AD9959_Update();
}

void AD9959_SetAmp(uint8_t channel, uint16_t amp)
{
    //	WriteReg(CSR, channel, 0);
    WriteReg(ACR, 0x00001000 + amp);
    AD9959_Update();
}

static void WriteReg(uint8_t reg, uint32_t data)
{
    uint8_t nBytes = reg_length[reg];
    uint8_t temp = reg;

    CS_LOW;
    SCLK_LOW;

    for (uint8_t i = 0; i < 8; i++)
    {
        if (temp & 0x80) D0_HIGH;
        else D0_LOW;

        SCLK_HIGH;
        temp <<= 1;
        SCLK_LOW;
    }

    for (uint8_t i = 0; i < nBytes; i++)
    {
        temp = (data >> ((nBytes - 1 - i) << 3)) & 0xFF;

        for (uint8_t j = 0; j < 8; j++)
        {
            if (temp & 0x80) D0_HIGH;
            else D0_LOW;

            SCLK_HIGH;
            temp <<= 1;
            SCLK_LOW;
        }
    }

    if (reg == FR1) {
        AD9959_Update();
    }

    CS_HIGH;
}

static inline void AD9959_Update(void)
{
    UP_LOW;
    DelayUs(1);
    UP_HIGH;
    DelayUs(1);
    UP_LOW;
}