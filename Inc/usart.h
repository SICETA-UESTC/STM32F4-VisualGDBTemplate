#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

#define UART_RX_BUFFER_SIZE 1024

size_t USART1_FIFO_ReadBytes(uint8_t *buffer, size_t length);

extern void Error_Handler(void);
