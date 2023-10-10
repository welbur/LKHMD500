/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : PinConfig.h
 * @brief          : Created: 2023-09-26
 *                   Author: WU555
 ******************************************************************************
 * @attention      : LKHMD500 MainBoard
 *                   2023-09-26  V0.0.1
 *
 *       1、usart1用于与LAN9255通信用，采用私有协议，采用DMA
 *           PC4     ------> USART1_TX
 *           PC5     ------> USART1_RX
 *           BaudRate = 460800
 *
 *       2、usart3用于打印测试信息
 *           PB10    ------> USART3_TX
 *           PB11    ------> USART3_RX
 *           BaudRate = 921600
 *
 *       3、电源模块 1 的引脚信息
 *           *  ADC输入
 *              PA0     ------> ADC1_IN1    ------> PowerM_1_Volt   用于检测输出电压
 *              PA1     ------> ADC1_IN2    ------> PowerM_1_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA2     ------> DAC3_OUT1   ------> PowerM_1_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC6     ------> TIM3_CH1    ------> PowerM_1_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA13                        ------> PowerM_1_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB3                         ------> PowerM_1_Fault  电源模块的错误指示信号
 * 
 *       4、电源模块 2 的引脚信息
 *           *  ADC输入
 *              PC0     ------> ADC2_IN6    ------> PowerM_2_Volt   用于检测输出电压
 *              PC1     ------> ADC2_IN7    ------> PowerM_2_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA4     ------> DAC1_OUT1   ------> PowerM_2_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC7     ------> TIM3_CH2    ------> PowerM_2_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA14                        ------> PowerM_2_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB4                         ------> PowerM_2_Fault  电源模块的错误指示信号
 *
 *       5、电源模块 3 的控制信息
 *           *  ADC输入
 *              PB0     ------> ADC3_IN12   ------> PowerM_3_Volt   用于检测输出电压
 *              PB1     ------> ADC3_IN1    ------> PowerM_3_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA5     ------> DAC1_OUT2   ------> PowerM_3_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC8     ------> TIM3_CH3    ------> PowerM_3_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA15                        ------> PowerM_3_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB5                         ------> PowerM_3_Fault  电源模块的错误指示信号
 * 
 *       6、电源模块 4 的控制信息
 *           *  ADC输入
 *              PB14    ------> ADC4_IN4    ------> PowerM_4_Volt   用于检测输出电压
 *              PB15    ------> ADC4_IN5    ------> PowerM_4_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA6     ------> DAC2_OUT1   ------> PowerM_4_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC9     ------> TIM3_CH4    ------> PowerM_4_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA16                        ------> PowerM_4_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB6                         ------> PowerM_4_Fault  电源模块的错误指示信号
 * 
 *       7、电源模块 5 的控制信息
 *           *  ADC输入
 *              PA8     ------> ADC5_IN1    ------> PowerM_5_Volt   用于检测输出电压
 *              PA9     ------> ADC5_IN2    ------> PowerM_5_Curr   用于检测输出电流
 *           *  DAC输出
 *              PB12    ------> DAC4_OUT1   ------> PowerM_5_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PA11    ------> TIM4_CH1    ------> PowerM_5_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA17                        ------> PowerM_5_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB7                         ------> PowerM_5_Fault  电源模块的错误指示信号
 * 
 *       8、WorkLed
 *              PD2     ------> WorkLed
 * 
 *       9、开发板中的引脚定义
 *              PC8     ------> WorkLed
 *              PA0     ------> KEY_Pin
 * 
 ******************************************************************************
 */

#ifndef _PINCONFIG_H_
#define	_PINCONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "BoardConfig.h"

#include <string.h>


/*********************************************************************************
 * @brief 1、 usart1用于与LAN9255通信用，采用私有协议，采用DMA
 *              PC4     ------> USART1_TX
 *              PC5     ------> USART1_RX
 *              BaudRate = 460800
 */
#ifdef LKHMD500MainB
#define USART1_TX_PIN                    GPIO_PIN_4
#define USART1_RX_PIN                    GPIO_PIN_5
#define USART1_GPIO_PORT                 GPIOC
#define USART1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART1_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#endif

/*********************************************************************************
 * @brief 2、 usart3用于打印测试信息
 *              PB10    ------> USART3_TX
 *              PB11    ------> USART3_RX
 *              BaudRate = 921600
 */
