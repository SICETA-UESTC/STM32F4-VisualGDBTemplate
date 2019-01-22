/**
  ******************************************************************************
  * @file       curve_chart.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Dynamic curve chart UI control
  *
  * @note       You need to initialize LCD driver before use this module
  *             If you're using framebuffer, when time is opportune, call 
  *             CurveChart_FrameUpdate() to update chart area to screen.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "curve_chart.h"
#include "lcd.h"

#if CHART_USE_FRAMEBUFFER
#include "frame_buffer.h"
#include "fsmc.h"
#else

#if LCD_DRIVER_IC == NT35510
#include "nt35510.h"
#elif LCD_DRIVER_IC == ILI9341
#include "ili9341.h"
#elif LCD_DRIVER_IC == ILI9325
#include "ili9325.h"
#endif

#endif // LCD_USE_BACKBUFFER 

/* Private Marcos ------------------------------------------------------------*/
#if CHART_USE_FRAMEBUFFER

/* You can use other memory block as framebuffer as long as
 * it's big enough to store pixels in chart window (width * height * 2 bytes)
 */
#define FRAMEBUFFER_BASE_ADDR       FSMC_SRAM_BASE_ADDR

#else
#if LCD_DRIVER_IC == NT35510

#define WRITE_PIXEL                 NT35510_WritePixel
#define READ_PIXEL                  NT35510_ReadPixel

#elif LCD_DRIVER_IC == ILI9341

#define WRITE_PIXEL                 ILI9341_WritePixel
#define READ_PIXEL                  ILI9341_ReadPixel

#elif LCD_DRIVER_IC == ILI9325

#define WRITE_PIXEL                 ILI9325_WritePixel
#define READ_PIXEL                  ILI9325_ReadPixel

#endif
#endif

/* Private variables ---------------------------------------------------------*/
#if CHART_USE_FRAMEBUFFER
static FrameBufferTypeDef s_framebuffer;
#endif // LCD_USE_BACKBUFFER 

/* Public Function Definitions -----------------------------------------------*/

#if CHART_USE_FRAMEBUFFER 
void CurveChart_Init(CurveChartTypeDef *chart)
{
    /* Draw border */
    LCD_DrawRect(chart->X - 1, chart->Y - 1, chart->Width + 1, chart->Height + 1, chart->BorderColor);
    /* Init backbuffer */
    FrameBuffer_Init(&s_framebuffer, FRAMEBUFFER_BASE_ADDR, FSMC_LCD_DATA_ADDR, chart->X, chart->Y, chart->Width, chart->Height);
    /* Fill background */
    FrameBuffer_Clear(&s_framebuffer, chart->BackgroudColor);
    /* Draw fine grid - horizontal */
    for (size_t i = chart->FineGridHeight; i < chart->Height; i += chart->FineGridHeight) {
        for (size_t j = 0; j < chart->Width; j++) {
            FrameBuffer_WritePixel(&s_framebuffer, j, i, chart->FineGridColor);
        }
    }
    /* Draw fine grid - vertical */
    for (size_t i = chart->FineGridWidth; i < chart->Width; i += chart->FineGridWidth) {
        for (size_t j = 0; j < chart->Height; j++) {
            FrameBuffer_WritePixel(&s_framebuffer, i, j, chart->FineGridColor);
        }
    }
    /* Draw coarse grid - horizontal */
    for (size_t i = chart->CoarseGridHeight; i < chart->Height; i += chart->CoarseGridHeight) {
        for (size_t j = 0; j < chart->Width; j++) {
            FrameBuffer_WritePixel(&s_framebuffer, j, i, chart->CoarseGridColor);
        }
    }
    /* Draw coarse grid - vertical */
    for (size_t i = chart->CoarseGridWidth; i < chart->Width; i += chart->CoarseGridWidth) {
        for (size_t j = 0; j < chart->Height; j++) {
            FrameBuffer_WritePixel(&s_framebuffer, i, j, chart->CoarseGridColor);
        }
    }
}
#else
void CurveChart_Init(CurveChartTypeDef *chart)
{
    /* Draw border */
    LCD_DrawRect(chart->X - 1, chart->Y - 1, chart->Width + 1, chart->Height + 1, chart->BorderColor);
    /* Fill background */
    LCD_FillRect(chart->X, chart->Y, chart->Width, chart->Height, chart->BackgroudColor);
    /* Draw fine grid - horizontal */
    for (size_t i = chart->FineGridHeight; i < chart->Height; i += chart->FineGridHeight) {
        LCD_DrawHLine(chart->X, i + chart->Y, chart->Width, chart->FineGridColor);
    }
    /* Draw coarse grid - horizontal */
    for (size_t i = chart->CoarseGridHeight; i < chart->Height; i += chart->CoarseGridHeight) {
        LCD_DrawHLine(chart->X, i + chart->Y, chart->Width, chart->CoarseGridColor);
    }
    /* Draw fine grid - vertical */
    for (size_t i = chart->FineGridWidth; i < chart->Width; i += chart->FineGridWidth) {
        LCD_DrawVLine(i + chart->X, chart->Y, chart->Height, chart->FineGridColor);
    }
    /* Draw coarse grid - vertical */
    for (size_t i = chart->CoarseGridWidth; i < chart->Width; i += chart->CoarseGridWidth) {
        LCD_DrawVLine(i + chart->X, chart->Y, chart->Height, chart->CoarseGridColor);
    }
}
#endif // CHART_USE_FRAMEBUFFER 

