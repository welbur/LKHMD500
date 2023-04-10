#ifndef _MSP_GPIO_H_
#define	_MSP_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "SlaveBoardConfig.h"
//#include "SPITransfer_C.h"
#include <stdio.h>

#ifdef DEVBoard
#define KEY_Pin                         GPIO_PIN_13
#define KEY_GPIO_Port                   GPIOC
#define KEY_Pin_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY_Pin_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define KEY_Pin_EXTI_IRQn               EXTI0_IRQn

#define RED_Pin GPIO_PIN_0
#define RED_GPIO_Port GPIOC
#define GREEN_Pin GPIO_PIN_1
#define GREEN_GPIO_Port GPIOC
#define BLUE_Pin GPIO_PIN_2
#define BLUE_GPIO_Port GPIOC

#define LED_R(n)			(n?HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_RESET))
#define LED_R_TogglePin		HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin)	//LED_R��ƽ��ת

#define LED_G(n)			(n?HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_RESET))
#define LED_G_TogglePin     HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin)	//LED_G��ƽ��ת

#define LED_B(n)			(n?HAL_GPIO_WritePin(BLUE_GPIO_Port,BLUE_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(BLUE_GPIO_Port,BLUE_Pin,GPIO_PIN_RESET))
#define LED_B_TogglePin     HAL_GPIO_TogglePin(BLUE_GPIO_Port,BLUE_Pin)	//LED_B��ƽ��ת

#endif

#ifdef DEVBoardYD
#define KEY_Pin                         GPIO_PIN_3
#define KEY_GPIO_Port                   GPIOB
#define KEY_Pin_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define KEY_Pin_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#define KEY_Pin_EXTI_IRQn               EXTI3_IRQn

#define RED_Pin GPIO_PIN_13
#define RED_GPIO_Port GPIOA
#define GREEN_Pin GPIO_PIN_14
#define GREEN_GPIO_Port GPIOA
#define BLUE_Pin GPIO_PIN_15
#define BLUE_GPIO_Port GPIOA

#define LED_R(n)			(n?HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,GPIO_PIN_RESET))
#define LED_R_TogglePin		HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin)	//LED_R��ƽ��ת

#define LED_G(n)			(n?HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,GPIO_PIN_RESET))
#define LED_G_TogglePin     HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin)	//LED_G��ƽ��ת

#define LED_B(n)			(n?HAL_GPIO_WritePin(BLUE_GPIO_Port,BLUE_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(BLUE_GPIO_Port,BLUE_Pin,GPIO_PIN_RESET))
#define LED_B_TogglePin     HAL_GPIO_TogglePin(BLUE_GPIO_Port,BLUE_Pin)	//LED_B��ƽ��ת

#endif

#define TrigINT_Pin GPIO_PIN_1
#define TrigINT_GPIO_Port GPIOB
#define TrigINT_ToMasterB(n)			(n?HAL_GPIO_WritePin(TrigINT_GPIO_Port,TrigINT_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(TrigINT_GPIO_Port,TrigINT_Pin,GPIO_PIN_RESET))

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif
