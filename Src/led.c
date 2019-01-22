#include "led.h"

static const uint16_t led_pins[] = { LED0_GPIO_PIN, LED1_GPIO_PIN };

void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable the GPIO_LED Clock */
    LED_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = LED_ALL_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_ALL_GPIO_PIN, LED_GPIO_OFF_STATE);
}

void LED_DeInit(void)
{
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_ALL_GPIO_PIN, LED_GPIO_OFF_STATE);
    HAL_GPIO_DeInit(LED_GPIO_PORT, LED_ALL_GPIO_PIN);
}

void LED_On(uint8_t select)
{
    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pins[select], LED_GPIO_ON_STATE);
}

void LED_Off(uint8_t select)
{
    HAL_GPIO_WritePin(LED_GPIO_PORT, led_pins[select], LED_GPIO_OFF_STATE);
}

void LED_Toggle(uint8_t select)
{
    HAL_GPIO_TogglePin(LED_GPIO_PORT, led_pins[select]);
}