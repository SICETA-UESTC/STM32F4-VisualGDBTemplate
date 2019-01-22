#pragma once
#include <stm32f4xx_hal.h>

#define CONVST0		GPIOA->BSRR = (uint32_t)GPIO_PIN_11 << 16U
#define CONVST1		GPIOA->BSRR = GPIO_PIN_11

#define RESET0		GPIOD->BSRR = (uint32_t)GPIO_PIN_2 << 16U
#define RESET1		GPIOD->BSRR = GPIO_PIN_2

#define RD0			GPIOD->BSRR = (uint32_t)GPIO_PIN_3 << 16U
#define RD1			GPIOD->BSRR = GPIO_PIN_3

#define CS0			GPIOD->BSRR = (uint32_t)GPIO_PIN_6 << 16U
#define CS1			GPIOD->BSRR = GPIO_PIN_6

#define AD7606_SAMPLE_COUNT		512
#define AD7606_VOLTAGE_REF		4.5f

void AD7606_Init(void);
void AD7606_Sampling(uint32_t samplingFreq, int16_t *channel0, int16_t *channel1, uint16_t count);

static inline void AD7606_Reset(void);