#ifdef LKHMD500MainB
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_GPIO_PORT                 GPIOB
#define USART3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#endif

/*********************************************************************************
 * @brief 3、 电源模块 1 的引脚信息
 *           *  ADC输入
 *              PA0     ------> ADC1_IN1    ------> PowerM_1_Volt   用于检测输出电压
 *              PA1     ------> ADC1_IN2    ------> PowerM_1_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA2     ------> DAC3_OUT1   ------> PowerM_1_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC6     ------> TIM3_CH1    ------> PowerM_1_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA13                        ------> PowerM_1_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB3                         ------> PowerM_1_Fault  电源模块的错误指示信号
 */
//ADC引脚
#ifdef LKHMD500MainB
#ifndef DEVBoardG4
#define ADC1_IN1_Pin                          GPIO_PIN_0  
#define PowerM_1_Volt                         ADC1_IN1_Pin
#define PowerM_1_Volt_Port                    GPIOA
#define PowerM_1_Volt_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_1_Volt_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
#endif
#define ADC1_IN2_Pin                          GPIO_PIN_1
#define PowerM_1_Curr                         ADC1_IN2_Pin
#define PowerM_1_Curr_Port                    GPIOA
#define PowerM_1_Curr_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_1_Curr_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()

//DAC引脚                          
#define DAC3_OUT1_Pin                         GPIO_PIN_2
#define PowerM_1_Ictrl                        DAC3_OUT1_Pin
#define PowerM_1_Ictrl_Port                   GPIOA
#define PowerM_1_Ictrl_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_1_Ictrl_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

//PWM输出引脚
#define TIM3_CH1_Pin                          GPIO_PIN_6
#define PowerM_1_PWM                          TIM3_CH1_Pin
#define PowerM_1_PWM_Port                     GPIOC
#define PowerM_1_PWM_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_1_PWM_CLK_DISABLE()            __HAL_RCC_GPIOC_CLK_DISABLE()

//输出控制信号
#define PowerM_1_EN                           GPIO_PIN_13
#define PowerM_1_EN_Port                      GPIOA
#define PowerM_1_EN_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_1_EN_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

//输入信号
#define PowerM_1_Fault                        GPIO_PIN_3
#define PowerM_1_Fault_Port                   GPIOB
#define PowerM_1_Fault_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_1_Fault_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define PowerM_1_Fault_EXTI_IRQn              EXTI3_IRQn
#endif

