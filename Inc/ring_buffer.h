/**
  ******************************************************************************
  * @file       ring_buffer.h
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.16
  * @brief      FIFO ring buffer, refer to kfifo from Linux kernel
  *
  * @note       Useful stuff, especially when dealing with async I/O
  ******************************************************************************
  */

/* Preprocessor Directives ---------------------------------------------------*/
#pragma once

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

/* Public Marcos -------------------------------------------------------------*/
#define MIN(A, B)  (((A) < (B)) ? (A) : (B))
#define IS_POWER_OF_2(X) (((X) & ((X) - 1)) == 0)

/* Public Types --------------------------------------------------------------*/
typedef struct {
    uint8_t *BaseAddr;
    size_t Capacity;
    size_t WritePos;
    size_t ReadPos;
    //uint8_t Status;
} RingBufferTypeDef;

/* Public function prototypes ------------------------------------------------*/
HAL_StatusTypeDef RingBuffer_Init(RingBufferTypeDef *fifo, const uint8_t *base_addr, size_t capacity);
void RingBuffer_Reset(RingBufferTypeDef *fifo);

size_t RingBuffer_GetBytesCount(const RingBufferTypeDef *fifo);
size_t RingBuffer_WriteBytes(RingBufferTypeDef *fifo, const uint8_t *buffer, size_t length);
size_t RingBuffer_ReadBytes(RingBufferTypeDef *fifo, uint8_t *buffer, size_t length);

size_t RingBuffer_WriteString(RingBufferTypeDef *fifo, const uint8_t *str);
size_t RingBuffer_ReadString(RingBufferTypeDef *fifo, uint8_t *str, size_t max_length);