#if CHART_USE_FRAMEBUFFER
/**
  * @brief  Update pixels in framebuffer to chart area on screen
  * @param  None
  * @retval None
  */
void CurveChart_FrameUpdate(void)
{
    FrameBuffer_Update(&s_framebuffer);
}
#endif // LCD_USE_BACKBUFFER 

void CurveChart_DrawBitmap(const CurveChartTypeDef *chart, uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *bitmap_buffer)
{
    if (y >= chart->Height) {
        return;
    }

    y = chart->Height - y;

    for (uint16_t i = 0; i < height; i++)
    {
        for (uint16_t j = 0; j < width; j++)
        {
            /* Regard black as transparent */
            if (bitmap_buffer[i * width + j] == 0x0000) {
                continue;
            }
#if CHART_USE_FRAMEBUFFER 
            FrameBuffer_WritePixel(&s_framebuffer, x + j, y + i, bitmap_buffer[i * width + j]);
#else
            WRITE_PIXEL(chart->X + x + j, chart->Y + y + i, bitmap_buffer[i * width + j]);
#endif // CHART_USE_FRAMEBUFFER 
        }
    }
}

void CurveChart_RecoverRect(const CurveChartTypeDef *chart, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{ 
    if (y >= chart->Height) {
        return;
    }

    y = chart->Height - y;

    for (uint16_t i = 0; i < height; i++)
    {
        for (uint16_t j = 0; j < width; j++)
        {
            uint16_t pixel_color = CurveChart_GetRecoverPixelColor(chart, x + j, y + i);
#if CHART_USE_FRAMEBUFFER 
            FrameBuffer_WritePixel(&s_framebuffer, x + j, y + i, pixel_color);
#else
            WRITE_PIXEL(chart->X + x + j, chart->Y + y + i, pixel_color);
#endif // CHART_USE_FRAMEBUFFER 

        }
    }
}

void CurveChart_DrawCurve(const CurveChartTypeDef *chart, const uint16_t *data, uint16_t color)
{
    uint16_t y0, y1, temp;

    for (uint16_t i = 0; i < chart->Width - 1; i++)
    {
        if (data[i] < data[i + 1]) {
            y0 = data[i];
            y1 = data[i + 1];
        }
        else {
            y1 = data[i];
            y0 = data[i + 1];
        }

        if (y0 >= chart->Height) {
            continue;
        }
        y1 = (y1 < chart->Height) ? y1 : chart->Height - 1;

        y0 = chart->Height - y0 - 1;
        y1 = chart->Height - y1 - 1;

        for (uint16_t j = y1; j <= y0; j++) {
#if CHART_USE_FRAMEBUFFER 
            FrameBuffer_WritePixel(&s_framebuffer, i, j, color);
#else
            WRITE_PIXEL(chart->X + i, chart->Y + j, color);
#endif // GRPAH_USE_BACKBUFFER
        }
    }
}

void CurveChart_DrawLineX(const CurveChartTypeDef *chart, uint16_t x, uint16_t color)
{
    if (x > chart->Width) return;

    for (uint16_t i = 0; i < chart->Height; i++)
    {
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, x, i, color);
#else
        WRITE_PIXEL(chart->X + x, chart->Y + i, color);
#endif // CHART_USE_FRAMEBUFFER 
    }
}

