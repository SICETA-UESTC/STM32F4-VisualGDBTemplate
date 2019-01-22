#pragma once
#include <stm32f4xx_hal.h>

#define SRAM_SIZE			0x1024000U		//SRAM size (Bytes)

void SRAM_WriteBytes(uint32_t offset, uint8_t* src, uint32_t count);
void SRAM_ReadBytes(uint32_t offset, uint8_t* dst, uint32_t count);
