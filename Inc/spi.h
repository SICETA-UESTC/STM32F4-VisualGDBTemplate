#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

void SPI2_Init(void);
void SPI_SetSpeed(SPI_HandleTypeDef* spi_handle, uint16_t baudrate_prescaler);
uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* spi_handle, uint8_t data);

extern void Error_Handler(void);