/*********************************************************************************
 * @brief 4、 电源模块 2 的引脚信息
 *           *  ADC输入
 *              PC0     ------> ADC2_IN6    ------> PowerM_2_Volt   用于检测输出电压
 *              PC1     ------> ADC2_IN7    ------> PowerM_2_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA4     ------> DAC1_OUT1   ------> PowerM_2_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC7     ------> TIM3_CH2    ------> PowerM_2_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA14                        ------> PowerM_2_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB4                         ------> PowerM_2_Fault  电源模块的错误指示信号
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC2_IN6_Pin                          GPIO_PIN_0  
#define PowerM_2_Volt                         ADC2_IN6_Pin
#define PowerM_2_Volt_Port                    GPIOC
#define PowerM_2_Volt_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_2_Volt_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define ADC2_IN7_Pin                          GPIO_PIN_1
#define PowerM_2_Curr                         ADC2_IN7_Pin
#define PowerM_2_Curr_Port                    GPIOC
#define PowerM_2_Curr_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_2_Curr_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()

//DAC引脚                          
#define DAC1_OUT1_Pin                         GPIO_PIN_4
#define PowerM_2_Ictrl                        DAC1_OUT1_Pin
#define PowerM_2_Ictrl_Port                   GPIOA
#define PowerM_2_Ictrl_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_2_Ictrl_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

//PWM输出引脚
#define TIM3_CH2_Pin                          GPIO_PIN_7
#define PowerM_2_PWM                          TIM3_CH2_Pin
#define PowerM_2_PWM_Port                     GPIOC
#define PowerM_2_PWM_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_2_PWM_CLK_DISABLE()            __HAL_RCC_GPIOC_CLK_DISABLE()

//输出控制信号
#define PowerM_2_EN                           GPIO_PIN_14
#define PowerM_2_EN_Port                      GPIOA
#define PowerM_2_EN_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_2_EN_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

//输入信号
#define PowerM_2_Fault                        GPIO_PIN_4
#define PowerM_2_Fault_Port                   GPIOB
#define PowerM_2_Fault_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_2_Fault_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define PowerM_2_Fault_EXTI_IRQn              EXTI4_IRQn
#endif

/*********************************************************************************
 * @brief 5、 电源模块 3 的引脚信息
 *           *  ADC输入
 *              PB0     ------> ADC3_IN12   ------> PowerM_3_Volt   用于检测输出电压
 *              PB1     ------> ADC3_IN1    ------> PowerM_3_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA5     ------> DAC1_OUT2   ------> PowerM_3_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC8     ------> TIM3_CH3    ------> PowerM_3_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA15                        ------> PowerM_3_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB5                         ------> PowerM_3_Fault  电源模块的错误指示信号
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC3_IN12_Pin                         GPIO_PIN_0  
#define PowerM_3_Volt                         ADC3_IN12_Pin
#define PowerM_3_Volt_Port                    GPIOB
#define PowerM_3_Volt_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_3_Volt_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#define ADC3_IN1_Pin                          GPIO_PIN_1
#define PowerM_3_Curr                         ADC3_IN1_Pin
#define PowerM_3_Curr_Port                    GPIOB
#define PowerM_3_Curr_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_3_Curr_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()

//DAC引脚                          
#define DAC1_OUT2_Pin                         GPIO_PIN_5
#define PowerM_3_Ictrl                        DAC1_OUT2_Pin
#define PowerM_3_Ictrl_Port                   GPIOA
#define PowerM_3_Ictrl_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_3_Ictrl_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

//PWM输出引脚
#ifndef DEVBoardG4
#define TIM3_CH3_Pin                          GPIO_PIN_8
#define PowerM_3_PWM                          TIM3_CH3_Pin
#define PowerM_3_PWM_Port                     GPIOC
#define PowerM_3_PWM_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_3_PWM_CLK_DISABLE()            __HAL_RCC_GPIOC_CLK_DISABLE()
#endif

//输出控制信号
#define PowerM_3_EN                           GPIO_PIN_15
#define PowerM_3_EN_Port                      GPIOA
#define PowerM_3_EN_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_3_EN_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

//输入信号
#define PowerM_3_Fault                        GPIO_PIN_5
#define PowerM_3_Fault_Port                   GPIOB
#define PowerM_3_Fault_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_3_Fault_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define PowerM_3_Fault_EXTI_IRQn              EXTI5_IRQn
#endif

/*********************************************************************************
 * @brief 6、 电源模块 4 的引脚信息
 *           *  ADC输入
 *              PB14    ------> ADC4_IN4    ------> PowerM_4_Volt   用于检测输出电压
 *              PB15    ------> ADC4_IN5    ------> PowerM_4_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA6     ------> DAC2_OUT1   ------> PowerM_4_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PC9     ------> TIM3_CH4    ------> PowerM_4_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA16                        ------> PowerM_4_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB6                         ------> PowerM_4_Fault  电源模块的错误指示信号
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC4_IN4_Pin                          GPIO_PIN_14  
#define PowerM_4_Volt                         ADC4_IN4_Pin
#define PowerM_4_Volt_Port                    GPIOB
#define PowerM_4_Volt_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_4_Volt_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#define ADC4_IN5_Pin                          GPIO_PIN_15
#define PowerM_4_Curr                         ADC4_IN5_Pin
#define PowerM_4_Curr_Port                    GPIOB
#define PowerM_4_Curr_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_4_Curr_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()

//DAC引脚                          
#define DAC2_OUT1_Pin                         GPIO_PIN_6
#define PowerM_4_Ictrl                        DAC2_OUT1_Pin
#define PowerM_4_Ictrl_Port                   GPIOA
#define PowerM_4_Ictrl_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_4_Ictrl_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

//PWM输出引脚
#define TIM3_CH4_Pin                          GPIO_PIN_9
#define PowerM_4_PWM                          TIM3_CH4_Pin
#define PowerM_4_PWM_Port                     GPIOC
#define PowerM_4_PWM_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_4_PWM_CLK_DISABLE()            __HAL_RCC_GPIOC_CLK_DISABLE()

//输出控制信号
#define PowerM_4_EN                           GPIO_PIN_10
#define PowerM_4_EN_Port                      GPIOC
#define PowerM_4_EN_CLK_ENABLE()              __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_4_EN_CLK_DISABLE()             __HAL_RCC_GPIOC_CLK_DISABLE()

//输入信号
#define PowerM_4_Fault                        GPIO_PIN_6
#define PowerM_4_Fault_Port                   GPIOB
#define PowerM_4_Fault_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_4_Fault_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define PowerM_4_Fault_EXTI_IRQn              EXTI6_IRQn
#endif

/*********************************************************************************
 * @brief 7、 电源模块 5 的引脚信息
 *           *  ADC输入
 *              PA8     ------> ADC5_IN1    ------> PowerM_5_Volt   用于检测输出电压
 *              PA9     ------> ADC5_IN2    ------> PowerM_5_Curr   用于检测输出电流
 *           *  DAC输出
 *              PB12    ------> DAC4_OUT1   ------> PowerM_5_Ictrl  用于控制输出电流大小
 *           *  PWM输出
 *              PA11    ------> TIM4_CH1    ------> PowerM_5_PWM    用以控制输出电流的脉冲
 *           *  输出控制信号
 *              PA17                        ------> PowerM_5_EN     电源模块的使能信号        
 *           *  输入信号
 *              PB7                         ------> PowerM_5_Fault  电源模块的错误指示信号
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC5_IN1_Pin                          GPIO_PIN_8  
#define PowerM_5_Volt                         ADC5_IN1_Pin
#define PowerM_5_Volt_Port                    GPIOA
#define PowerM_5_Volt_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_5_Volt_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
#define ADC5_IN2_Pin                          GPIO_PIN_9
#define PowerM_5_Curr                         ADC5_IN2_Pin
#define PowerM_5_Curr_Port                    GPIOA
#define PowerM_5_Curr_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_5_Curr_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()

//DAC引脚                          
#define DAC4_OUT1_Pin                         GPIO_PIN_12
#define PowerM_5_Ictrl                        DAC4_OUT1_Pin
#define PowerM_5_Ictrl_Port                   GPIOB
#define PowerM_5_Ictrl_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_5_Ictrl_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

//PWM输出引脚
#define TIM4_CH1_Pin                          GPIO_PIN_11
#define PowerM_5_PWM                          TIM4_CH1_Pin
#define PowerM_5_PWM_Port                     GPIOA
#define PowerM_5_PWM_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define PowerM_5_PWM_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()

//输出控制信号
#define PowerM_5_EN                           GPIO_PIN_11
#define PowerM_5_EN_Port                      GPIOC
#define PowerM_5_EN_CLK_ENABLE()              __HAL_RCC_GPIOC_CLK_ENABLE()
#define PowerM_5_EN_CLK_DISABLE()             __HAL_RCC_GPIOC_CLK_DISABLE()

//输入信号
#define PowerM_5_Fault                        GPIO_PIN_7
#define PowerM_5_Fault_Port                   GPIOB
#define PowerM_5_Fault_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define PowerM_5_Fault_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define PowerM_5_Fault_EXTI_IRQn              EXTI7_IRQn
#endif

/*********************************************************************************
 * @brief 8、 按键和led引脚定义
 *          实际板子WorkLed
 *              PD2     ------> WorkLed
 * 
 *          开发板中的引脚定义
 *              PC8     ------> WorkLed
 *              PA0     ------> KEY_Pin
 */
#ifdef DEVBoardG4                       //开发板上的按键和led引脚定义
#define KEY_Pin                         GPIO_PIN_0
#define KEY_GPIO_Port                   GPIOA
#define KEY_Pin_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY_Pin_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
#define KEY_Pin_EXTI_IRQn               EXTI0_IRQn

#define WorkLed_Pin                     GPIO_PIN_8
#define WorkLed_GPIO_Port               GPIOC
#else                             //实际使用的板子
/* 工作指示灯 */
#define WorkLed_Pin                     GPIO_PIN_2
#define WorkLed_GPIO_Port               GPIOD

#endif



#ifdef __cplusplus
}
#endif


#endif  /* PINCONFIG_H_*/
