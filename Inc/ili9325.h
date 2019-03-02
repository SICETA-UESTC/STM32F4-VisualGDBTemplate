/**
  ******************************************************************************
  * @file       ili9325.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.22
  * @brief      ILI9325 TFT LCD low-level driver based on FSMC parallel port
  *
  * @note       You need to initialize FSMC in advance to use the driver
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include "fsmc.h"

/* Public Marcos ------------------------------------------------------------*/
#define CMD_WRITE_GRAM              0x0022
#define CMD_READ_GRAM               0x0022

#define PIXEL_WIDTH                 320
#define PIXEL_HEIGHT                240

#define DRIVER_INIT                 ILI9325_Init
#define DRIVER_READID               ILI9325_ReadID
#define DRIVER_ON                   ILI9325_DisplayOn
#define DRIVER_OFF                  ILI9325_DisplayOff
#define SET_CURSOR                  ILI9325_SetCursor
#define SET_WINDOW                  ILI9325_SetWindow
#define PREPARE_WRITE               ILI9325_PrepareWrite
#define WRITE_GRAM                  ILI9325_WriteData
#define WRITE_PIXEL                 ILI9325_WritePixel
#define READ_PIXEL                  ILI9325_ReadPixel

/* Low-Level I/O Functions ---------------------------------------------------*/

/**
  * @brief  Selects a LCD register
  * @param  reg: Register address
  * @retval None
  */
static inline void ILI9325_SelectReg(uint16_t reg)
{
    *((__IO uint16_t*)FSMC_LCD_REG_ADDR) = reg;
}

/**
  * @brief  Writes data to selected LCD register
  * @param  data: Data to write
  * @retval None
  */
static inline void ILI9325_WriteData(uint16_t data)
{
    *((__IO uint16_t*)FSMC_LCD_DATA_ADDR) = data;
}

/**
  * @brief  Selects a register and write data
  * @param  reg: Register address
  * @param  data: Data to write
  * @retval None
  */
static inline void ILI9325_WriteReg(uint16_t reg, uint16_t data)
{
    ILI9325_SelectReg(reg);
    ILI9325_WriteData(data);
}

/**
  * @brief  Reads data from selected LCD register
  * @param  None
  * @retval Data from register
  */
static inline uint16_t ILI9325_ReadData(void)
{
    return *((__IO uint16_t*)FSMC_LCD_DATA_ADDR);
}

/* LCD Interface Functions ---------------------------------------------------*/

/**
  * @brief  Initializes ILI9325 low-level bus layer
  * @param  orientation: Screen display orientation
  * @retval None
  */
void ILI9325_Init(uint8_t orientation);

/**
* @brief  Reads the driver ID
* @param  None
* @retval driver ID
*/
static inline uint16_t ILI9325_ReadID(void)
{
    uint16_t id;
    ILI9325_SelectReg(0x0000);
    /* Dummy read */
    ILI9325_ReadData();
    ILI9325_ReadData();
    /* Get driver ID */
    id = (ILI9325_ReadData() << 8);
    id |= ILI9325_ReadData();

    return id;
}

/**
  * @brief  Enables LCD display
  * @param  None
  * @retval None
  */
static inline void ILI9325_DisplayOn(void)
{
    /* Power On sequence -------------------------------------------------------*/
    ILI9325_WriteReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    ILI9325_WriteReg(0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    ILI9325_WriteReg(0x12, 0x0000); /* VREG1OUT voltage */
    ILI9325_WriteReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */

    ILI9325_WriteReg(0x10, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    ILI9325_WriteReg(0x11, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */

    ILI9325_WriteReg(0x12, 0x0139); /* VREG1OUT voltage */

    ILI9325_WriteReg(0x13, 0x1D00); /* VDV[4:0] for VCOM amplitude */
    ILI9325_WriteReg(0x29, 0x0013); /* VCM[4:0] for VCOMH */

    /* Display On */
    ILI9325_WriteReg(0x07, 0x0173); /* 262K color and display ON */
}

/**
  * @brief  Disables LCD display
  * @param  None
  * @retval None
  */
static inline void ILI9325_DisplayOff(void)
{
    /* Power Off sequence ------------------------------------------------------*/
    ILI9325_WriteReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    ILI9325_WriteReg(0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    ILI9325_WriteReg(0x12, 0x0000); /* VREG1OUT voltage */
    ILI9325_WriteReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */

    ILI9325_WriteReg(0x29, 0x0000); /* VCM[4:0] for VCOMH */

    /* Display Off */
    ILI9325_WriteReg(0x0, 0x0000);
}

/**
  * @brief  Sets cursor position
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval None
  */
void ILI9325_SetCursor(uint16_t x, uint16_t y);

/**
  * @brief  Sets a display window
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  width:  Display window width
  * @param  height: Display window height
  * @retval None
  */
void ILI9325_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
  * @brief  Prepare write GRAM
  * @param  None
  * @retval None
  */
static inline void ILI9325_PrepareWrite(void)
{
    ILI9325_SelectReg(CMD_WRITE_GRAM);
}

/**
  * @brief  Writes a pixel to GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
static inline void ILI9325_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
    /* Set cursor pos */
    ILI9325_SetCursor(x, y);
    /* Write pixel color */
    ILI9325_PrepareWrite();
    ILI9325_WriteData(color);
}

/**
  * @brief  Reads a pixel from GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval Pixel color (RGB565 format)
  */
static inline uint16_t ILI9325_ReadPixel(uint16_t x, uint16_t y)
{
    /* Set cursor pos */
    ILI9325_SetCursor(x, y);
    /* Read pixel color */
    ILI9325_SelectReg(CMD_READ_GRAM);
    return ILI9325_ReadData();
}