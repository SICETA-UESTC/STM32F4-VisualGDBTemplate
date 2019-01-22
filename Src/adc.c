#include "adc.h"

ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adc_handle)
{
    if (adc_handle->Instance == ADC1)
    {
         /* ADC1 clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        /* GPIO clock enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PA0-WKUP     ------> ADC1_IN0

        */
        GPIO_InitTypeDef GPIO_InitStruct;

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* Peripheral DMA init*/
        hdma_adc1.Instance = DMA2_Stream4;
        hdma_adc1.Init.Channel = DMA_CHANNEL_0;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_NORMAL;
        hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(adc_handle, DMA_Handle, hdma_adc1);
    }

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adc_handle)
{
    if (adc_handle->Instance == ADC1)
    {
        __HAL_RCC_ADC1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
        HAL_DMA_DeInit(adc_handle->DMA_Handle);
        HAL_NVIC_DisableIRQ(ADC_IRQn);
    }
}

