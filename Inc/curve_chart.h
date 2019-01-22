/**
  ******************************************************************************
  * @file       curve_chart.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Dynamic curve chart UI control
  *
  * @note       You need to initialize LCD driver before use this module
  *             If you're using framebuffer, when time is opportune, call
  *             CurveChart_FrameUpdate() to update chart area to screen.
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>
#include "lcd.h"

/* Public Marcos -------------------------------------------------------------*/
/* Note that if the whole screen area is using frambuffer, it will be redundant for chart to enable this */
#if LCD_USE_FRAMEBUFFER
#define CHART_USE_FRAMEBUFFER           0
#else
#define CHART_USE_FRAMEBUFFER           1
#endif

/* Public Types --------------------------------------------------------------*/
typedef struct
{
    uint16_t X;
    uint16_t Y;

    uint16_t Width;
    uint16_t Height;
    uint16_t CoarseGridWidth;
    uint16_t CoarseGridHeight;
    uint16_t FineGridWidth;
    uint16_t FineGridHeight;

    uint16_t BorderColor;
    uint16_t BackgroudColor;
    uint16_t CoarseGridColor;
    uint16_t FineGridColor;

} CurveChartTypeDef;

/* Public Function Prototypes ------------------------------------------------*/
void CurveChart_Init(CurveChartTypeDef *chart);
void CurveChart_DrawBitmap(const CurveChartTypeDef *chart,
                           uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *bitmap_buffer);
void CurveChart_RecoverRect(const CurveChartTypeDef *chart, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void CurveChart_DrawCurve(const CurveChartTypeDef *chart, const uint16_t *data, uint16_t color);
void CurveChart_DrawLineX(const CurveChartTypeDef *chart, uint16_t x, uint16_t color);
void CurveChart_DrawDashedLineX(const CurveChartTypeDef *chart, uint16_t x, uint16_t color);
void CurveChart_DrawLineY(const CurveChartTypeDef *chart, uint16_t y, uint16_t color);
void CurveChart_DrawDashedLineY(const CurveChartTypeDef *chart, uint16_t y, uint16_t color);
void CurveChart_RecoverGrid(const CurveChartTypeDef *chart, const uint16_t *data);
void CurveChart_RecoverLineX(const CurveChartTypeDef *chart, uint16_t x);
void CurveChart_RecoverLineY(const CurveChartTypeDef *chart, uint16_t y);

#if CHART_USE_FRAMEBUFFER
void CurveChart_FrameUpdate(void);
#endif // LCD_USE_FRAMEBUFFER

/* Private Function Prototypes -----------------------------------------------*/
static inline uint16_t CurveChart_GetRecoverPixelColor(const CurveChartTypeDef *chart, uint16_t x0, uint16_t y0);