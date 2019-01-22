/**
  ******************************************************************************
  * @file       fsmc.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      FSMC initialization and address mapping
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

/* Public Marcos -------------------------------------------------------------*/

/* LCD address mapping */
#define FSMC_LCD_BANK               FSMC_NORSRAM_BANK4  /* Bank select NE[1-4] */
#define FSMC_LCD_REG_SELECT         6U                  /* Register select A[0-25] */

#define FSMC_LCD_BASE_ADDR          (0x60000000U | (FSMC_LCD_BANK << 25))
#define FSMC_LCD_REG_ADDR			FSMC_LCD_BASE_ADDR 	/* LCD command address */
#define FSMC_LCD_DATA_ADDR			(FSMC_LCD_BASE_ADDR + (1 << (FSMC_LCD_REG_SELECT + 1))) /* LCD data address */

/* SRAM address mapping */
#define FSMC_SRAM_BANK              FSMC_NORSRAM_BANK3  /* Bank select NE[1-4] */
#define FSMC_SRAM_BASE_ADDR		    (0x60000000U | (FSMC_SRAM_BANK << 25))  /* External sram data address */

/* Public function prototypes ------------------------------------------------*/
void FSMC_Init(void);

/* Private function prototypes -----------------------------------------------*/
static void HAL_FSMC_MspInit(void);

/* External function prototypes ----------------------------------------------*/
extern void Error_Handler(void);
