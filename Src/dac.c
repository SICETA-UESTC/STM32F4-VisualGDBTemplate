#include "dac.h"

DAC_HandleTypeDef hdac;
//extern DMA_HandleTypeDef hdma_dac;
//extern TIM_HandleTypeDef htim6;

//static uint16_t dacValue[128];

void DAC_Init(void)
{
    /* DAC clock enable */
    __HAL_RCC_DAC_CLK_ENABLE();

    /* GPIO clock enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**DAC GPIO Configuration
    PA4     ------> DAC_OUT1
    */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hdac.Instance = DAC;
    HAL_DAC_Init(&hdac);

    DAC_ChannelConfTypeDef sConfig;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
    /*
    TIM6_DAC_Trigger_Init();

    hdma_dac.Instance = DMA1_Stream5;
    hdma_dac.Init.Channel = DMA_CHANNEL_7;
    hdma_dac.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac.Init.Mode = DMA_CIRCULAR;
    hdma_dac.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dac.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_dac) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac);

    for (uint16_t i = 0; i < 50; i++)
    {
        dacValue[i] = i << 6;
    }*/
}
/*
void DAC_Output(void)
{
    HAL_TIM_Base_Start(&htim6);
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, &dacValue, 128, DAC_ALIGN_12B_R);
}
*/