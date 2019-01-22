/**
  ******************************************************************************
  * @file       ili9325.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.22
  * @brief      ILI9341 TFT LCD low-level driver based on FSMC parallel port
  *
  * @note       You need to initialize FSMC in advance to use the driver
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ili9325.h"
#include "lcd.h"

/* Public Function Definitions -----------------------------------------------*/

/**
  * @brief  Initializes ILI9325 low-level bus layer
  * @param  orientation: Screen display orientation
  * @retval None
  */
void ILI9325_Init(uint8_t orientation)
{
    /* Start Initial Sequence --------------------------------------------------*/
    ILI9325_WriteReg(0x00, 0x0001); /* Start internal OSC. */
    ILI9325_WriteReg(0x01, 0x0100); /* Set SS and SM bit */
    ILI9325_WriteReg(0x02, 0x0700); /* Set 1 line inversion */
    ILI9325_WriteReg(0x03, 0x1018); /* Set GRAM write direction and BGR=1. */
    ILI9325_WriteReg(0x04, 0x0000); /* Resize register */
    ILI9325_WriteReg(0x08, 0x0202); /* Set the back porch and front porch */
    ILI9325_WriteReg(0x09, 0x0000); /* Set non-display area refresh cycle ISC[3:0] */
    ILI9325_WriteReg(0x0A, 0x0000); /* FMARK function */
    ILI9325_WriteReg(0x0C, 0x0000); /* RGB interface setting */
    ILI9325_WriteReg(0x0D, 0x0000); /* Frame marker Position */
    ILI9325_WriteReg(0x0F, 0x0000); /* RGB interface polarity */

    /* Power On sequence -------------------------------------------------------*/
    ILI9325_WriteReg(0x10, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    ILI9325_WriteReg(0x11, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    ILI9325_WriteReg(0x12, 0x0000); /* VREG1OUT voltage */
    ILI9325_WriteReg(0x13, 0x0000); /* VDV[4:0] for VCOM amplitude */

    ILI9325_WriteReg(0x10, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    ILI9325_WriteReg(0x11, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */

    ILI9325_WriteReg(0x12, 0x0139); /* VREG1OUT voltage */

    ILI9325_WriteReg(0x13, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    ILI9325_WriteReg(0x29, 0x0013); /* VCM[4:0] for VCOMH */

    ILI9325_WriteReg(0x20, 0x0000); /* GRAM horizontal Address */
    ILI9325_WriteReg(0x21, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve (ILI9325) ----------------------------------------*/
    ILI9325_WriteReg(0x30, 0x0007);
    ILI9325_WriteReg(0x31, 0x0302);
    ILI9325_WriteReg(0x32, 0x0105);
    ILI9325_WriteReg(0x35, 0x0206);
    ILI9325_WriteReg(0x36, 0x0808);
    ILI9325_WriteReg(0x37, 0x0206);
    ILI9325_WriteReg(0x38, 0x0504);
    ILI9325_WriteReg(0x39, 0x0007);
    ILI9325_WriteReg(0x3C, 0x0105);
    ILI9325_WriteReg(0x3D, 0x0808);

    /* Set GRAM area -----------------------------------------------------------*/
    ILI9325_WriteReg(0x50, 0x0000); /* Horizontal GRAM Start Address */
    ILI9325_WriteReg(0x51, 0x00EF); /* Horizontal GRAM End Address */
    ILI9325_WriteReg(0x52, 0x0000); /* Vertical GRAM Start Address */
    ILI9325_WriteReg(0x53, 0x013F); /* Vertical GRAM End Address */

    ILI9325_WriteReg(0x60, 0xA700); /* Gate Scan Line(GS=1, scan direction is G320~G1) */
    ILI9325_WriteReg(0x61, 0x0001); /* NDL,VLE, REV */
    ILI9325_WriteReg(0x6A, 0x0000); /* set scrolling line */

    /* Partial Display Control -------------------------------------------------*/
    ILI9325_WriteReg(0x80, 0x0000);
    ILI9325_WriteReg(0x81, 0x0000);
    ILI9325_WriteReg(0x82, 0x0000);
    ILI9325_WriteReg(0x83, 0x0000);
    ILI9325_WriteReg(0x84, 0x0000);
    ILI9325_WriteReg(0x85, 0x0000);

    /* Panel Control -----------------------------------------------------------*/
    ILI9325_WriteReg(0x90, 0x0010);
    ILI9325_WriteReg(0x92, 0x0000);
    ILI9325_WriteReg(0x93, 0x0003);
    ILI9325_WriteReg(0x95, 0x0110);
    ILI9325_WriteReg(0x97, 0x0000);
    ILI9325_WriteReg(0x98, 0x0000);

    /* set GRAM write direction and BGR = 1 */
    /* I/D=00 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    switch (orientation)
    {
        case LCD_ORIENTATION_0_DEGREE:
            ILI9325_WriteReg(0x03, 0x1018);
            break;
        case LCD_ORIENTATION_90_DEGREE:
            ILI9325_WriteReg(0x03, 0x1000);
            break;
        case LCD_ORIENTATION_180_DEGREE:
            ILI9325_WriteReg(0x03, 0x1008);
            break;
        case LCD_ORIENTATION_270_DEGREE:
            ILI9325_WriteReg(0x03, 0x1010);
            break;
        default:
            ILI9325_WriteReg(0x03, 0x1018);
            break;
    }

    /* 262K color and display ON */
    ILI9325_WriteReg(0x07, 0x0173);

    /* Prepare to write GRAM */
    ILI9325_SelectReg(0x22);
}