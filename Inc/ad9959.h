#pragma once
#include <stm32f4xx_hal.h>

//片选
#define CS_HIGH			GPIOE->BSRR = GPIO_PIN_5
#define CS_LOW			GPIOE->BSRR = (uint32_t)GPIO_PIN_5 << 16U

//重置
#define RST_HIGH		GPIOG->BSRR = GPIO_PIN_14
#define RST_LOW			GPIOG->BSRR = (uint32_t)GPIO_PIN_14 << 16U

//IO更新
#define UP_HIGH			GPIOE->BSRR = GPIO_PIN_4
#define UP_LOW			GPIOE->BSRR = (uint32_t)GPIO_PIN_4 << 16U

//配置端口
#define P0_HIGH			GPIOF->BSRR = GPIO_PIN_6
#define P0_LOW			GPIOF->BSRR = (uint32_t)GPIO_PIN_6 << 16U

#define P1_HIGH			GPIOF->BSRR = GPIO_PIN_7
#define P1_LOW			GPIOF->BSRR = (uint32_t)GPIO_PIN_7 << 16U

#define P2_HIGH			GPIOE->BSRR = GPIO_PIN_2
#define P2_LOW			GPIOE->BSRR = (uint32_t)GPIO_PIN_2 << 16U

#define P3_HIGH			GPIOE->BSRR = GPIO_PIN_3
#define P3_LOW			GPIOE->BSRR = (uint32_t)GPIO_PIN_3 << 16U

//串口时钟同步
#define SCLK_HIGH		GPIOE->BSRR = GPIO_PIN_6
#define SCLK_LOW		GPIOE->BSRR = (uint32_t)GPIO_PIN_6 << 16U

//串口IO
#define D0_HIGH			GPIOB->BSRR = GPIO_PIN_6
#define D0_LOW			GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16U

#define D1_HIGH			GPIOB->BSRR = GPIO_PIN_7
#define D1_LOW			GPIOB->BSRR = (uint32_t)GPIO_PIN_7 << 16U

#define D2_HIGH			GPIOG->BSRR = GPIO_PIN_15
#define D2_LOW			GPIOG->BSRR = (uint32_t)GPIO_PIN_15 << 16U

#define D3_HIGH			GPIOB->BSRR = GPIO_PIN_5
#define D3_LOW			GPIOB->BSRR = (uint32_t)GPIO_PIN_5 << 16U
 

//应用参考值
#define CRYSTAL_FREQ	25000000U

#define FREQ_REF		8.5904963602764		//频率参考
#define PHASE_REF		45.511111111111		//相位参考

typedef enum {
	CSR, FR1, FR2, CFR, CFTW0, CPOW0, ACR, LSRR, RDW, FDW,
	CW1, CW2, CW3, CW4, CW5, CW6, CW7, CW8, CW9, CW10, CW11, CW12, CW13, CW14, CW15, 
} AD9559_Reg;

typedef enum {
	AD9959_CHANNEL_0 = 0x10,
	AD9959_CHANNEL_1 = 0x20,
	AD9959_CHANNEL_2 = 0x40,
	AD9959_CHANNEL_3 = 0x80,
} AD9559_Channel;

#define ASK_MOD 0x400000
#define FSK_MOD 0x800000
#define PSK_MOD 0xc00000

#define LEVEL_16 0x0000300
#define LEVEL_8  0x0000200
#define LEVEL_4  0x0000100
#define LEVEL_2  0x0000000

void AD9959_Init(void);
void AD9959_Reset(void);
void AD9959_SetFreq(uint8_t channel, uint32_t freq);
void AD9959_SetPhase(uint8_t channel, float phase);
void AD9959_SetAmp(uint8_t channel, uint16_t amp);

//void AD9959_SingleOutput(uint8_t channel, uint32_t freq, float phase, uint16_t amp);

void AD9959_SweepFreq(
	uint8_t channel, uint16_t amp,
	uint32_t freqStart, uint32_t freqEnd, uint32_t freqStep,
	uint16_t timeStep);

static void WriteReg(uint8_t reg, uint32_t data);
static inline void AD9959_Update(void);