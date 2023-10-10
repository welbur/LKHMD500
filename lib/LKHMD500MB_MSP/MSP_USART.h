#ifndef _MSP_USART_H
#define _MSP_USART_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "BoardConfig.h"
#include "PinConfig.h"
#include "LOG.h"
#include <string.h>
#include <stdlib.h>
//#include "ErrorInfo.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART6_UART_Init(void);
void MX_USART3_UART_Init(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
