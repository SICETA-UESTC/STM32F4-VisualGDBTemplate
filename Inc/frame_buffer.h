/**
  ******************************************************************************
  * @file       frame_buffer.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Buffer to hold RGB565 pixel color data and map coordinates
  *
  * @note       There's no memory bounds check, you need to make sure all pixel
  *             positions are within the window when performing read/write.
  *             Call FrameBuffer_Update() to flush all pixels in buffer to
  *             destination memory address (e.g. GRAM).
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

/* Public Marcos -------------------------------------------------------------*/
#define FRAME_BUFFER_USE_DMA           1

/* Public Types --------------------------------------------------------------*/
typedef struct
{
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    __IO uint16_t *PixelData;
    __IO uint16_t *DstAddr;

} FrameBufferTypeDef;

/* Public Function Prototypes ------------------------------------------------*/
void FrameBuffer_Init(FrameBufferTypeDef *fb, 
                      const uint16_t *base_addr, const uint16_t *dst_addr,
                      uint16_t x, uint16_t y, uint16_t width, uint16_t height);

void FrameBuffer_Clear(FrameBufferTypeDef *fb, uint16_t color);
void FrameBuffer_Update(FrameBufferTypeDef *fb);

/* Public Inline Functions ---------------------------------------------------*/
inline void FrameBuffer_WritePixel(FrameBufferTypeDef *fb, uint16_t x, uint16_t y, uint16_t color)
{
    fb->PixelData[fb->Width * y + x] = color;
}

inline uint16_t FrameBuffer_ReadPixel(const FrameBufferTypeDef *fb, uint16_t x, uint16_t y)
{
    return fb->PixelData[fb->Width * y + x];
}