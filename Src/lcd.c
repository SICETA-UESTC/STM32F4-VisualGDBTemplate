/**
  ******************************************************************************
  * @file       lcd.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Universal TFT LCD driver and basic drawing functions
  *
  * @note       You need to initialize FSMC in advance to use this module.
  *             If you're using framebuffer, remember to call LCD_FrameUpdate()
  *             to update screen display at opportune time.
  *             If you're using GB2312 font library, you need to enable FATFS in 
  *             advance, also specified font lib files are required.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "fsmc.h"

#include "colors.h"
#include "ascii.h" 
#include "big_number.h"

#include <stdlib.h>
#include <string.h>

#if LCD_DRIVER_IC == NT35510
#include "nt35510.h"
#elif LCD_DRIVER_IC == ILI9341
#include "ili9341.h"
#elif LCD_DRIVER_IC == ILI9325
#include "ili9325.h"
#endif

#if LCD_USE_FRAMEBUFFER
#include "frame_buffer.h"
#endif // LCD_USE_BACKBUFFER

#if LCD_USE_FATFS
#include "fatfs.h"
#endif // LCD_USE_FATFS

/* Private Marcos ------------------------------------------------------------*/

#if LCD_USE_FRAMEBUFFER

/* You can use other memory block as framebuffer as long as
 * it's big enough to store pixels of the whole screen area (width * height * 2 bytes)
 */
#define FRAMEBUFFER_BASE_ADDR       FSMC_SRAM_BASE_ADDR
#define WRITE_PIXEL(X, Y, COL)      FrameBuffer_WritePixel(&s_framebuffer, X, Y, COL)
#define READ_PIXEL(X, Y)            FrameBuffer_ReadPixel(&s_framebuffer, X, Y)

#endif // LCD_USE_FRAMEBUFFER

/* Public variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
static LCD_InfoTypeDef s_lcd_info;

#if LCD_USE_FRAMEBUFFER
static FrameBufferTypeDef s_framebuffer;
#endif // LCD_USE_FRAMEBUFFER

#if LCD_USE_FATFS
static uint8_t s_file_buffer[2048];
#endif // LCD_USE_FATFS

#if LCD_USE_FONTLIB
static FIL s_ascii_font_file, s_gb2312_font_file;
#endif // LCD_USE_FONTLIB

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_m2m;

/* Public Function Definitions -----------------------------------------------*/

/**
  * @brief  Initializes LCD display
  * @param  orientation: Screen display orientation
  * @retval None
  */
void LCD_Init(uint8_t orientation)
{
    /* Low-level driver initialization */
    DRIVER_INIT(orientation);

    s_lcd_info.Orientation = orientation;
    s_lcd_info.ID = DRIVER_READID();

    switch (orientation)
    {
        /* Vertical screen */
        case LCD_ORIENTATION_0_DEGREE:
        case LCD_ORIENTATION_180_DEGREE:
            s_lcd_info.Width = PIXEL_HEIGHT;
            s_lcd_info.Height = PIXEL_WIDTH;
            break;

        /* Horizontal screen */
        case LCD_ORIENTATION_90_DEGREE:
        case LCD_ORIENTATION_270_DEGREE:
            s_lcd_info.Width = PIXEL_WIDTH;
            s_lcd_info.Height = PIXEL_HEIGHT;
            break;
    }

#if LCD_USE_FRAMEBUFFER
    /* Initialize framebuffer */
    FrameBuffer_Init(&s_framebuffer, FRAMEBUFFER_BASE_ADDR, FSMC_LCD_DATA_ADDR, 0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif // LCD_USE_FRAMEBUFFER

    /* Initialize backlight GPIO */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = LCD_BL_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_BL_GPIO_PORT, &GPIO_InitStruct);

    /* Display on */
    LCD_Clear(BLACK);
    LCD_DisplayOn();
}

#if LCD_USE_FONTLIB 
/**
  * @brief  Initializes GB2312 font library
  * @param  type: font type select
  * @retval None
  */
void LCD_SetFont(FontType font_type)
{
    s_lcd_info.FontType = font_type;
}
#endif // LCD_USE_FONTLIB 

/**
  * @brief  Enables LCD display
  * @param  None
  * @retval None
  */
void LCD_DisplayOn(void)
{
    /* Driver on */
    DRIVER_ON();
    /* Backlight on */
    LCD_BL_GPIO_PORT->BSRR = LCD_BL_GPIO_PIN;
}

