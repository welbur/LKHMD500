/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention      : LKHM840 PowerBoard
 *                   2023-08-11  V0.0.3
 *
 *       1、usart1用于与主控板通信用，采用私有背板协议，采用RS485方式
 *           PA9     ------> USART1_TX
 *           PA10    ------> USART1_RX
 *           BaudRate = 921600
 *
 *       2、usart6用于与主控板通信用，第二个RS485，备用
 *           PA11     ------> USART6_TX
 *           PA12     ------> USART6_RX
 *           BaudRate = 921600
 *
 *       3、usart3用于打印测试信息
 *           PC10    ------> USART3_TX
 *           PC11    ------> USART3_RX
 *           BaudRate = 921600
 *
 * 		 4、通过tim输出pwm信号，或者是相位角控制信号
 *  		 *  TIM3 GPIO Configuration
 *			    PC6     ------> TIM3_CH1
 *		 	    PC7     ------> TIM3_CH2
 *			    PC8     ------> TIM3_CH3
 *			    PC9     ------> TIM3_CH4
 *			 *  TIM4 GPIO Configuration
 *			    PB6     ------> TIM4_CH1
 *			    PB7     ------> TIM4_CH2
 *			    PB8     ------> TIM4_CH3
 *			    PB9     ------> TIM4_CH4
 *			 *  TIM12 GPIO Configuration
 *		  	    PB15     ------> TIM12_CH2
 *
 *	 	 5、过零检测信号输入
 * 				PB0      ------> ZRC_IN1
 * 				PB1      ------> ZRC_IN2
 * 				PB2      ------> ZRC_IN3
 * 				PB3      ------> ZRC_IN4
 * 				PB4      ------> ZRC_IN5
 * 				PB5      ------> ZRC_IN6
 * 				PB10     ------> ZRC_IN7
 * 				PB12     ------> ZRC_IN8
 * 				PB13     ------> ZRC_IN9
 * 
 * 		 6、ADC信号进入 （使用MLX91220时采用）
 *     		 *  ADC1 GPIO Configuration
 *			    PA0     ------> ADC1_IN0
 *			    PA1     ------> ADC1_IN1
 *			    PA2     ------> ADC1_IN2
 *			    PA3     ------> ADC1_IN3
 *			    PA4     ------> ADC1_IN4
 *			    PA5     ------> ADC1_IN5
 *			    PA6     ------> ADC1_IN6
 *			    PA7     ------> ADC1_IN7
 *			    PC4     ------> ADC1_IN14
 *
 * 		 7、热电偶转换芯片 简单SPI接口（只读）
 * 				
 * 
 *       8、WorkLed, BoardID
 *              PC0     ------> WorkLed
 *  			PC1     ------> BoardID_1
 * 				PC2     ------> BoardID_2
 * 
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
#include "MSP_TIM.h"
#include "MSP_DMA.h"

#include "PinConfig.h"

#include "LOG.h"

#include "EtherCatPDO.h"
//#include "BackPanelTrans.h"
//#include "FrameFormat.h"


// 复制一个数组到另一个数组
#define COPY_ARRAY(dest, src, len) memcpy(dest, src, (len) * sizeof((src)[0]))
// 板ID位操作函数
// #define Enable_Board(reg, bit)   ((reg) |= (1 << (bit)))
// #define Disable_Baord(reg, bit) ((reg) &= ~(1 << (bit)))
#define whichBoard_Enable(reg, bit) ((reg) & (1 << (bit))) //(reg & (1 << bit)) >> bit
// #define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))



extern void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
