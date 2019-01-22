/**
  ******************************************************************************
  * @file       ring_buffer.c
  * @author     Weng Xiaoran, SICEIEC-UESTC
  * @date       2019.1.16
  * @brief      FIFO ring buffer, refer to kfifo from Linux kernel
  *
  * @note       Useful stuff, especially when dealing with async I/O
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ring_buffer.h"
#include <memory.h>
#include <string.h>

/* Public Function Definitions -----------------------------------------------*/

/**
  * @brief  Initializes ringbuffer
  * @param  fifo: Pointer to ringbuffer instance
  * @param  base_addr: Buffer address in SRAM
  * @param  capacity: Maximum number of btyes buffer can hold
  * @retval HAL status
  */
HAL_StatusTypeDef RingBuffer_Init(RingBufferTypeDef *fifo, const uint8_t *base_addr, size_t capacity)
{
    if (fifo == NULL || !IS_POWER_OF_2(capacity)) {
        return HAL_ERROR;
    }

    fifo->BaseAddr = base_addr;
    fifo->Capacity = capacity;
    fifo->ReadPos = 0;
    fifo->WritePos = 0;
    return HAL_OK;
}

/**
  * @brief  Resets ringbuffer to empty status
  * @param  fifo: Pointer to ringbuffer instance
  * @retval None
  */
void RingBuffer_Reset(RingBufferTypeDef *fifo)
{
    fifo->WritePos = fifo->ReadPos = 0;
}

/**
  * @brief  Get current number of bytes stored in buffer
  * @param  fifo: Pointer to ringbuffer instance
  * @retval Number of bytes
  */
size_t RingBuffer_GetBytesCount(const RingBufferTypeDef *fifo)
{
    return fifo->WritePos - fifo->ReadPos;
}

/**
  * @brief  Writes data into buffer
  * @param  fifo: Pointer to ringbuffer instance
  * @param  buffer: Data buffer address
  * @param  length: Number of bytes to write
  * @retval Actually number of bytes written
  */
size_t RingBuffer_WriteBytes(RingBufferTypeDef *fifo, const uint8_t *buffer, size_t length)
{
    if (fifo == NULL || length == 0) {
        return 0;
    }

    length = MIN(length, fifo->Capacity - fifo->WritePos + fifo->ReadPos);

    /* Map write position into buffer address range */
    size_t in_buffer_pos = fifo->WritePos & (fifo->Capacity - 1);
    size_t to_end_length = MIN(length, fifo->Capacity - in_buffer_pos);

    memcpy(fifo->BaseAddr + in_buffer_pos, buffer, to_end_length);
    memcpy(fifo->BaseAddr, buffer + to_end_length, length - to_end_length);

    fifo->WritePos += length;
    return length;
}

/**
  * @brief  Reads data from buffer
  * @param  fifo: Pointer to ringbuffer instance
  * @param  buffer: Data buffer address
  * @param  length: Number of bytes to read
  * @retval Actually number of bytes read
  */
size_t RingBuffer_ReadBytes(RingBufferTypeDef *fifo, uint8_t *buffer, size_t length)
{
    if (fifo == NULL || length == 0) {
        return 0;
    }

    length = MIN(length, fifo->WritePos - fifo->ReadPos);

    /* Map read position into buffer address range */
    size_t in_buffer_pos = fifo->ReadPos & (fifo->Capacity - 1);
    size_t to_end_length = MIN(length, fifo->Capacity - in_buffer_pos);

    memcpy(buffer, fifo->BaseAddr + in_buffer_pos, to_end_length);
    memcpy(buffer + to_end_length, fifo->BaseAddr, length - to_end_length);

    fifo->ReadPos += length;
    return length;
}

/**
  * @brief  Writes a string into buffer
  * @param  fifo: Pointer to ringbuffer instance
  * @param  str: Pointer to string buffer
  * @retval Actually number of bytes written
  */
size_t RingBuffer_WriteString(RingBufferTypeDef *fifo, const uint8_t *str)
{
    return RingBuffer_WriteBytes(fifo, str, strlen(str));
}

/**
  * @brief  Reads a string from buffer
  * @param  fifo: Pointer to ringbuffer instance
  * @param  str: Pointer to string buffer
  * @param  max_length: Maximum length for the string
  * @retval Actually number of bytes read
  */
size_t RingBuffer_ReadString(RingBufferTypeDef *fifo, uint8_t *str, size_t max_length)
{
    size_t length = RingBuffer_ReadBytes(fifo, str, max_length);
    str[length] = '\0';
    return length;
}