/**
  * @brief  Disables LCD display
  * @param  None
  * @retval None
  */
void LCD_DisplayOff(void)
{
    /* Driver off */
    DRIVER_OFF();
    /* Backlight off */
    LCD_BL_GPIO_PORT->BSRR = (uint32_t)LCD_BL_GPIO_PIN << 16U;
}

#if LCD_USE_FRAMEBUFFER
/**
  * @brief  Update pixels in framebuffer to whole screen
  * @param  None
  * @retval None
  */
void LCD_FrameUpdate(void)
{
    FrameBuffer_Update(&s_framebuffer);
}
#endif // LCD_USE_FRAMEBUFFER

/**
  * @brief  Fill whole screen with single color
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_Clear(uint16_t color)
{
#if LCD_USE_FRAMEBUFFER
    FrameBuffer_Clear(&s_framebuffer, color);
#else
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);

    PREPARE_WRITE();
    for (size_t i = 0; i < s_lcd_info.Width * s_lcd_info.Height; i++) {
        WRITE_GRAM(color);
    }
#endif // LCD_USE_FRAMEBUFFER
}

/**
  * @brief  Draw a horizontal line on screen
  * @param  x: Start point X top-left position
  * @param  y: Start point Y top-left position
  * @param  width: Width of line
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawHLine(uint16_t x, uint16_t y, uint16_t width, uint16_t color)
{
#if LCD_USE_FRAMEBUFFER
    for (uint16_t i = 0; i < width; i++) {
        WRITE_PIXEL(x + i, y, color);
    }
#else
    SET_WINDOW(x, y, width, 1);
    PREPARE_WRITE();

    for (uint16_t i = 0; i < width; i++) {
        WRITE_GRAM(color);
    }

    /* Get your ass back here! */
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif
}

/**
  * @brief  Draw a vertical line on screen
  * @param  x: Start point X top-left position
  * @param  y: Start point Y top-left position
  * @param  height: Height of line
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color)
{
#if LCD_USE_FRAMEBUFFER
    for (uint16_t i = 0; i < height; i++) {
        WRITE_PIXEL(x, y + i, color);
    }
#else
    SET_WINDOW(x, y, 1, height);
    PREPARE_WRITE();

    for (uint16_t i = 0; i < height; i++) {
        WRITE_GRAM(color);
    }

    /* Get your ass back here! */
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif
}

/**
  * @brief  Draw a straight line on screen
  * @param  x0: Start point X top-left position
  * @param  y0: Start point Y top-left position
  * @param  x1: End point X top-left position
  * @param  y1: End point Y top-left position
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t temp;
    _Bool is_steep = (abs(y1 - y0) > abs(x1 - x0));

    if (is_steep) {
        temp = x0; x0 = y0; y0 = temp;
        temp = x1; x1 = y1; y1 = temp;
    }
    if (x0 > x1) {
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t y_step = (y0 < y1) ? 1 : -1;

    for (int16_t x = x0, y = y0; x <= x1; x++) {
        if (is_steep) {
            WRITE_PIXEL(y, x, color);
        }
        else {
            WRITE_PIXEL(x, y, color);
        }

        err -= dy;
        if (err < 0) {
            y += y_step;
            err += dx;
        }
    }
}

/**
  * @brief  Draw a rectangle on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  width:  Rectangle width
  * @param  height: Rectangle height
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    LCD_DrawHLine(x, y, width, color);
    LCD_DrawHLine(x, y + height - 1, width, color);
    LCD_DrawVLine(x, y, height, color);
    LCD_DrawVLine(x + width - 1, y, height, color);
}

/**
  * @brief  Fill a rectangular area on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  width:  Rectangle width
  * @param  height: Rectangle height
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
#if LCD_USE_FRAMEBUFFER
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            WRITE_PIXEL(x + j, y + i, color);
        }
    }
#else
    SET_WINDOW(x, y, width, height);
    PREPARE_WRITE();

    for (uint32_t i = 0; i < width * height; i++) {
        WRITE_GRAM(color);
    }

    /* Get your ass back here! */
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif // LCD_USE_FRAMEBUFFER
}

