/* Prevent recursive inclusion -------------------------------------*/
#pragma once
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

extern SD_HandleTypeDef hsd;
void SDIO_SD_Init(void);
