/**
  ******************************************************************************
  * @file       frame_buffer.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.18
  * @brief      Buffer to hold RGB565 pixel color data and map coordinates
  *
  * @note       There's no memory bounds check, you need to make sure all pixel
  *             positions are within the window when performing read/write ops.
  *             Call FrameBuffer_Update() to flush all pixels in buffer to 
  *             destination memory address (e.g. GRAM).
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "frame_buffer.h"
#include "lcd.h"

#if LCD_DRIVER_IC == NT35510
#include "nt35510.h"
#elif LCD_DRIVER_IC == ILI9341
#include "ili9341.h"
#elif LCD_DRIVER_IC == ILI9325
#include "ili9325.h"
#endif

/* Private Marcos ------------------------------------------------------------*/

/* External Variables --------------------------------------------------------*/
#if FRAME_BUFFER_USE_DMA
extern DMA_HandleTypeDef hdma_m2m;
#endif // FRAME_BUFFER_USE_DMA

/* Public Function Definitions -----------------------------------------------*/

/**
  * @brief  Initializes framebuffer
  * @param  fb: pointer to framebuffer instance
  * @param  base_addr: Buffer address in SRAM
  * @param  dst_addr: GRAM address the pixels will be flush to
  * @param  x: Specifies the X top-left position in screen area
  * @param  y: Specifies the Y top-left position in screen area
  * @param  width: Display window width in screen area
  * @param  height: Display window height in screen area
  * @retval None
  */
void FrameBuffer_Init(FrameBufferTypeDef *fb, 
                      const uint16_t *base_addr, const uint16_t *dst_addr, 
                      uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    fb->PixelData = base_addr;
    fb->DstAddr = dst_addr;
    fb->X = x;
    fb->Y = y;
    fb->Width = width;
    fb->Height = height;
}

/**
  * @brief  Fill pixel buffer with selected color
  * @param  fb: Pointer to pixel buffer structure
  * @param  color: RGB565 format color
  * @retval None
  */
void FrameBuffer_Clear(FrameBufferTypeDef *fb, uint16_t color)
{
    for (size_t i = 0; i < fb->Width * fb->Height; i++) {
        fb->PixelData[i] = color;
    }
}

/**
  * @brief  Update all pixels in buffer to GRAM
  * @param  fb: Pointer to pixel buffer structure
  * @retval None
  */
void FrameBuffer_Update(FrameBufferTypeDef *fb)
{
    uint32_t src_addr = fb->PixelData;
    size_t word_count = fb->Width * fb->Height / 2;

    SET_WINDOW(fb->X, fb->Y, fb->Width, fb->Height);
    PREPARE_WRITE();

#if FRAME_BUFFER_USE_DMA

    while (word_count > 0xFFFF) {
        HAL_DMA_Start(&hdma_m2m, src_addr, (uint32_t)fb->DstAddr, 0xFFFF);
        /* Polling for DMA to transfer pixel block of size 0xFFFFh == 65535 * 4 Bytes */
        HAL_DMA_PollForTransfer(&hdma_m2m, HAL_DMA_FULL_TRANSFER, 1000);
        src_addr += 0x3FFFC;
        word_count -= 0xFFFF;
    }

    HAL_DMA_Start(&hdma_m2m, src_addr, (uint32_t)fb->DstAddr, word_count);
    /* Polling for DMA to transfer remaining pixels */
    HAL_DMA_PollForTransfer(&hdma_m2m, HAL_DMA_FULL_TRANSFER, 1000);
#else
    for (size_t i = 0; i < fb->Width * fb->Height; i++) {
        WRITE_GRAM(fb->Pixels[i]);
    }
#endif // FRAME_BUFFER_USE_DMA
}