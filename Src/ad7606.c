#include "ad7606.h"
#include "tim.h"
#include <arm_math.h>

extern TIM_HandleTypeDef htim13;
static uint16_t offset = 0;
static __IO int16_t value[8];

static _Bool is_period_complete;
static int16_t *ad7606_channel_0;
static int16_t *ad7606_channel_1;

void AD7606_Init(void)
{
    /* Parallel 16-bit Data Port */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* RESET, RD, CS */
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* FRSTDATA */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* BUSY */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void AD7606_Sampling(uint32_t samplingFreq, int16_t *channel0, int16_t *channel1, uint16_t count)
{
    ad7606_channel_0 = channel0;
    ad7606_channel_1 = channel1;

    AD7606_Reset();
    RD1;

    HAL_TIM_PWM_DeInit(&htim13);
    TIM13_PWM_Output_Init(1000000U / samplingFreq);
    HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);

    while (!is_period_complete) {
        DelayUs(10);
    }

    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    /*
    arm_q15_to_float(ad7606_channel_0, channel0, count);
    arm_q15_to_float(ad7606_channel_1 + 1, channel1, count);	//ÏàÎ»²¹³¥ 1

    arm_scale_f32(channel0, AD7606_VOLTAGE_REF, channel0, count);
    arm_scale_f32(channel1, AD7606_VOLTAGE_REF, channel1, count);
    */
}


static inline void AD7606_Reset(void)
{
    RESET0;
    RESET1;
    DelayUs(1);
    RESET0;
    is_period_complete = 0;
    offset = 0;
}

void EXTI9_5_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_7) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);

        if (offset >= AD7606_SAMPLE_COUNT)
        {
            HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1);
            is_period_complete = 1;
            return;
        }

        CS0;
        __NOP();
        RD0;
        while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9) == RESET);

        for (uint8_t i = 0; i < 8; i++)
        {
            RD0;
            __NOP(); __NOP(); __NOP(); __NOP();
            value[i] = GPIOC->IDR;
            RD1;
        }
        CS1;

#ifdef DEBUG
        ad7606_channel_0[offset] = value[7];
        ad7606_channel_1[offset] = value[6];
#else
        ad7606_channel_0[offset] = value[7];
        ad7606_channel_1[offset] = value[6];
#endif
        ++offset;
    }
}