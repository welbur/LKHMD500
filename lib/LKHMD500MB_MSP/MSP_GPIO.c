/**
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  * @file       BSP_GPIO.c/h
  * @brief      GPIO的二次封装
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V3.0.0     2020.7.14     	              	1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  */
#include "MSP_GPIO.h"

// uint8_t DefaultBoardID = 0;
// HAL_TickFreqTypeDef TickFreq = HAL_TICK_FREQ_DEFAULT; /* 1KHz */

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 * Free pins are configured automatically as Analog (this feature is enabled through
 * the Code Generation settings)
 */
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(PowerM_1_EN_Port, PowerM_1_EN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PowerM_2_EN_Port, PowerM_2_EN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PowerM_3_EN_Port, PowerM_3_EN, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(PowerM_4_EN_Port, PowerM_4_EN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PowerM_5_EN_Port, PowerM_5_EN, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(WorkLed_GPIO_Port, WorkLed_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PowerM_1_EN_Pin PowerM_2_EN_Pin PowerM_3_EN_Pin PowerM_4_EN_Pin PowerM_5_EN_Pin*/
	GPIO_InitStruct.Pin = PowerM_1_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PowerM_1_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_2_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PowerM_2_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_3_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PowerM_3_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_4_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PowerM_4_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_5_EN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PowerM_5_EN_Port, &GPIO_InitStruct);

#ifdef DEVBoardG4
  	/*Configure GPIO pin : KEY_Pin_Pin */
  	GPIO_InitStruct.Pin = KEY_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
#endif
	/*Configure GPIO pin : WorkLed_Pin */
	GPIO_InitStruct.Pin = WorkLed_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(WorkLed_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PowerM_1_Fault_Pin PowerM_2_Fault_Pin PowerM_3_Fault_Pin PowerM_4_Fault_Pin
							 PowerM_5_Fault_Pin */
	GPIO_InitStruct.Pin = PowerM_1_Fault;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PowerM_1_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_2_Fault;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PowerM_2_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_3_Fault;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PowerM_3_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_4_Fault;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PowerM_4_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PowerM_5_Fault;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PowerM_5_Fault_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
