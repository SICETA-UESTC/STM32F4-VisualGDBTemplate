#pragma once
#include <stm32f4xx_hal.h>

#define LED_GPIO_ON_STATE       RESET
#define LED_GPIO_OFF_STATE      SET

#define LED_GPIO_PORT           GPIOF
#define LED_GPIO_CLK_ENABLE     __HAL_RCC_GPIOF_CLK_ENABLE

#define LED0_GPIO_PIN           GPIO_PIN_9
#define LED1_GPIO_PIN           GPIO_PIN_10
#define LED_ALL_GPIO_PIN        LED0_GPIO_PIN | LED1_GPIO_PIN

typedef enum {
    LED_0, LED_1,
} LedTags;


void LED_Init(void);
void LED_On(uint8_t select);
void LED_Off(uint8_t select);
void LED_Toggle(uint8_t select);
