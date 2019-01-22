#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

void DelayUs(uint16_t us);

void TIM2_Init(void);
void TIM3_Init(void);
void TIM5_Init(void);
void TIM6_Init(void);
void TIM7_Init(void);
void TIM13_PWM_Output_Init(uint16_t period);

extern void Error_Handler(void);

