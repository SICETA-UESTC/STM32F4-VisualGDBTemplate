/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "dma.h"
#include "ring_buffer.h"
#include <memory.h>

UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

static RingBufferTypeDef s_huart1_rx_fifo;
static uint8_t s_uart1_rx_buffer[UART_RX_BUFFER_SIZE];

/* USART1 init function */
void USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    RingBuffer_Init(&s_huart1_rx_fifo, s_uart1_rx_buffer, UART_RX_BUFFER_SIZE);
    HAL_UART_Receive_DMA(&huart1, s_huart1_rx_fifo.BaseAddr, UART_RX_BUFFER_SIZE);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uart_handle)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (uart_handle->Instance == USART1)
    {
        /* GPIO clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA2_Stream2;
        hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(uart_handle, hdmarx, hdma_usart1_rx);

        /* USART1_TX Init */
        hdma_usart1_tx.Instance = DMA2_Stream7;
        hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(uart_handle, hdmatx, hdma_usart1_tx);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uart_handle)
{
    if (uart_handle->Instance == USART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uart_handle->hdmarx);
        HAL_DMA_DeInit(uart_handle->hdmatx);
    }
}

size_t USART1_FIFO_ReadBytes(uint8_t *buffer, size_t length)
{
    s_huart1_rx_fifo.WritePos = UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

    size_t avaliable_bytes_count = (s_huart1_rx_fifo.WritePos > s_huart1_rx_fifo.ReadPos)
                                    ? s_huart1_rx_fifo.WritePos - s_huart1_rx_fifo.ReadPos
                                    : UART_RX_BUFFER_SIZE + s_huart1_rx_fifo.WritePos - s_huart1_rx_fifo.ReadPos;

    length = MIN(length, avaliable_bytes_count);
    size_t to_end_length = MIN(length, UART_RX_BUFFER_SIZE - s_huart1_rx_fifo.ReadPos);

    memcpy(buffer, s_huart1_rx_fifo.BaseAddr + s_huart1_rx_fifo.ReadPos, to_end_length);
    memcpy(buffer + to_end_length, s_huart1_rx_fifo.BaseAddr, length - to_end_length);

    s_huart1_rx_fifo.ReadPos = (s_huart1_rx_fifo.ReadPos + length) % UART_RX_BUFFER_SIZE;
    return length;
}