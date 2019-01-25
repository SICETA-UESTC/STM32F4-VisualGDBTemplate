/* Includes ------------------------------------------------------------------*/
#include "spi.h"

SPI_HandleTypeDef hspi2;

/* SPI2 init function */
void SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spi_handle)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if (spi_handle->Instance == SPI2)
    {
        /* GPIO clock enable */
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* SPI2 clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();

        /**SPI2 GPIO Configuration
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI2_MspInit 1 */

        /* USER CODE END SPI2_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spi_handle)
{
    if (spi_handle->Instance == SPI2)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI2_CLK_DISABLE();

        /**SPI2 GPIO Configuration
        PB13     ------> SPI2_SCK
        PB14     ------> SPI2_MISO
        PB15     ------> SPI2_MOSI
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    }
}

void SPI_SetSpeed(SPI_HandleTypeDef* spi_handle, uint16_t baudrate_prescaler)
{
    spi_handle->Instance->CR1 = ((uint16_t)(SPI2->CR1 & 0xFFC7)) | (baudrate_prescaler & 0x0038);
    spi_handle->Instance->CR1 |= SPI_CR1_SPE;
}

uint8_t SPI_ReadWriteByte(SPI_HandleTypeDef* spi_handle, uint8_t data)
{
    while (!(spi_handle->Instance->SR & SPI_FLAG_TXE)) { __NOP(); }
    spi_handle->Instance->DR = data;
    while (!(spi_handle->Instance->SR & SPI_FLAG_RXNE)) { __NOP(); }
    return SPI2->DR;
}