/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MSP_TIM_H__
#define __MSP_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "PowerBoardConfig.h"
#include "PinConfig.h"
#include "LOG.h"
//#include <string.h>
//#include <stdlib.h>

extern __IO uint32_t uhCCR1_Val;
extern __IO uint32_t uhCCR2_Val;
extern __IO uint32_t uhCCR3_Val;
extern __IO uint32_t uhCCR4_Val;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim14;

//void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM12_Init(void);
void MX_TIM14_Init(void);

void ReadPinValue(void);

#ifdef __cplusplus
}
#endif

#endif /* __MSP_TIM_H__ */

