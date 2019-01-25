/**
  ******************************************************************************
  * @file       nt35510.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      NT35510 TFT LCD low-level driver based on FSMC parallel port
  *
  * @note       You need to initialize FSMC in advance to use the driver
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include "fsmc.h"

/* Public Marcos -------------------------------------------------------------*/
#define CMD_WRITE_GRAM  0x2C00
#define CMD_READ_GRAM   0x2E00

#define PIXEL_WIDTH     800
#define PIXEL_HEIGHT    480

/* Low-Level I/O Functions ---------------------------------------------------*/

/**
  * @brief  Selects a LCD register
  * @param  reg: Register address
  * @retval None
  */
static inline void NT35510_SelectReg(uint16_t reg)
{
    *((__IO uint16_t*)FSMC_LCD_REG_ADDR) = reg;
}

/**
  * @brief  Writes data to selected LCD register
  * @param  data: Data to write
  * @retval None
  */
static inline void NT35510_WriteData(uint16_t data)
{
    *((__IO uint16_t*)FSMC_LCD_DATA_ADDR) = data;
}

/**
  * @brief  Selects a register and write data
  * @param  reg: Register address
  * @param  data: Data to write
  * @retval None
  */
static inline void NT35510_WriteReg(uint16_t reg, uint16_t data)
{
    NT35510_SelectReg(reg);
    NT35510_WriteData(data);
}

/**
  * @brief  Reads data from selected LCD register
  * @param  None
  * @retval Data from register
  */
static inline uint16_t NT35510_ReadData(void)
{
    return *((__IO uint16_t*)FSMC_LCD_DATA_ADDR);
}

/* LCD Interface Functions ---------------------------------------------------*/

/**
  * @brief  Initializes NT35510 low-level bus layer
  * @param  orientation: Screen display orientation
  * @retval None
  */
void NT35510_Init(uint8_t orientation);

/**
* @brief  Reads the driver ID
* @param  None
* @retval driver ID
*/
static inline uint16_t NT35510_ReadID(void)
{
    uint16_t id;
    /* Get driver ID */
    NT35510_SelectReg(0xDA00);
    id = NT35510_ReadData();
    NT35510_SelectReg(0xDB00);
    id |= (NT35510_ReadData() << 8);
    return id;
}

/**
  * @brief  Enables LCD display
  * @param  None
  * @retval None
  */
static inline void NT35510_DisplayOn(void)
{
    NT35510_SelectReg(0x2900);
}

/**
  * @brief  Disables LCD display
  * @param  None
  * @retval None
  */
static inline void NT35510_DisplayOff(void)
{
    NT35510_SelectReg(0x2800);
}

/**
  * @brief  Sets cursor position
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval None
  */
static inline void NT35510_SetCursor(uint16_t x, uint16_t y)
{
    /* Column address set */
    NT35510_WriteReg(0x2A00, x >> 8);
    NT35510_WriteReg(0x2A01, x & 0xFF);
    /* Page address set */
    NT35510_WriteReg(0x2B00, y >> 8);
    NT35510_WriteReg(0x2B01, y & 0xFF);
}

/**
  * @brief  Sets a Display window
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  width:  Display window width
  * @param  height: Display window height
  * @retval None
  */
static inline void NT35510_SetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    /* Column address range set */
    NT35510_WriteReg(0x2A00, x >> 8);
    NT35510_WriteReg(0x2A01, x & 0xFF);
    NT35510_WriteReg(0x2A02, (x + width - 1) >> 8);
    NT35510_WriteReg(0x2A03, (x + width - 1) & 0xFF);
    /* Page address range set */
    NT35510_WriteReg(0x2B00, y >> 8);
    NT35510_WriteReg(0x2B01, y & 0xFF);
    NT35510_WriteReg(0x2B02, (y + height - 1) >> 8);
    NT35510_WriteReg(0x2B03, (y + height - 1) & 0xFF);
}

/**
  * @brief  Prepare write GRAM
  * @param  None
  * @retval None
  */
static inline void NT35510_PrepareWrite(void)
{
    NT35510_SelectReg(CMD_WRITE_GRAM);
}

/**
  * @brief  Writes a pixel to GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
static inline void NT35510_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
    /* Set cursor pos */
    NT35510_SetCursor(x, y);
    /* Write pixel color */
    NT35510_PrepareWrite();
    NT35510_WriteData(color);
}

/**
  * @brief  Reads a pixel from GRAM
  * @param  x: Specifies the X top-left position
  * @param  y: Specifies the Y top-left position
  * @retval Pixel color (RGB565 format)
  */
static inline uint16_t NT35510_ReadPixel(uint16_t x, uint16_t y)
{
    /* Set cursor pos */
    NT35510_SetCursor(x, y);
    /* Read pixel color */
     NT35510_SelectReg(CMD_READ_GRAM);

     NT35510_ReadData();  // Dummy
    uint16_t r =  NT35510_ReadData();
    uint16_t b =  NT35510_ReadData();
    uint16_t g = r & 0xFF;

    /* return RGB565 pixel color */
    g <<= 8;
    return ((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11);
}