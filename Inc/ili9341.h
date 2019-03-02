/**
  ******************************************************************************
  * @file       ili9341.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.19
  * @brief      ILI9341 TFT LCD low-level driver based on FSMC parallel port
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
#define CMD_WRITE_GRAM              0x002C
#define CMD_READ_GRAM               0x002E

#define PIXEL_WIDTH                 320
#define PIXEL_HEIGHT                240

#define DRIVER_INIT                 ILI9341_Init
#define DRIVER_READID               ILI9341_ReadID
#define DRIVER_ON                   ILI9341_DisplayOn
#define DRIVER_OFF                  ILI9341_DisplayOff
#define SET_CURSOR                  ILI9341_SetCursor
#define SET_WINDOW                  ILI9341_SetWindow
#define PREPARE_WRITE               ILI9341_PrepareWrite
#define WRITE_GRAM                  ILI9341_WriteData
#define WRITE_PIXEL                 ILI9341_WritePixel
#define READ_PIXEL                  ILI9341_ReadPixel

/* Low-Level I/O Functions ---------------------------------------------------*/

/**
  * @brief  Selects a LCD register
  * @param  reg: Register address
  * @retval None
  */
static inline void ILI9341_SelectReg(uint16_t reg)
{
    *((__IO uint16_t*)FSMC_LCD_REG_ADDR) = reg;
}

/**
  * @brief  Writes data to selected LCD register
  * @param  data: Data to write
  * @retval None
  */
static inline void ILI9341_WriteData(uint16_t data)
{
    *((__IO uint16_t*)FSMC_LCD_DATA_ADDR) = data;
}

/**
  * @brief  Reads data from selected LCD register
  * @param  None
  * @retval Data from register
  */
static inline uint16_t ILI9341_ReadData(void)
{
    return *((__IO uint16_t*)FSMC_LCD_DATA_ADDR);
}


/* LCD Interface Functions ---------------------------------------------------*/

/**
  * @brief  Initializes ILI9341 low-level bus layer
  * @param  orientation: Screen display orientation
  * @retval None
  */
void ILI9341_Init(uint8_t orientation);

/**
* @brief  Reads the driver ID
* @param  None
* @retval driver ID
*/
static inline uint16_t ILI9341_ReadID(void)
{
    uint16_t id;
    ILI9341_SelectReg(0x00D3);
    /* Dummy read */
    ILI9341_ReadData();
    ILI9341_ReadData();
    /* Get driver ID */
    id = (ILI9341_ReadData() << 8);
    id |= ILI9341_ReadData();

    return id;
}

/**
  * @brief  Enables LCD display
  * @param  None
  * @retval None
  */
static inline void ILI9341_DisplayOn(void)
{
    ILI9341_SelectReg(0x29);
}

/**
  * @brief  Disables LCD display
  * @param  None
  * @retval None
  */
static inline void ILI9341_DisplayOff(void)
{
    ILI9341_SelectReg(0x28);
}

/**
  * @brief  Sets cursor position
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval None
  */
static inline void ILI9341_SetCursor(uint16_t x, uint16_t y)
{
    /* Column address set */
    ILI9341_SelectReg(0x2A);
    ILI9341_WriteData(x >> 8);
    ILI9341_WriteData(x & 0xFF);
    /* Page address set */
    ILI9341_SelectReg(0x2B);
    ILI9341_WriteData(y >> 8);
    ILI9341_WriteData(y & 0xFF);
}

/**
  * @brief  Sets a display window
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  width:  Display window width
  * @param  height: Display window height
  * @retval None
  */
static inline void ILI9341_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    /* Column address range set */
    ILI9341_SelectReg(0x2A);
    ILI9341_WriteData(x >> 8);
    ILI9341_WriteData(x & 0xFF);
    ILI9341_WriteData((x + width -1) >> 8);
    ILI9341_WriteData((x + width - 1) & 0xFF);
    /* Page address range set */
    ILI9341_SelectReg(0x2B);
    ILI9341_WriteData(y >> 8);
    ILI9341_WriteData(y & 0xFF);
    ILI9341_WriteData((y + height - 1) >> 8);
    ILI9341_WriteData((y + height - 1) & 0xFF);
}

/**
  * @brief  Prepare write GRAM
  * @param  None
  * @retval None
  */
static inline void ILI9341_PrepareWrite(void)
{
    ILI9341_SelectReg(CMD_WRITE_GRAM);
}

/**
  * @brief  Writes a pixel to GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
static inline void ILI9341_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
    /* Set cursor pos */
    ILI9341_SetCursor(x, y);
    /* Write pixel color */
    ILI9341_PrepareWrite();
    ILI9341_WriteData(color);
}

/**
  * @brief  Reads a pixel from GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval Pixel color (RGB565 format)
  */
static inline uint16_t ILI9341_ReadPixel(uint16_t x, uint16_t y)
{
    /* Set cursor pos */
    ILI9341_SetCursor(x, y);
    /* Read pixel color */
    ILI9341_SelectReg(CMD_READ_GRAM);

    ILI9341_ReadData();  // Dummy
    uint16_t r = ILI9341_ReadData();
    uint16_t b = ILI9341_ReadData();
    uint16_t g = r & 0xFF;

    /* return RGB565 pixel color */
    g <<= 8;
    return ((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11);
}