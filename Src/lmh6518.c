#include "lmh6518.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi2;
static LMH6518_TypeDef lmh6518;

void LMH6518_SetAuxOutput(_Bool isEnabled)
{
    lmh6518.DataField.AuxHighZ = !isEnabled;
    LMH6518_WriteData();
}

void LMH6518_PreAmp(LMH6518_PreAmpMode mode)
{
    lmh6518.DataField.PreAmp = mode;
    LMH6518_WriteData();
}

void LMH6518_SetBandWidth(LMH6518_FilterBandwith bandwith)
{
    lmh6518.DataField.Filter = bandwith;
    LMH6518_WriteData();
}

void LMH6518_SetAttenuation(LMH6518_LadderAttenuation attenuation)
{
    lmh6518.DataField.Attenuation = attenuation;
    LMH6518_WriteData();
}

static inline void LMH6518_WriteData(void)
{
    SPI2_RW_Byte(LMH6518_WRITE);
    SPI2_RW_Byte(lmh6518.Bytes[0]);
    SPI2_RW_Byte(lmh6518.Bytes[1]);
}
