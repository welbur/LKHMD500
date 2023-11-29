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
	HAL_GPIO_WritePin(DCM1_EN_Port, DCM1_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DCM2_EN_Port, DCM2_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DCM3_EN_Port, DCM3_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DCM4_EN_Port, DCM4_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DCM5_EN_Port, DCM5_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(WorkLed_GPIO_Port, WorkLed_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(D51_RSTn_Port, D51_RSTn_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : D51_RSTn*/
	GPIO_InitStruct.Pin = D51_RSTn_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(D51_RSTn_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DCM1_EN_Pin DCM2_EN_Pin DCM3_EN_Pin DCM4_EN_Pin DCM5_EN_Pin*/
	GPIO_InitStruct.Pin = DCM1_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DCM1_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM2_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DCM2_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM3_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DCM3_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM4_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DCM4_EN_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM5_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DCM5_EN_Port, &GPIO_InitStruct);

#ifdef DEVBoardG4
  	/*Configure GPIO pin : KEY_Pin_Pin */
  	GPIO_InitStruct.Pin = KEY_Pin;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;	//GPIO_NOPULL;
  	HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
#endif
	/*Configure GPIO pin : WorkLed_Pin */
	GPIO_InitStruct.Pin = WorkLed_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(WorkLed_GPIO_Port, &GPIO_InitStruct);

#if 1
	/*Configure GPIO pins : DCM1_Fault_Pin DCM2_Fault_Pin DCM3_Fault_Pin DCM4_Fault_Pin
							 DCM5_Fault_Pin */
	GPIO_InitStruct.Pin = DCM1_Fault_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	//GPIO_MODE_IT_RISING;	//GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(DCM1_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM2_Fault_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM2_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM3_Fault_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM3_Fault_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM4_Fault_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM4_Fault_Port, &GPIO_InitStruct);
#ifndef DEVBoardG4
	GPIO_InitStruct.Pin = DCM5_Fault_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM5_Fault_Port, &GPIO_InitStruct);
#endif
#endif

#if 1
	/*Configure GPIO pins : DCM1_Pdet DCM2_Pdet DCM3_Pdet DCM4_Pdet
							 DCM5_Pdet */
	GPIO_InitStruct.Pin = DCM1_Pdet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM1_Pdet_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = DCM2_Pdet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM2_Pdet_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DCM3_Pdet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM3_Pdet_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DCM4_Pdet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM4_Pdet_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DCM5_Pdet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DCM5_Pdet_Port, &GPIO_InitStruct);

#endif
#if 0
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 10);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 10);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 10);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 10);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif
}
