/* Includes ------------------------------------------------------------------*/
#include "fsmc.h"

SRAM_HandleTypeDef hsram1;
SRAM_HandleTypeDef hsram_lcd;
static _Bool s_is_fsmc_initialized = 0;

void FSMC_Init(void)
{
    HAL_FSMC_MspInit();

    FSMC_NORSRAM_TimingTypeDef RW_Timing, W_Timing;

    /* Perform the SRAM1 memory initialization sequence */
    hsram1.Instance = FSMC_NORSRAM_DEVICE;
    hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram1.Init */
    hsram1.Init.NSBank = FSMC_NORSRAM_BANK3;
    hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
    /* RW_Timing */
    RW_Timing.AddressSetupTime = 0x00;
    RW_Timing.AddressHoldTime = 0x00;
    RW_Timing.DataSetupTime = 0x08;	//HCLK = 168MHz 6ns * 9 = 54ns
    RW_Timing.BusTurnAroundDuration = 0x00;
    RW_Timing.CLKDivision = 0x00;
    RW_Timing.DataLatency = 0x00;
    RW_Timing.AccessMode = FSMC_ACCESS_MODE_A;

    if (HAL_SRAM_Init(&hsram1, &RW_Timing, NULL) != HAL_OK)
    {
        Error_Handler();
    }

    /* Perform the TFTLCD SRAM initialization sequence */
    hsram_lcd.Instance = FSMC_NORSRAM_DEVICE;
    hsram_lcd.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram_lcd.Init */
    hsram_lcd.Init.NSBank = FSMC_NORSRAM_BANK4;
    hsram_lcd.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram_lcd.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    hsram_lcd.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram_lcd.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram_lcd.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram_lcd.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram_lcd.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram_lcd.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram_lcd.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram_lcd.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
    hsram_lcd.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram_lcd.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    hsram_lcd.Init.PageSize = FSMC_PAGE_SIZE_NONE;

    /* ReadWrite Timing */
    RW_Timing.AddressSetupTime = 0x01;		//HCLK = 168MHz об 6ns * 2 = 12ns
    RW_Timing.AddressHoldTime = 0x00;
    RW_Timing.DataSetupTime = 0x12;			//HCLK = 168MHz об 6ns * 23 = 162ns
    RW_Timing.BusTurnAroundDuration = 0x00;
    RW_Timing.CLKDivision = 0x00;
    RW_Timing.DataLatency = 0x00;
    RW_Timing.AccessMode = FSMC_ACCESS_MODE_A;

    /* Extended Write Timing */
    W_Timing.AddressSetupTime = 0x00;
    W_Timing.AddressHoldTime = 0x00;
    W_Timing.DataSetupTime = 0x02;			//HCLK = 168MHz об 6ns * 3 = 18ns
    W_Timing.BusTurnAroundDuration = 0x00;
    W_Timing.CLKDivision = 0x00;
    W_Timing.DataLatency = 0x00;
    W_Timing.AccessMode = FSMC_ACCESS_MODE_A;

    if (HAL_SRAM_Init(&hsram_lcd, &RW_Timing, &W_Timing) != HAL_OK)
    {
        Error_Handler();
    }
}

static void HAL_FSMC_MspInit(void)
{
    if (s_is_fsmc_initialized) {
        return;
    }
    s_is_fsmc_initialized = 1;

    /* GPIO clock enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* Peripheral clock enable */
    __HAL_RCC_FSMC_CLK_ENABLE();

    /** FSMC GPIO Configuration
    PF0   ------> FSMC_A0
    PF1   ------> FSMC_A1
    PF2   ------> FSMC_A2
    PF3   ------> FSMC_A3
    PF4   ------> FSMC_A4
    PF5   ------> FSMC_A5
    PF12   ------> FSMC_A6
    PF13   ------> FSMC_A7
    PF14   ------> FSMC_A8
    PF15   ------> FSMC_A9
    PG0   ------> FSMC_A10
    PG1   ------> FSMC_A11
    PE7   ------> FSMC_D4
    PE8   ------> FSMC_D5
    PE9   ------> FSMC_D6
    PE10   ------> FSMC_D7
    PE11   ------> FSMC_D8
    PE12   ------> FSMC_D9
    PE13   ------> FSMC_D10
    PE14   ------> FSMC_D11
    PE15   ------> FSMC_D12
    PD8   ------> FSMC_D13
    PD9   ------> FSMC_D14
    PD10   ------> FSMC_D15
    PD11   ------> FSMC_A16
    PD12   ------> FSMC_A17
    PD13   ------> FSMC_A18
    PD14   ------> FSMC_D0
    PD15   ------> FSMC_D1
    PG2   ------> FSMC_A12
    PG3   ------> FSMC_A13
    PG4   ------> FSMC_A14
    PG5   ------> FSMC_A15
    PD0   ------> FSMC_D2
    PD1   ------> FSMC_D3
    PD4   ------> FSMC_NOE
    PD5   ------> FSMC_NWE
    PG10   ------> FSMC_NE3
    PG12   ------> FSMC_NE4
    PE0   ------> FSMC_NBL0
    PE1   ------> FSMC_NBL1
    */

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
        | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
        | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10
        | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14
        | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
        | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13
        | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
        | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}