/**
  * @brief  Draw a circle on screen
  * @param  x0: Center X position
  * @param  y0: Center Y position
  * @param  radius: Circle radius
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawCircle(uint16_t x0, uint16_t y0, uint8_t radius, uint16_t color)
{
    int16_t x = 0;
    int16_t y = radius;
    int16_t di = 3 - radius / 2;
    while (x <= y)
    {
        WRITE_PIXEL(x0 + x, y0 - y, color);
        WRITE_PIXEL(x0 + y, y0 - x, color);       
        WRITE_PIXEL(x0 + y, y0 + x, color);              
        WRITE_PIXEL(x0 + x, y0 + y, color);
        WRITE_PIXEL(x0 - x, y0 + y, color);      
        WRITE_PIXEL(x0 - y, y0 + x, color);
        WRITE_PIXEL(x0 - x, y0 - y, color);            
        WRITE_PIXEL(x0 - y, y0 - x, color);   	         
        x++;
   
        if (di < 0) {
            di += 4 * x + 6;
        }
        else {
            di += 10 + 4 * (x - y);
            y--;
        }
    }
}

/**
  * @brief  Fill a circular area on screen
  * @param  x0: Center X position
  * @param  y0: Center Y position
  * @param  radius: Circle radius
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_FillCircle(uint16_t x0, uint16_t y0, uint8_t radius, uint16_t color)
{
    int16_t x = 0;
    int16_t y = radius;
    int16_t di = 3 - radius / 2;
    while (x <= y)
    {
        if (y > 0)
        {
            LCD_DrawHLine(x0 - y, y0 + x, 2 * y, color);
            LCD_DrawHLine(x0 - y, y0 - x, 2 * y, color);
        }

        if (x > 0)
        {
            LCD_DrawHLine(x0 - x, y0 - y, 2 * x, color);
            LCD_DrawHLine(x0 - x, y0 + y, 2 * x, color);
        }
        x++;

        if (di < 0) {
            di += 4 * x + 6;
        }
        else {
            di += 10 + 4 * (x - y);
            y--;
        }
    }
}

/**
  * @brief  Draw a RGB565 bitmap stream on screen from memory 
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  width:  Bitmap width
  * @param  height: Bitmap height
  * @param  img_buffer: Pointer to bitmap pixel buffer
  * @retval None
  */
void LCD_DrawBitmapStream(const uint16_t *stream_buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
#if LCD_USE_FRAMEBUFFER
    for (size_t i = 0; i < height; i++) {
        memcpy(s_framebuffer.PixelData + s_framebuffer.Width * y + x, stream_buffer, width * 2);
    }
#else
    SET_WINDOW(x, y, width, height);
    PREPARE_WRITE();

    HAL_DMA_Start(&hdma_m2m, stream_buffer, FSMC_LCD_DATA_ADDR, width * height / 2);
    HAL_DMA_PollForTransfer(&hdma_m2m, HAL_DMA_FULL_TRANSFER, 1000);

    /* Get your ass back here! */
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif  // LCD_USE_FRAMEBUFFER
}

#if LCD_USE_FATFS
/**
  * @brief  Draw a RGB565 bitmap stream on screen from binary file
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  width:  Bitmap width
  * @param  height: Bitmap height
  * @param  file_name: Bitmap filename
  * @retval None
  */
