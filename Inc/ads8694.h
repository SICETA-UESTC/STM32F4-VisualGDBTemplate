#pragma once
#include <stm32f4xx_hal.h>

#define REF_VOLT			4096		//mV

#define ADS8694_CS0			GPIOB->BSRR = (uint32_t)GPIO_PIN_11 << 16U
#define ADS8694_CS1			GPIOB->BSRR = GPIO_PIN_11

#define ADS8694_RST0		GPIOG->BSRR = (uint32_t)GPIO_PIN_7 << 16U
#define ADS8694_RST1		GPIOG->BSRR = GPIO_PIN_7

typedef enum {
	NO_OP		= 0x00,		// Continued Operation
	STDBY		= 0x82,		// Standby
	PWR_DN		= 0x83,		// Power Down
	RST			= 0x85,		// Reset program registers
	AUTO_RST	= 0xA0,		// Auto Channel x Sequence with Reset
	MAN_CH0		= 0xC0,		// Manual Channel 0 Selection
	MAN_CH1		= 0xC4,		// Manual Channel 1 Selection
	MAN_CH2		= 0xC8,		// Manual Channel 2 Selection
	MAN_CH3		= 0xCC,		// Manual Channel 3 Selection
	MAN_AUX		= 0xE0,		// Manual Aux Selection
} ADS8694_Cmd;

typedef enum {
	INPUT_RANGE_BIPOLAR_2_5x	= 0x00,
	INPUT_RANGE_BIPOLAR_1_25x	= 0x01,
	INPUT_RANGE_BIPOLAR_0_625x	= 0x02,
	INPUT_RANGE_UNIPOLAR_2_5x	= 0x05,
	INPUT_RANGE_UNIPOLAR_1_25x	= 0x06,
} ADS8694_InputRange;

#define ADS8694_CHANNEL_0		0x01
#define ADS8694_CHANNEL_1		0x02
#define ADS8694_CHANNEL_2		0x04
#define ADS8694_CHANNEL_3		0x08
#define ADS8694_CHANNEL_All		0xFF

void ADS8694_Init(void);
void ADS8694_ConfigSampling(int32_t *pBuffer, uint32_t count, uint8_t channel, uint8_t inputRange);
void ADS8694_SetSamplingRate(uint32_t samplingRate);
void ADS8694_StartSampling(void);

static void ADS8694_SendCmd(uint8_t cmd);
static void ADS8694_WriteReg(uint8_t addr, uint8_t data);
static uint8_t ADS8694_ReadReg(uint8_t addr);
static inline void ADS8694_Reset(void);

extern uint8_t SPI2_RW_Byte(uint8_t data);
