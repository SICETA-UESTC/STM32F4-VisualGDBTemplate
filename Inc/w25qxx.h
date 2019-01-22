#pragma once
#include <stm32f4xx_hal.h>

#define ENABLE_W25QXX_CS	GPIOB->BSRR = (uint32_t)GPIO_PIN_14 << 16U
#define DISABLE_W25QXX_CS	GPIOB->BSRR = GPIO_PIN_14

#define  PAGE_SIZE  256

#define DUMMY		0xFF
#define W25Q16		0XEF14
#define W25Q32		0XEF15
#define W25Q64		0XEF16
#define W25Q128		0XEF17

void W25QXX_Init(void);
void W25QXX_PageWrite(uint32_t, uint8_t*, uint16_t);
void W25QXX_Write(uint32_t, uint8_t*, uint32_t);
void W25QXX_Read(uint32_t, uint8_t*, uint32_t);
void W25QXX_SectorErase(uint32_t);
void W25QXX_BlockErase(uint32_t);

static void ReadID(void);
static void WaitForWriteEnd(void);
static void WriteEnable(void);
static void PowerDown(void);
static void WakeUp(void);

extern void DelayUs(uint32_t);
