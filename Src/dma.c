/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

DMA_HandleTypeDef hdma_m2m;

DMA_HandleTypeDef hdma_adc1;

DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_m2m
  */
void DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Configure DMA request hdma_m2m on DMA2_Stream0 */
    hdma_m2m.Instance = DMA2_Stream0;
    hdma_m2m.Init.Channel = DMA_CHANNEL_0;
    hdma_m2m.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_m2m.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_m2m.Init.MemInc = DMA_MINC_DISABLE;
    hdma_m2m.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_m2m.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_m2m.Init.Mode = DMA_NORMAL;
    hdma_m2m.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_m2m.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_m2m.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_m2m.Init.MemBurst = DMA_MBURST_INC4;
    hdma_m2m.Init.PeriphBurst = DMA_MBURST_INC4;
    if (HAL_DMA_Init(&hdma_m2m) != HAL_OK)
    {
        Error_Handler();
    }

    /* DMA interrupt init */

    /* USART TX RX DMA */
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    /* DMA2_Stream7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    /* SDIO TX RX DMA */
    /* DMA2_Stream3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    /* DMA2_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 4, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
}

/* USART1 DMA global interrupt*/
void DMA2_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

/* ADC DMA global interrupt*/
void DMA2_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

/* SDIO DMA global interrupt*/
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_rx);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_tx);
}