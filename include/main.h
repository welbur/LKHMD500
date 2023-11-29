/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 
 
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "BoardConfig.h"
#include "stdint.h"
#include "stm32g4xx_hal.h"

	/* Private includes ----------------------------------------------------------*/
	/* USER CODE BEGIN Includes */

#include "MSP_GPIO.h"
#include "MSP_USART.h"
#include "MSP_ADC.h"
#include "MSP_DAC.h"
#include "MSP_TIM.h"
#include "MSP_DMA.h"

#include "PinConfig.h"

#include "LOG.h"

#include "EtherCatPDO.h"
#include "DCMCtrl.h"
#include "BoardConfig.h"
//#include "BackPanelTrans.h"
//#include "FrameFormat.h"


extern void Error_Handler(void);
void usb_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
