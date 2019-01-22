#pragma once
#include <stm32f4xx_hal.h>

#define LATCH_HIGH	GPIOF->BSRR = GPIO_PIN_8
#define LATCH_LOW	GPIOF->BSRR = (uint32_t)GPIO_PIN_8 << 16U

void PE4302_Init(void);
void PE4302_SetAttenuation(uint8_t twoTimes_dB);