/* Includes ------------------------------------------------------------------*/
#include "tim.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim13;

/* If you don't need to use HAL_Delay() or don't mess around with SysTick */
/*
void DelayMs(uint16_t ms)
{
	uint32_t temp;
	SysTick->LOAD = ms * 21000;
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	do
	{
		temp = SysTick->CTRL;
	} while (temp & 0x01 && !(temp & (1 << 16)));
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
}

void DelayUs(uint16_t us)
{
	uint32_t temp;
	SysTick->LOAD = us * 21;
	SysTick->VAL = 0;
	SysTick->CTRL = 1;
	do
	{
		temp = SysTick->CTRL;
	} while (temp & 0x01 && !(temp & (1 << 16)));
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
}
*/

/* TIM7 is used for us delay */
void TIM7_Init(void)
{
    htim7.Instance = TIM7;
    htim7.Init.Prescaler = 83;
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.Period = 0xFFFF;
    if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Delay in us */
void DelayUs(uint16_t us)
{
    uint16_t differ = 0xFFFF - us - 4;

    HAL_TIM_Base_Start(&htim7);
    __HAL_TIM_SetCounter(&htim7, differ);

    while (differ < 0xffff - 5)
    {
        differ = __HAL_TIM_GetCounter(&htim7);
    }

    HAL_TIM_Base_Stop(&htim7);
}

void TIM2_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 6;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 119;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 3;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}

    /* GPIO clock enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
	/**TIM2 GPIO Configuration
	PB11     ------> TIM2_CH4
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void TIM3_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 6;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 399;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIM5_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 13;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 0xFFFFFFFF;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 4;
	if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}

    /* GPIO clock enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
	/**TIM5 GPIO Configuration
	PA3     ------> TIM5_CH4
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* TIM6 init function */
void TIM6_Init(void)
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 8399;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 10000;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
}

/* TIM13 init function */
void TIM13_PWM_Output_Init(uint16_t period)
{
	TIM_OC_InitTypeDef sConfigOC;

	htim13.Instance = TIM13;
	htim13.Init.Prescaler = 83;							//分频系数 84MHz / (83 + 1) => 1MHz
	htim13.Init.CounterMode = TIM_COUNTERMODE_UP;		//向上计数模式
	htim13.Init.Period = period - 1;					//周期

	if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = period / 2 + 1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

    /* GPIO clock enable */
    __HAL_RCC_GPIOF_CLK_ENABLE();
	/**TIM13 GPIO Configuration
	PF8     ------> TIM13_CH1
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_handle)
{
	if (tim_handle->Instance == TIM2)
	{
		/* TIM2 clock enable */
		__HAL_RCC_TIM2_CLK_ENABLE();
		/* TIM2 interrupt Init */
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if (tim_handle->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();
	}
	else if (tim_handle->Instance == TIM5)
	{
		/* TIM5 clock enable */
		__HAL_RCC_TIM5_CLK_ENABLE();
		/* TIM5 interrupt Init */
		HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
	}
	else if (tim_handle->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();
		/* TIM6 interrupt Init */
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
	else if (tim_handle->Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();
	}
	else if (tim_handle->Instance == TIM13)
	{
		__HAL_RCC_TIM13_CLK_ENABLE();
	}
}


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_handle)
{
	if (tim_handle->Instance == TIM2)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM2_CLK_DISABLE();
		/* TIM2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}
	else if (tim_handle->Instance == TIM3)
	{
		__HAL_RCC_TIM3_CLK_DISABLE();
	}
	else if (tim_handle->Instance == TIM5)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM5_CLK_DISABLE();
		/* TIM6 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM5_IRQn);
	}
	else if (tim_handle->Instance == TIM6)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM6_CLK_DISABLE();
		/* TIM6 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
	}
	else if (tim_handle->Instance == TIM7)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM7_CLK_DISABLE();
	}
	else if (tim_handle->Instance == TIM13)
	{
		__HAL_RCC_TIM13_CLK_DISABLE();
	}
}