/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "MSP_TIM.h"

/* Private typedef -----------------------------------------------------------*/
#if 0
#define PERIOD_VALUE (uint32_t)(2000 - 1)		   /* Period Value  */
#define PULSE1_VALUE (uint32_t)(PERIOD_VALUE / 5)  /* Capture Compare 1 Value  */
#define PULSE2_VALUE (uint32_t)(PERIOD_VALUE / 2)  /* Capture Compare 2 Value  */
#define PULSE3_VALUE (uint32_t)(PERIOD_VALUE / 4)  /* Capture Compare 3 Value  */
#define PULSE4_VALUE (uint32_t)(PERIOD_VALUE / 10) /* Capture Compare 4 Value  */
#endif

__IO uint32_t TIM6tick_10us = 0;		//用于产生10us计数

__IO uint32_t uhCCR1_Val = 600;
__IO uint32_t uhCCR2_Val = 20000;
__IO uint32_t uhCCR3_Val = 8012;
__IO uint32_t uhCCR4_Val = 4006;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/**
 * @brief TIM3 Initialization Function, 用于PWM输出
 * @param None
 * @retval None
 */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}


/**
 * @brief TIM4 Initialization Function, 用于PWM输出
 * @param None
 * @retval None
 */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
 * @brief TIM6 Initialization Function， 用于产生us级别的计数
 * @param None
 * @retval None
 */
void MX_TIM6_Init(void)
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = (uint32_t)(SystemCoreClock / 10000000) - 1; // 10MHz = 0.1us
//  LOG("systemcore clock : %ld\r\n", htim6.Init.Prescaler);
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = (uint32_t)((10000000 / 100000) - 1);    // 0.1us * 100 = 10us
//  LOG("period : %ld\r\n", htim6.Init.Period);

	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		LOG_error("tim6 base init error!\r\n"); // Error_Handler();
	}

	HAL_TIM_Base_Start_IT(&htim6);
}


/**
 * @brief TIM7 Initialization Function， 用于给eos系统用
 * @param None
 * @retval None
 */
void MX_TIM7_Init(void)
{
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (uint32_t)(SystemCoreClock / 1000000) - 1; //  1MHz = 1us
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = (1000000 / 1000) - 1;    //9; 1us * 1000 = 1ms
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	// htim14.Init.RepetitionCounter = 0;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		LOG_error("tim7 base init error!\r\n"); // Error_Handler();
	}

	HAL_TIM_Base_Start_IT(&htim7);
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
	if (htim_base->Instance == TIM6)
	{
		/* TIM6 clock enable */
		__HAL_RCC_TIM6_CLK_ENABLE();

		/* TIM6 interrupt Init */
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 4);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
  	else if (htim_base->Instance == TIM7)
  	{
		/* TIM7 clock enable */
		__HAL_RCC_TIM7_CLK_ENABLE();

		/* TIM6 interrupt Init */
		HAL_NVIC_SetPriority(TIM7_DAC_IRQn, 1, 5);
		HAL_NVIC_EnableIRQ(TIM7_DAC_IRQn);
  	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim_base)
{

	if (htim_base->Instance == TIM6)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM6_CLK_DISABLE();

		/* TIM6 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
	}
	else if (htim_base->Instance == TIM7)
	{
		/* Peripheral clock disable */
		__HAL_RCC_TIM7_CLK_DISABLE();

		/* TIM6 interrupt Deinit */
		HAL_NVIC_DisableIRQ(TIM7_DAC_IRQn);
	}
}



#if 1
/**
 * @brief  HAL_getTick_us
 * @note   每隔10us计数加一
 * @param  none
 * @retval TIM6tick_10us
 */
uint32_t HAL_getTick_us(void) {
    return TIM6tick_10us;
}
#endif