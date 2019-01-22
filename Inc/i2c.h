#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

extern I2C_HandleTypeDef hi2c1;
void I2C1_Init(void);

extern void Error_Handler(void);
