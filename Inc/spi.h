#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

void SPI2_Init(void);
void SPI2_SetSpeed(uint16_t BaudRatePrescaler);
uint8_t SPI2_RW_Byte(uint8_t data);

extern void Error_Handler(void);
