#pragma once
#include <stm32f4xx_hal.h>

#define LMH6518_READ	0x80
#define LMH6518_WRITE	0x00

typedef enum {
	PREAMP_HG, PREAMP_LG
} LMH6518_PreAmpMode;

typedef enum {
	BD_FULL, BD_20MHz, BD_100MHz, BD_200MHz, BD_350MHz, BD_650MHz, BD_750MHz,
} LMH6518_FilterBandwith;

typedef enum {
	LA_0dB, LA_2dB, LA_4dB, LA_6dB, LA_8dB, LA_10dB, LA_12dB, LA_14dB, LA_16dB, LA_18dB, LA_20dB,
} LMH6518_LadderAttenuation;

typedef union {
	/* PGA Register */
	struct {
		unsigned	Attenuation		: 4;
		_Bool		PreAmp			: 1;
		_Bool						: 1;
		unsigned	Filter			: 3;
		_Bool						: 1;
		_Bool		AuxHighZ		: 1;
		unsigned					: 5;
	} DataField;
	/* SPI ReadWrite Value */
	uint8_t Bytes[2];
} LMH6518_TypeDef;

void LMH6518_SetAuxOutput(_Bool isEnabled);
void LMH6518_PreAmp(LMH6518_PreAmpMode mode);
void LMH6518_SetBandWidth(LMH6518_FilterBandwith bandwith);
void LMH6518_SetAttenuation(LMH6518_LadderAttenuation attenuation);

static inline void LMH6518_WriteData(void);



