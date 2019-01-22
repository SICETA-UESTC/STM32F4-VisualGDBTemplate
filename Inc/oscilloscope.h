#pragma once
#include <stm32f4xx_hal.h>

#define CLAMP(X, LOW, HIGH)  (((X) > (HIGH)) ? (HIGH) : (((X) < (LOW)) ? (LOW) : (X)))
#define SAMPLE_COUNT		2048
#define VOLT_FACTOR			0.0063463f
#define EXTRA_GAIN_FACTOR	0.12700467f
//#define VOLT_FACTOR		0.00634431f

#define GRID_X				15
#define GRID_Y				15
#define GRID_WIDTH			600
#define GRID_HEIGHT			400

#define TIMEBOX_X			GRID_X + GRID_WIDTH + 10
#define TIMEBOX_Y			GRID_Y - 1
#define TIMEBOX_WIDTH		168
#define TIMEBOX_HEIGHT		127

#define VOLTBOX_X			GRID_X + GRID_WIDTH + 10
#define VOLTBOX_Y			TIMEBOX_Y + TIMEBOX_HEIGHT + 10
#define VOLTBOX_WIDTH		168
#define	VOLTBOX_HEIGHT		127

#define TRIGBOX_X			GRID_X + GRID_WIDTH + 10
#define TRIGBOX_Y			VOLTBOX_Y + VOLTBOX_HEIGHT + 10
#define TRIGBOX_WIDTH		168
#define	TRIGBOX_HEIGHT		67

#define INPUTBOX_X			GRID_X + GRID_WIDTH + 10
#define INPUTBOX_Y			TRIGBOX_Y + TRIGBOX_HEIGHT + 10
#define INPUTBOX_WIDTH		168
#define INPUTBOX_HEIGHT		150

typedef enum {
	DIV_1ms, DIV_5ms, DIV_10ms, DIV_50ms,
} TimeBase;

typedef enum {
	DIV_5mV, DIV_10mV, DIV_50mV, DIV_100mV, DIV_500mV, DIV_1V,
} VoltBase;

typedef enum {
	DC_Coupling, AC_Coupling
} Coupling;

typedef enum {
	X1, X10,
} ProbeAttenuation;

typedef struct {
	_Bool Coupling;
	_Bool ProbeAttenuation;
	uint8_t Gain;
	uint8_t TimeBase;
	int16_t TimeOffset;
	uint8_t VoltBase;
	int32_t VoltOffset;
	int32_t TriggerVolt;
	float DisplayScaleFactor;
} OscArgs_TypeDef;

static const uint8_t *time_base_tag[4] = { "1ms/div", "5ms/dive", "10ms/div", "50ms/div" };
static const uint8_t *volt_base_tag[6] = { "5mA/div", "10mA/div", "50mA/div", "100mA/div", "500mA/div", "1A/div" };

void Oscilloscope_Init(void);
void Oscilloscope_Start(void);

static void AdjustVerticalPos(_Bool up_down_select);
static inline void UpdateVerticalPosInfo(void);

static void AdjustHorizontalPos(_Bool right_left_select);
static inline void UpdateHorizontalPosInfo(void);

static void AdjustTriggerVoltage(_Bool up_down_select);
static inline void ConfigSamplingArgs(void);

//ZLG7290 KeyBoard Driver
extern void ZLG7290_Init(void);
extern uint8_t ZLG7290_ReadKey(void);
//On Chip ADC
extern void ADC1_Init(void);
//Delay
extern void HAL_Delay(uint32_t ms);
