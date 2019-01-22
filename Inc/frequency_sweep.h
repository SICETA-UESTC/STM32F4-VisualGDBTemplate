#pragma once
#include <stm32f4xx_hal.h>
#include "ad9959.h"

#define	OUTPUT_CHANNEL		AD9959_CHANNEL_1

#define ADC_SAMPLE_COUNT	8
#define MAX_SAMPLE_COUNT	2048
#define MIN_SAMPLE_COUNT	100

#define GRID_X				40
#define GRID_Y				10
#define GRID_WIDTH			500
#define GRID_HEIGHT			400

#define FREQBOX_X			GRID_X + GRID_WIDTH + 8
#define FREQBOX_Y			GRID_Y - 1
#define FREQBOX_WIDTH		242
#define FREQBOX_HEIGHT		104

#define AMPBOX_X			FREQBOX_X
#define AMPBOX_Y			FREQBOX_Y + FREQBOX_HEIGHT + 8
#define AMPBOX_WIDTH		FREQBOX_WIDTH
#define AMPBOX_HEIGHT		56

#define CURSORBOX_X			FREQBOX_X
#define CURSORBOX_Y			AMPBOX_Y + AMPBOX_HEIGHT + 8
#define CURSORBOX_WIDTH		FREQBOX_WIDTH
#define CURSORBOX_HEIGHT	225

#define STATUS_LED(X)		GPIOF->BSRR = (uint32_t)GPIO_PIN_10 << ((X) ? 16 : 0)

static const uint16_t amp_table[][2] =
{
    { 15, 1018 },	//100mV
    { 16, 1020 },	//98mV
    { 16, 1000 },	//96mV
    { 16, 975 },	//94mV
    { 17, 1010 },	//92mV
    { 17, 985 },	//90mV
    { 18, 1015 },	//88mV
    { 18, 990 },	//86mV
    { 19, 1023 },	//84mV
    { 19, 1003 },	//82mV
    { 19, 971 },	//80mV
    { 19, 945 },	//78mV
    { 20, 1018 },	//76mV
    { 20, 992 },	//74mV
    { 21, 1018 },	//72mV
    { 21, 991 },	//70mV
    { 22, 1015 },	//68mV
    { 22, 983 },	//66mV
    { 23, 1008 },	//64mV
    { 23, 974 },	//62mV
    { 24, 1010 },	//60mV
    { 24, 978 },	//58mV
    { 25, 994 },	//56mV
    { 26, 1012 },	//54mV
    { 27, 1023 },	//52mV
    { 27, 985 },	//50mV
    { 28, 1020 },	//48mV
    { 28, 980 },	//46mV
    { 29, 990 },	//44mV
    { 30, 990 },	//42mV
    { 32, 986 },	//40mV
    { 33, 1023 },	//38mV
    { 34, 1017 },	//36mV
    { 35, 1015 },	//34mV
    { 35, 956 },	//32mV
    { 36, 985 },	//30mV
    { 38, 1023 },	//28mV
    { 39, 1007 },	//26mV
    { 40, 1001 },	//24mV
    { 42, 1023 },	//22mV
    { 43, 980 },	//20mV
    { 45, 1023 },	//18mV
    { 47, 1012 },	//16mV
    { 50, 1007 },	//14mV
    { 52, 995 },	//12mV
    { 56, 1023 },	//10mV
    { 59, 978 },	//8mV
    { 63, 942 },	//6mV
    { 63, 625 },	//4mV
};

void FreqSweep_Init(void);
void FreqSweep_Start(void);
//void FreqPoint_Output(void);

static void FreqSweepAndSampling(void);
static void SetFreqParameters(void);
static void UpdateFreqInfoDispaly(void);
static void UpdateOutputAmp(void);
//static void GetCodeTable(void);

//Inline Functions
static inline void FreqParametersDisplay(uint8_t i, _Bool isSlected);
static inline void CursorParametersDisplay(void);

//ZLG7290 Keyboard Driver
extern void ZLG7290_Init();
extern uint8_t ZLG7290_ReadKey();

//PE4302 Attenuator
extern void PE4302_Init(void);
extern void PE4302_SetAttenuation(uint8_t twoTimes_dB);
//AD9959 DDS
extern void AD9959_Init(void);
extern void AD9959_SetFreq(uint8_t channel, uint32_t freq);
extern void AD9959_SetAmp(uint8_t channel, uint16_t amp);
//On Chip ADC
extern void ADC1_Init();
//Delay
extern void HAL_Delay(uint32_t ms);