void LCD_DrawBitmapStreamFromFile(const uint8_t* file_name, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t *line_buffer = (uint16_t *)s_file_buffer;
    FIL stream_file;
    size_t read_count;

#if LCD_USE_FRAMEBUFFER
    uint16_t *fb_write_addr = s_framebuffer.PixelData + s_lcd_info.Width * y + x;
#else
    SET_WINDOW(x, y, width, height);
    PREPARE_WRITE();
#endif

    if (f_open(&stream_file, file_name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
    {
        for (size_t i = 0; i < height; i++)
        {
            if (f_read(&stream_file, line_buffer, width * 2, &read_count) != FR_OK) {
                break;
            }
#if LCD_USE_FRAMEBUFFER
            memcpy(fb_write_addr, line_buffer, width * 2);
            fb_write_addr += s_lcd_info.Width;
#else
            for (size_t j = 0; j < width; j++) {
                WRITE_GRAM(line_buffer[j]);
            }
#endif // LCD_USE_FRAMEBUFFER
        }
    }

#if !LCD_USE_FRAMEBUFFER
    /* Get your ass back here! */
    SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif
    f_close(&stream_file);
}

/**
  * @brief  Draw a BMP image on screen from binary file
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  file_name: Bitmap filename
  * @retval None
  */
void LCD_DrawBmpFromFile(const uint8_t* file_name, uint16_t x, uint16_t y)
{
    uint8_t *line_buffer = (uint8_t *)s_file_buffer;
    FIL bmp_file;
    size_t read_count;

    if (f_open(&bmp_file, file_name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
    {
        BmpFileHeader file_header;
        BmpInfoHeader info_header;

        f_read(&bmp_file, &file_header, sizeof(BmpFileHeader), &read_count);

        /* Not BMP file */
        if (file_header.bfType != 0x4D42) {
            return;
        }

        f_read(&bmp_file, &info_header, sizeof(BmpInfoHeader), &read_count);

        /* Start to write pixel data */
#if LCD_USE_FRAMEBUFFER
        uint16_t *fb_write_addr = s_framebuffer.PixelData + s_lcd_info.Width * y + x;
        uint16_t *line_buffer_rgb565 = (uint16_t *)line_buffer;
#else
        SET_WINDOW(x, y, info_header.biWidth, info_header.biHeight);
        PREPARE_WRITE();
#endif
        size_t stride = (info_header.biWidth * info_header.biBitCount + 31) / 32 * 4;

        for (size_t i = 0; i < info_header.biHeight; i++) {
            /* Read from end of bmp file */
            f_lseek(&bmp_file, file_header.bfOffBits + (info_header.biHeight - i - 1) * stride - 2);
            /* Get a line of RGB888 data */
            f_read(&bmp_file, line_buffer, stride + 2, &read_count);

            /* RGB565 */
            if (info_header.biBitCount == 16)
            {
                uint16_t color;
                uint16_t *color_ptr = (uint16_t *)(line_buffer + 2);

                for (size_t j = 0; j < info_header.biWidth; j++)
                {
                    color = *(color_ptr++);
#if LCD_USE_FRAMEBUFFER
                    line_buffer_rgb565[j] = color;
#else
                    WRITE_GRAM(color);
#endif // LCD_USE_FRAMEBUFFER
                }
            }
            /* RGB888 */
            else if (info_header.biBitCount == 24)
            {
                uint8_t r, g, b;
                uint8_t *color_ptr = line_buffer + 2;

                for (size_t j = 0; j < info_header.biWidth; j++)
                {
                    b = *(color_ptr++);
                    g = *(color_ptr++);
                    r = *(color_ptr++);

#if LCD_USE_FRAMEBUFFER
                    line_buffer_rgb565[j] = pack_rgb565(r, g, b);
#else
                    WRITE_GRAM(pack_rgb565(r, g, b));
#endif // LCD_USE_FRAMEBUFFER
                }
            }
#if LCD_USE_FRAMEBUFFER
            memcpy(fb_write_addr, line_buffer_rgb565, info_header.biWidth * 2);
            fb_write_addr += s_lcd_info.Width;
#endif
        }
#if !LCD_USE_FRAMEBUFFER
        /* Get your ass back here! */
        SET_WINDOW(0, 0, s_lcd_info.Width, s_lcd_info.Height);
#endif
    }
    f_close(&bmp_file);
}
#endif

/**
  * @brief  Draw a signed decimal number on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  font_size: Size of characters
  * @param  num: The signed number to be drawn
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawNumber(int32_t num, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t temp_str[12] = { 0 };
    uint8_t i = 10;
    _Bool is_negative = 0;

    if (num < 0) {
        num = -num;
        is_negative = 1;
    }

    do {
        temp_str[i--] = num % 10 + '0';
        num /= 10;
    } while (num);

    if (is_negative) {
        temp_str[i--] = '-';
    }

    LCD_DrawString(temp_str + i + 1, font_size, x, y, color);
}

/**
  * @brief  Draw a signed decimal number on screen (size fixed at 64x48)
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  num: The signed number to be drawn
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawBigNumber(uint8_t num, uint16_t x, uint16_t y, uint16_t color)
{
    if (num == '.') {
        num = 10;
    }
    uint16_t y0 = y;

    for (uint16_t i = 0; i < 384; i++)
    {
        uint8_t temp = big_number_64x48[num][i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (temp & 1) WRITE_PIXEL(x, y, color);

            temp >>= 1;
            ++y;
            if ((y - y0) == 64)
            {
                y = y0;
                ++x;
                break;
            }
        }
    }
}

/**
  * @brief  Draw a string on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  font_size: Size of characters
  * @param  str: Poniter to string buffer
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawString(const uint8_t *str, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color)
{
#if LCD_USE_FONTLIB

    _Bool is_ascii_font_ready;
    _Bool is_gb2312_font_ready;

    if (s_lcd_info.FontType == FONT_TYPE_SANS)
    {
        switch (font_size)
        {
            case 16:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/sans_ascii_8x16.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/sans_gb2312_16x16.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 24:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/sans_ascii_12x24.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/sans_gb2312_24x24.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 32:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/sans_ascii_16x32.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/sans_gb2312_32x32.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 40:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/sans_ascii_20x40.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/sans_gb2312_40x40.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            default:
                return;
        }
    }
    else if (s_lcd_info.FontType == FONT_TYPE_SERIF)
    {
        switch (font_size)
        {
            case 16:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/serif_ascii_8x16.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/serif_gb2312_16x16.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 24:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/serif_ascii_12x24.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/serif_gb2312_24x24.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 32:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/serif_ascii_16x32.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/serif_gb2312_32x32.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            case 40:
                is_ascii_font_ready = (f_open(&s_ascii_font_file, "0:/fontlib/serif_ascii_20x40.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                is_gb2312_font_ready = (f_open(&s_gb2312_font_file, "0:/fontlib/serif_gb2312_40x40.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK);
                break;

            default:
                return;
        }
    }

    while (*str && x < s_lcd_info.Width)
    {
        if (*str < 0x80)
        {
            if (is_ascii_font_ready) {
                LCD_DrawCharASCII(*str, font_size, x, y, color);
            }
            str++;
            x += font_size / 2;
        }
        else
        {
            if (is_gb2312_font_ready) {
                LCD_DrawCharGB2312(str, font_size, x, y, color);
            }
            str += 2;
            x += font_size;
        }
    }

    f_close(&s_ascii_font_file);
    f_close(&s_gb2312_font_file);
#else
    while (*str)
    {
        if (*str > 0x7F) {
            continue;
        }
        LCD_DrawCharASCII(*str, font_size, x, y, color);
        str++;
        x += font_size / 2;
    }
#endif // LCD_USE_FONTLIB
}

/**
  * @brief  Draw a ASCII character on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  font_size: Size of characters
  * @param  ch: character to be drawn
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawCharASCII(uint8_t ch, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t buffer_size = font_size * font_size >> 4;
    uint16_t y0 = y;
    uint8_t* font_buffer;

    ch -= ' ';

#if LCD_USE_FONTLIB

    size_t read_count;
    font_buffer = (uint8_t *)s_file_buffer;

    /* Calculate offset in font lib file */
    f_lseek(&s_ascii_font_file, ch * buffer_size);
    f_read(&s_ascii_font_file, font_buffer, buffer_size, &read_count);

#else
    switch (font_size)
    {
        case 16: font_buffer = ASCII_8x16[ch]; break;
        case 24: font_buffer = ASCII_12x24[ch]; break;
        case 32: font_buffer = ASCII_16x32[ch]; break;
        case 40: font_buffer = ASCII_20x40[ch]; break;
        default: return;
    }
#endif

    for (uint8_t i = 0; i < buffer_size; i++)
    {
        uint8_t temp = font_buffer[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (temp & 1) WRITE_PIXEL(x, y, color);

            temp >>= 1;
            ++y;
            if ((y - y0) == font_size)
            {
                y = y0;
                ++x;
                break;
            }
        }
    }
}

#if LCD_USE_FONTLIB 
/**
  * @brief  Draw a GB2312 character on screen
  * @param  x: Top-left corner X position
  * @param  y: Top-left corner Y position
  * @param  font_size: Size of characters
  * @param  ch_ptr: Pointer to the GB2312 character
  * @param  color: Pixel color (RGB565 format)
  * @retval None
  */
void LCD_DrawCharGB2312(uint8_t* ch_ptr, uint8_t font_size, uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t buffer_size = font_size * font_size >> 3;
    uint16_t y0 = y;

    uint8_t *font_buffer = (uint8_t *)s_file_buffer;
    size_t read_count;

    /* Calculate offset in font lib file */
    f_lseek(&s_gb2312_font_file, ((ch_ptr[0] - 0xA1) * 94 + (ch_ptr[1] - 0xA1)) * buffer_size);
    f_read(&s_gb2312_font_file, font_buffer, buffer_size, &read_count);

    for (uint8_t i = 0; i < buffer_size; i++)
    {
        uint8_t temp = font_buffer[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (temp & 1) WRITE_PIXEL(x, y, color);

            temp >>= 1;
            ++y;
            if ((y - y0) == font_size)
            {
                y = y0;
                ++x;
                break;
            }
        }
    }
}
#endif

/* End of file ---------------------------------------------------------------*/