void CurveChart_DrawDashedLineX(const CurveChartTypeDef *chart, uint16_t x, uint16_t color)
{
    if (x > chart->Width) return;
    uint8_t pixel_count = 0;

    for (uint16_t i = 0; i < chart->Height; i++)
    {
        ++pixel_count;

        if (pixel_count > 4) {
            i += 2;
            pixel_count = 0;
        }
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, x, i, color);
#else
        WRITE_PIXEL(chart->X + x, chart->Y + i, color);
#endif // CHART_USE_FRAMEBUFFER 

    }
}


void CurveChart_DrawLineY(const CurveChartTypeDef *chart, uint16_t y, uint16_t color)
{
    if (y > chart->Height) {
        return; 
    }

    for (uint16_t i = 0; i < chart->Width; i++)
    {
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, i, chart->Height - y, color);
#else
        WRITE_PIXEL(chart->X + i, chart->Y + chart->Height - y, color);
#endif // CHART_USE_FRAMEBUFFER 
    }
}

void CurveChart_DrawDashedLineY(const CurveChartTypeDef *chart, uint16_t y, uint16_t color)
{
    if (y > chart->Height) {
        return;
    }

    uint8_t pixel_count = 0;

    for (uint16_t i = 0; i < chart->Width; i++)
    {
        ++pixel_count;

        if (pixel_count > 4) {
            i += 2;
            pixel_count = 0;
        }
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, i, chart->Height - y, color);
#else
        WRITE_PIXEL(chart->X + i, chart->Y + chart->Height - y, color);
#endif // CHART_USE_FRAMEBUFFER 
    }
}

void CurveChart_RecoverGrid(const CurveChartTypeDef *chart, const uint16_t *data)
{
    uint16_t y0, y1, temp;
    uint16_t pixel_color;

    for (uint16_t i = 0; i < chart->Width - 1; i++)
    {
        if (data[i] < data[i + 1]) {
            y0 = data[i];
            y1 = data[i + 1];
        }
        else {
            y1 = data[i];
            y0 = data[i + 1];
        }

        if (y0 >= chart->Height) {
            continue;
        }
        y1 = (y1 < chart->Height) ? y1 : chart->Height - 1;

        y0 = chart->Height - y0 - 1;
        y1 = chart->Height - y1 - 1;

        for (uint16_t j = y1; j <= y0; j++)
        {
            pixel_color = CurveChart_GetRecoverPixelColor(chart, i, j);

#if CHART_USE_FRAMEBUFFER 
            FrameBuffer_WritePixel(&s_framebuffer, i, j, pixel_color);
#else
            WRITE_PIXEL(chart->X + i, chart->Y + j, pixel_color);
#endif // CHART_USE_FRAMEBUFFER 
        }
    }
}

void CurveChart_RecoverLineX(const CurveChartTypeDef *chart, uint16_t x)
{
    if (x > chart->Width) return;
    uint16_t pixel_color;
    for (uint16_t i = 0; i < chart->Height; i++)
    {
        pixel_color = CurveChart_GetRecoverPixelColor(chart, x, i);
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, x, i, pixel_color);
#else
        WRITE_PIXEL(chart->X + x, chart->Y + i, pixel_color);
#endif // CHART_USE_FRAMEBUFFER 
    }
}

void CurveChart_RecoverLineY(const CurveChartTypeDef *chart, uint16_t y)
{
    if (y > chart->Height) return;
    uint16_t pixel_color;

    for (uint16_t i = 0; i < chart->Width; i++)
    {
        pixel_color = CurveChart_GetRecoverPixelColor(chart, i, chart->Height - y);
#if CHART_USE_FRAMEBUFFER 
        FrameBuffer_WritePixel(&s_framebuffer, i, chart->Height - y, pixel_color);
#else
        WRITE_PIXEL(chart->X + i, chart->Y + chart->Height - y, pixel_color);
#endif // CHART_USE_FRAMEBUFFER 
    }
}

static inline uint16_t CurveChart_GetRecoverPixelColor(const CurveChartTypeDef *chart, uint16_t x0, uint16_t y0)
{
    if ((x0 % chart->CoarseGridWidth == 0) || (y0 % chart->CoarseGridHeight == 0)) {
        return chart->CoarseGridColor;
    }

    if ((x0 % chart->FineGridWidth == 0) || (y0 % chart->FineGridHeight == 0)) {
        return chart->FineGridColor;
    }

    return chart->BackgroudColor;
}