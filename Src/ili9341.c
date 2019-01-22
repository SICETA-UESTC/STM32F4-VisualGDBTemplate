/**
  ******************************************************************************
  * @file       ili9341.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.19
  * @brief      ILI9341 TFT LCD low-level driver based on FSMC parallel port
  *
  * @note       You need to initialize FSMC in advance to use the driver
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"
#include "lcd.h"

/* Public Function Definitions -----------------------------------------------*/

/**
  * @brief  Initializes ILI9341 low-level bus layer
  * @param  orientation: Screen display orientation
  * @retval None
  */
void ILI9341_Init(uint8_t orientation)
{
    /* Reset */
    ILI9341_SelectReg(0x00);
    ILI9341_WriteData(0x01);

    /*  Power control B (CFh)  */
    ILI9341_SelectReg(0xCF);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x81);
    ILI9341_WriteData(0x30);

    /*  Power on sequence control (EDh) */
    ILI9341_SelectReg(0xED);
    ILI9341_WriteData(0x64);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x12);
    ILI9341_WriteData(0x81);

    /*  Driver timing control A (E8h) */
    ILI9341_SelectReg(0xE8);
    ILI9341_WriteData(0x85);
    ILI9341_WriteData(0x10);
    ILI9341_WriteData(0x78);

    /*  Power control A (CBh) */
    ILI9341_SelectReg(0xCB);
    ILI9341_WriteData(0x39);
    ILI9341_WriteData(0x2C);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x34);
    ILI9341_WriteData(0x02);

    /* Pump ratio control (F7h) */
    ILI9341_SelectReg(0xF7);
    ILI9341_WriteData(0x20);

    /* Driver timing control B */
    ILI9341_SelectReg(0xEA);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x00);

    /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
    ILI9341_SelectReg(0xB1);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x1B);

    /*  Display Function Control (B6h) */
    ILI9341_SelectReg(0xB6);
    ILI9341_WriteData(0x0A);
    ILI9341_WriteData(0xA2);

    /* Power Control 1 (C0h) */
    ILI9341_SelectReg(0xC0);
    ILI9341_WriteData(0x35);

    /* Power Control 2 (C1h) */
    ILI9341_SelectReg(0xC1);
    ILI9341_WriteData(0x11);

    /* VCOM Control 1 (C5h) */
    ILI9341_SelectReg(0xC5);
    ILI9341_WriteData(0x45);
    ILI9341_WriteData(0x45);

    /*  VCOM Control 2 (C7h)  */
    ILI9341_SelectReg(0xC7);
    ILI9341_WriteData(0xA2);

    /* Enable 3G (F2h) */
    ILI9341_SelectReg(0xF2);
    ILI9341_WriteData(0x00);

    /* Gamma Set (26h) */
    ILI9341_SelectReg(0x26);
    ILI9341_WriteData(0x01);

    /* Positive Gamma Correction */
    ILI9341_SelectReg(0xE0);
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x26);
    ILI9341_WriteData(0x24);
    ILI9341_WriteData(0x0B);
    ILI9341_WriteData(0x0E);
    ILI9341_WriteData(0x09);
    ILI9341_WriteData(0x54);
    ILI9341_WriteData(0xA8);
    ILI9341_WriteData(0x46);
    ILI9341_WriteData(0x0C);
    ILI9341_WriteData(0x17);
    ILI9341_WriteData(0x09);
    ILI9341_WriteData(0x0F);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x00);

    /* Negative Gamma Correction (E1h) */
    ILI9341_SelectReg(0XE1);
    ILI9341_WriteData(0x00);
    ILI9341_WriteData(0x19);
    ILI9341_WriteData(0x1B);
    ILI9341_WriteData(0x04);
    ILI9341_WriteData(0x10);
    ILI9341_WriteData(0x07);
    ILI9341_WriteData(0x2A);
    ILI9341_WriteData(0x47);
    ILI9341_WriteData(0x39);
    ILI9341_WriteData(0x03);
    ILI9341_WriteData(0x06);
    ILI9341_WriteData(0x06);
    ILI9341_WriteData(0x30);
    ILI9341_WriteData(0x38);
    ILI9341_WriteData(0x0F);

    /* Memory access control set */
    ILI9341_SelectReg(0x36);

    switch (orientation)
    {
        case LCD_ORIENTATION_0_DEGREE:
            ILI9341_WriteData(0x08);
            break;
        case LCD_ORIENTATION_90_DEGREE:
            ILI9341_WriteData(0x68);
            break;
        case LCD_ORIENTATION_180_DEGREE:
            ILI9341_WriteData(0xC8);
            break;
        case LCD_ORIENTATION_270_DEGREE:
            ILI9341_WriteData(0xA8);
            break;
        default:
            ILI9341_WriteData(0x00);
            break;
    }

    /*  Pixel Format Set (3Ah)  */
    ILI9341_SelectReg(0x3A);
    ILI9341_WriteData(0x55); //RGB16

    /* Sleep Out (11h)  */
    ILI9341_SelectReg(0x11);
    HAL_Delay(200);

    /* Display ON (29h) */
    ILI9341_SelectReg(0x29);
}