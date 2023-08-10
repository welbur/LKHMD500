#ifndef _MSP_USART_H
#define _MSP_USART_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "PowerBoardConfig.h"
#include "PinConfig.h"
#include "LOG.h"
#include <string.h>
#include <stdlib.h>
//#include "ErrorInfo.h"

extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART3_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif
