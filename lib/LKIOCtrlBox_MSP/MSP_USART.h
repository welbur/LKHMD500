#ifndef _MSP_USART_H
#define _MSP_USART_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include "stdio.h"	
//#include "../../include/usart.h"

extern uint8_t rx1_buf;

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif
