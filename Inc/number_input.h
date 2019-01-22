#pragma once
#include <stm32f4xx_hal.h>

#define NUMINPUT_X		30
#define NUMINPUT_Y		440

#define DIGITBOX_X		NUMINPUT_X + 180
#define DIGITBOX_Y		NUMINPUT_Y

_Bool GetInputFloat(float *inputVal);
_Bool GetInputInt(uint32_t *inputVal);

static _Bool GetUserInput(void);
static inline void NumberInput(uint8_t num);
static inline void DotInput(void);
static inline void BackSpace(void);
