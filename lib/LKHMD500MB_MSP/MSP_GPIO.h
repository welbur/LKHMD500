#ifndef _MSP_GPIO_H_
#define	_MSP_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "PinConfig.h"
#include "BoardConfig.h"
#include <stdio.h>
#include "LOG.h"
//#include "ErrorInfo.h"



#define WorkLed(n)			(n?HAL_GPIO_WritePin(WorkLed_GPIO_Port,WorkLed_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(WorkLed_GPIO_Port,WorkLed_Pin,GPIO_PIN_RESET))
#define WorkLed_TogglePin     HAL_GPIO_TogglePin(WorkLed_GPIO_Port,WorkLed_Pin)	//WorkLed 闪烁 


void MX_GPIO_Init(void);

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
