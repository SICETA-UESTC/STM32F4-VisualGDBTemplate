#include "pe4302.h"

//extern UART_HandleTypeDef huart3;

void PE4302_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}

void PE4302_SetAttenuation(uint8_t twoTimes_dB)
{
    twoTimes_dB = (twoTimes_dB > 63) ? 63 : twoTimes_dB;

    LATCH_HIGH;
    GPIOC->ODR = twoTimes_dB;
    LATCH_LOW;
}