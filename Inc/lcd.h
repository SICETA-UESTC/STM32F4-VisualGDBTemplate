/**
  ******************************************************************************
  * @file       lcd.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Universal TFT LCD driver and basic drawing functions
  *
  * @note       You need to initialize FSMC in advance to use this module.
  *             If you're using framebuffer, remember to call LCD_FrameUpdate()
  *             to update screen display at opportune time.
  *             If you're using GBK font library, you need to enable FATFS in 
  *             advance, and specified font lib files are required.
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include "fsmc.h"

/* Public Marcos -------------------------------------------------------------*/

/* Driver IC types */
#define NT35510                     0x8000
#define ILI9341                     0x9341
#define ILI9325                     0x9325

/* Backlight GPIO */
#define LCD_BL_GPIO_PORT            GPIOA
#define LCD_BL_GPIO_PIN             GPIO_PIN_6

/* LCD Configurations */
#define LCD_DRIVER_IC               ILI9341
#define LCD_USE_FRAMEBUFFER         0
#define LCD_USE_FATFS               1
#define LCD_USE_FONTLIB             1

#if !LCD_USE_FATFS
#define LCD_USE_FONTLIB             0
#endif

/* Public Types --------------------------------------------------------------*/

typedef enum {
    FONT_TYPE_SANS, FONT_TYPE_SERIF,
} FontType;

typedef enum {
    LCD_ORIENTATION_0_DEGREE,
    LCD_ORIENTATION_90_DEGREE,
    LCD_ORIENTATION_180_DEGREE,
    LCD_ORIENTATION_270_DEGREE,
} LCDOrientations;

typedef struct {
    uint16_t Width;
    uint16_t Height;
    uint8_t Orientation;
    uint16_t ID;
    uint8_t FontType;
} LCD_InfoTypeDef;

#pragma pack(2)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserverd1;
    uint16_t bfReserverd2;
    uint32_t bfOffBits;
} BmpFileHeader;

typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BmpInfoHeader;

/* Public Function Prototypes ------------------------------------------------*/
void LCD_Init(uint8_t orientation);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t color);
void LCD_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color);
void LCD_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color);
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t radius, uint16_t color);
void LCD_FillCircle(uint16_t x0, uint16_t y0, uint8_t radius, uint16_t color);
void LCD_DrawBitmapStream(const uint16_t *stream_buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void LCD_DrawNumber(int32_t num, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawBigNumber(uint8_t num, uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawString(const uint8_t *str, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawCharASCII(uint8_t ch, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color);

#if LCD_USE_FRAMEBUFFER
void LCD_FrameUpdate(void);
#endif // LCD_USE_FRAMEBUFFER

#if LCD_USE_FATFS
void LCD_DrawBitmapStreamFromFile(const uint8_t* file_name, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void LCD_DrawBmpFromFile(const uint8_t* file_name, uint16_t x, uint16_t y);
#endif // LCD_USE_FATFS

#if LCD_USE_FONTLIB
void LCD_SetFont(FontType font_type);
void LCD_DrawCharGB2312(uint8_t* ch_ptr, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color);
#endif // LCD_USE_GBKFONTLIB 

/* Private Function Prototypes -----------------------------------------------*/

/* End of file ---------------------------------------------------------------*/