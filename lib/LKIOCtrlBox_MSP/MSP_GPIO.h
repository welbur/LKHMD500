#ifndef _MSP_GPIO_H_
#define	_MSP_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

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

/**
 * @brief 定义第一块DI Board的触发信号 
 * @brief DI Board --> Master Board
 */
#define DIB_INT_PIN1                         GPIO_PIN_3
#define DIB_INT_PIN1_PORT                    GPIOC
#define DIB_INT_PIN1_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DIB_INT_PIN1_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DIB_INT_PIN1_EXTI_IRQn               EXTI3_IRQn          
#define DIB_INT_PIN1_EXTI_SP                 1          
/**
 * @brief 定义第二块DI Board的触发信号 
 * @brief DI Board --> Master Board
 */
#define DIB_INT_PIN2                         GPIO_PIN_4
#define DIB_INT_PIN2_PORT                    GPIOC
#define DIB_INT_PIN2_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DIB_INT_PIN2_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DIB_INT_PIN2_EXTI_IRQn               EXTI4_IRQn         
#define DIB_INT_PIN2_EXTI_SP                 2            
/**
 * @brief 定义第3块DI Board的触发信号 
 * @brief DI Board --> Master Board
 */
#define DIB_INT_PIN3                         GPIO_PIN_5
#define DIB_INT_PIN3_PORT                    GPIOC
#define DIB_INT_PIN3_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DIB_INT_PIN3_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DIB_INT_PIN3_EXTI_IRQn               EXTI9_5_IRQn          
#define DIB_INT_PIN3_EXTI_SP                 3            
/**
 * @brief 定义第4块DI Board的触发信号 
 * @brief DI Board --> Master Board
 */
#define DIB_INT_PIN4                         GPIO_PIN_6
#define DIB_INT_PIN4_PORT                    GPIOC
#define DIB_INT_PIN4_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DIB_INT_PIN4_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DIB_INT_PIN4_EXTI_IRQn               EXTI9_5_IRQn         
#define DIB_INT_PIN4_EXTI_SP                 4            
/**
 * @brief 定义第1块DQ Board的触发信号 
 * @brief DQ Board --> Master Board
 */
#define DQB_INT_PIN1                         GPIO_PIN_7
#define DQB_INT_PIN1_PORT                    GPIOC
#define DQB_INT_PIN1_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DQB_INT_PIN1_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DQB_INT_PIN1_EXTI_IRQn               EXTI9_5_IRQn          
#define DQB_INT_PIN1_EXTI_SP                 5            
/**
 * @brief 定义第2块DQ Board的触发信号 
 * @brief DQ Board --> Master Board
 */
#define DQB_INT_PIN2                         GPIO_PIN_8
#define DQB_INT_PIN2_PORT                    GPIOC
#define DQB_INT_PIN2_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define DQB_INT_PIN2_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define DQB_INT_PIN2_EXTI_IRQn               EXTI9_5_IRQn          
#define DQB_INT_PIN2_EXTI_SP                 6              
/**
 * @brief 定义RS485 Board的触发信号 
 * @brief RS485 Board --> Master Board
 */
#define RS485B_INT_PIN                         GPIO_PIN_9
#define RS485B_INT_PIN_PORT                    GPIOC
#define RS485B_INT_PIN_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define RS485B_INT_PIN_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define RS485B_INT_PIN_EXTI_IRQn               EXTI9_5_IRQn    
#define RS485B_INT_PIN_EXTI_SP                 0
/**
 * @brief 定义Menu Board的触发信号 
 * @brief MENU Board --> Master Board
 */
#define MENUB_INT_PIN                         GPIO_PIN_10
#define MENUB_INT_PIN_PORT                    GPIOC
#define MENUB_INT_PIN_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define MENUB_INT_PIN_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define MENUB_INT_PIN_EXTI_IRQn               EXTI15_10_IRQn    //未定义
#define MENUB_INT_PIN_EXTI_SP                 7

#define GPIO_EXTI_PP              2       //定义GPIO中断的主优先级


#define UsedGPIOn               3       //定义使用的gpio数量
#define USEDGPIOx_CLK_ENABLE(__INDEX__)     do{if((__INDEX__) == 0) KEY_Pin_CLK_ENABLE(); else \
                                               if((__INDEX__) == 1) DIB_INT_PIN1_CLK_ENABLE(); else \
                                               if((__INDEX__) == 2) DIB_INT_PIN2_CLK_ENABLE(); \
                                                 }while(0)
#define USEDGPIOx_CLK_DISABLE(__INDEX__)    do{if((__INDEX__) == 0) KEY_Pin_CLK_DISABLE(); else \
                                               if((__INDEX__) == 1) DIB_INT_PIN1_CLK_DISABLE(); else \
                                               if((__INDEX__) == 2) DIB_INT_PIN2_CLK_DISABLE(); \
                                                 }while(0)


typedef enum 
{  
  DEV_BUTTON = 0,
  DIBoard_INT_IN1 = 1,
  DIBoard_INT_IN2 = 2,
}UsedGPIO_TypeDef;

typedef enum 
{  
  GPIO_MODE = 0,
  EXTI_MODE = 1
}UsedGPIOMode_TypeDef;

typedef enum 
{  
  WorkNormal = 0,
  WorkError = 1,
}LedStatus_TypeDef;

typedef struct
{
  LedStatus_TypeDef LedStatus;      
  uint8_t           LedBlinkn;
  uint32_t          LedBlinkFreq;
}LedPara_TypeDef;

//static void EXTILine0_Config(void);
void MX_GPIO_Init(void);
void BSP_PB_Init(UsedGPIO_TypeDef UsedGPIO, UsedGPIOMode_TypeDef UsedGPIOMode);
void BSP_PB_DeInit(UsedGPIO_TypeDef UsedGPIO);
uint32_t BSP_PB_GetState(UsedGPIO_TypeDef UsedGPIO);
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void WorkLed(void);
void Set_WorkLed_Status(LedStatus_TypeDef ls, uint32_t bf);
void EXTILine_Config(void);

#ifdef __cplusplus
}
#endif

#endif
