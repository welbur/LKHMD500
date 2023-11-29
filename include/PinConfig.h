/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : PinConfig.h
 * @brief          : Created: 2023-09-26
 *                   Author: WU555
 ******************************************************************************
 * @attention      : LKHMD500 MainBoard
 *                   2023-09-26  V0.0.1     初稿
 *                   2023-11-03  V0.0.2     PCB画完，重新更新引脚定义
 *                   2023-11-27  v0.0.3     配合PCB V0.2 修改响应的引脚定义
 *
 *       1、usart1用于与LAN9255通信用，采用私有协议
 *           PB6     ------> USART1_TX
 *           PB7     ------> USART1_RX
 *           BaudRate = 460800
 *           *  输出控制信号  用于复位LAN9255
 *              PB9  ------> D51_RSTn       用于复位LAN9255
 *
 *       2、usart3用于打印测试信息
 *           PB10    ------> USART3_TX
 *           PB11    ------> USART3_RX
 *           BaudRate = 460800
 *
 *       3、电源模块 1 的引脚信息
 *           *  ADC输入
 *              PC0     ------> ADC2_IN6    ------> DCM1_Curr   用于检测输出电流
 *              PC1     ------> ADC2_IN7    ------> DCM1_Volt   用于检测输出电压
 *           *  DAC输出
 *              PA4     ------> DAC1_OUT1   ------> DCM1_Ictrl  用于控制输出电流大小
 *           *  输出控制信号
 *              PC2                         ------> DCM1_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC10                        ------> DCM1_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PA7                         ------> DCM1_Pdet   用于检测是否有输入电源接入
 * 
 *       4、电源模块 2 的引脚信息
 *           *  ADC输入
 *              PA0     ------> ADC1_IN1    ------> DCM2_Volt   用于检测输出电压
 *              PA1     ------> ADC1_IN2    ------> DCM2_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA5     ------> DAC1_OUT2   ------> DCM2_Ictrl  用于控制输出电流大小       
 *           *  输出控制信号
 *              PA3                         ------> DCM2_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC11                        ------> DCM2_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PC4                         ------> DCM2_Pdet   用于检测是否有输入电源接入
 *
 *       5、电源模块 3 的控制信息
 *           *  ADC输入
 *              PB0     ------> ADC3_IN12   ------> DCM3_Curr   用于检测输出电流
 *              PB1     ------> ADC3_IN1    ------> DCM3_Volt   用于检测输出电压
 *           *  DAC输出
 *              PA6     ------> DAC1_OUT1   ------> DCM3_Ictrl  用于控制输出电流大小           
 *           *  输出控制信号
 *              PB2                         ------> DCM3_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC12                        ------> DCM3_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PC5                         ------> DCM3_Pdet    用于检测是否有输入电源接入
 * 
 *       6、电源模块 4 的控制信息
 *           *  ADC输入
 *              PB14    ------> ADC4_IN4    ------> DCM4_Volt   用于检测输出电压
 *              PB15    ------> ADC4_IN5    ------> DCM4_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA2     ------> OPAMP1_VOUT ------> DCM4_Ictrl  用于控制输出电流大小            
 *           *  输出控制信号
 *              PB13                        ------> DCM4_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC6                         ------> DCM4_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PB3                         ------> DCM4_Pdet    用于检测是否有输入电源接入
 * 
 *       7、电源模块 5 的控制信息
 *           *  ADC输入
 *              PA9     ------> ADC5_IN2    ------> DCM5_Volt   用于检测输出电压
 *              PA8     ------> ADC5_IN1    ------> DCM5_Curr   用于检测输出电流
 *           *  DAC输出
 *              PB12    ------> OPAMP4_VOUT ------> DCM5_Ictrl  用于控制输出电流大小    
 *           *  输出控制信号
 *              PC9                         ------> DCM5_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC8                         ------> DCM5_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PD2                         ------> DCM5_Pdet    用于检测是否有输入电源接入
 * 
 *       8、WorkLed
 *              PC7     ------> WorkLed
 * 
 *       9、开发板中的引脚定义
 *              PC8     ------> WorkLed
 *              PA0     ------> KEY_Pin
 * 
 *       10、USB引脚定义
 *              PA11    ------> USB_DM
 *              PA12    ------> USB_DP
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
 * @brief 1、 usart1用于与LAN9255通信用，采用私有协议
 *           PB6     ------> USART1_TX
 *           PB7     ------> USART1_RX
 *           BaudRate = 460800
 *           *  输出控制信号  用于复位LAN9255
 *              PB9  ------> D51_RSTn       用于复位LAN9255
 */
#ifdef LKHMD500MainB
#define USART1_TX_Pin                    GPIO_PIN_6
#define USART1_RX_Pin                    GPIO_PIN_7
#define USART1_GPIO_PORT                 GPIOB
#define USART1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART1_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
//D51_RSTn 用于复位LAN9255
#define D51_RSTn_Pin                     GPIO_PIN_9
#define D51_RSTn_Port                    GPIOB
#define D51_RSTn_Pin_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define D51_RSTn_Pin_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#endif

/*********************************************************************************
 * @brief 2、 usart3用于打印测试信息
 *              PB10    ------> USART3_TX
 *              PB11    ------> USART3_RX
 *              BaudRate = 460800
 */
#ifdef LKHMD500MainB
#define USART3_TX_Pin                    GPIO_PIN_10
#define USART3_RX_Pin                    GPIO_PIN_11
#define USART3_GPIO_PORT                 GPIOB
#define USART3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#endif

/*********************************************************************************
 * @brief 3、 电源模块 1 的引脚信息
 *           *  ADC输入
 *              PC0     ------> ADC2_IN6    ------> DCM1_Curr   用于检测输出电流
 *              PC1     ------> ADC2_IN7    ------> DCM1_Volt   用于检测输出电压
 *           *  DAC输出
 *              PA4     ------> DAC1_OUT1   ------> DCM1_Ictrl  用于控制输出电流大小
 *           *  输出控制信号
 *              PC2                         ------> DCM1_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC10                        ------> DCM1_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PA7                         ------> DCM1_Pdet   用于检测是否有输入电源接入
 */
//ADC引脚
#ifdef LKHMD500MainB

#define ADC2_IN6_Pin                      GPIO_PIN_0  
#define DCM1_Curr_Pin                     ADC2_IN6_Pin
#define DCM1_Curr_Port                    GPIOC
#define DCM1_Curr_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM1_Curr_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()

#define ADC2_IN7_Pin                      GPIO_PIN_1
#define DCM1_Volt_Pin                     ADC2_IN7_Pin
#define DCM1_Volt_Port                    GPIOC
#define DCM1_Volt_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM1_Volt_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()

//DAC引脚                          
#define DAC1_OUT1_Pin                     GPIO_PIN_4
#define DCM1_Ictrl_Pin                    DAC1_OUT1_Pin
#define DCM1_Ictrl_Port                   GPIOA
#define DCM1_Ictrl_Pin_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM1_Ictrl_Pin_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()

//输出控制信号
#define DCM1_EN_Pin                       GPIO_PIN_2
#define DCM1_EN_Port                      GPIOC
#define DCM1_EN_Pin_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM1_EN_Pin_CLK_DISABLE()         __HAL_RCC_GPIOC_CLK_DISABLE()

//输入信号
#define DCM1_Fault_Pin                    GPIO_PIN_10
#define DCM1_Fault_Port                   GPIOC
#define DCM1_Fault_Pin_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM1_Fault_Pin_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM1_Fault_Pin_EXTI_IRQn          EXTI15_10_IRQn

//输入信号
#define DCM1_Pdet_Pin                      GPIO_PIN_7
#define DCM1_Pdet_Port                     GPIOA
#define DCM1_Pdet_Pin_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM1_Pdet_Pin_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()
#define DCM1_Pdet_Pin_EXTI_IRQn            EXTI9_5_IRQn

#endif

/*********************************************************************************
 * @brief 4、 电源模块 2 的引脚信息
 *           *  ADC输入
 *              PA0     ------> ADC1_IN1    ------> DCM2_Volt   用于检测输出电压
 *              PA1     ------> ADC1_IN2    ------> DCM2_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA5     ------> DAC1_OUT2   ------> DCM2_Ictrl  用于控制输出电流大小       
 *           *  输出控制信号
 *              PA3                         ------> DCM2_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC11                        ------> DCM2_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PC4                         ------> DCM2_Pdet   用于检测是否有输入电源接入
 */
#ifdef LKHMD500MainB
//ADC引脚
#ifndef DEVBoardG4
#define ADC1_IN1_Pin                      GPIO_PIN_0  
#define DCM2_Volt_Pin                     ADC1_IN1_Pin
#define DCM2_Volt_Port                    GPIOA
#define DCM2_Volt_Pin_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM2_Volt_Pin_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()
#endif
#define ADC1_IN2_Pin                      GPIO_PIN_1
#define DCM2_Curr_Pin                     ADC1_IN2_Pin
#define DCM2_Curr_Port                    GPIOA
#define DCM2_Curr_Pin_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM2_Curr_Pin_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()

//DAC引脚                          
#define DAC1_OUT2_Pin                     GPIO_PIN_5
#define DCM2_Ictrl_Pin                    DAC1_OUT2_Pin
#define DCM2_Ictrl_Port                   GPIOA
#define DCM2_Ictrl_Pin_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM2_Ictrl_Pin_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()

//输出控制信号
#define DCM2_EN_Pin                       GPIO_PIN_3
#define DCM2_EN_Port                      GPIOA
#define DCM2_EN_Pin_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM2_EN_Pin_CLK_DISABLE()         __HAL_RCC_GPIOA_CLK_DISABLE()

//输入信号
#define DCM2_Fault_Pin                    GPIO_PIN_11
#define DCM2_Fault_Port                   GPIOC
#define DCM2_Fault_Pin_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM2_Fault_Pin_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM2_Fault_Pin_EXTI_IRQn          EXTI15_10_IRQn

//输入信号
#define DCM2_Pdet_Pin                     GPIO_PIN_4
#define DCM2_Pdet_Port                    GPIOC
#define DCM2_Pdet_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM2_Pdet_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM2_Pdet_Pin_EXTI_IRQn           EXTI4_IRQn
#endif

/*********************************************************************************
 * @brief 5、 电源模块 3 的引脚信息
 *           *  ADC输入
 *              PB0     ------> ADC3_IN12   ------> DCM3_Curr   用于检测输出电流
 *              PB1     ------> ADC3_IN1    ------> DCM3_Volt   用于检测输出电压
 *           *  DAC输出
 *              PA6     ------> DAC2_OUT1   ------> DCM3_Ictrl  用于控制输出电流大小           
 *           *  输出控制信号
 *              PB2                         ------> DCM3_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC12                        ------> DCM3_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PC5                         ------> DCM3_Pdet    用于检测是否有输入电源接入
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC3_IN12_Pin                     GPIO_PIN_0  
#define DCM3_Curr_Pin                     ADC3_IN12_Pin
#define DCM3_Curr_Port                    GPIOB
#define DCM3_Curr_Pin_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM3_Curr_Pin_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()

#define ADC3_IN1_Pin                      GPIO_PIN_1
#define DCM3_Volt_Pin                     ADC3_IN1_Pin
#define DCM3_Volt_Port                    GPIOB
#define DCM3_Volt_Pin_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM3_Volt_Pin_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()

//DAC引脚                          
#define DAC2_OUT1_Pin                     GPIO_PIN_6
#define DCM3_Ictrl_Pin                    DAC2_OUT1_Pin
#define DCM3_Ictrl_Port                   GPIOA
#define DCM3_Ictrl_Pin_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM3_Ictrl_Pin_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()

//输出控制信号
#define DCM3_EN_Pin                       GPIO_PIN_2
#define DCM3_EN_Port                      GPIOB
#define DCM3_EN_Pin_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM3_EN_Pin_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()

//输入信号
#define DCM3_Fault_Pin                    GPIO_PIN_12
#define DCM3_Fault_Port                   GPIOC
#define DCM3_Fault_Pin_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM3_Fault_Pin_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM3_Fault_Pin_EXTI_IRQn          EXTI15_10_IRQn    //EXTI5_IRQn

//输入信号
#define DCM3_Pdet_Pin                     GPIO_PIN_5
#define DCM3_Pdet_Port                    GPIOC
#define DCM3_Pdet_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM3_Pdet_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM3_Pdet_Pin_EXTI_IRQn           EXTI9_5_IRQn
#endif

/*********************************************************************************
 * @brief 6、 电源模块 4 的引脚信息
 *           *  ADC输入
 *              PB14    ------> ADC4_IN4    ------> DCM4_Volt   用于检测输出电压
 *              PB15    ------> ADC4_IN5    ------> DCM4_Curr   用于检测输出电流
 *           *  DAC输出
 *              PA2     ------> OPAMP1_VOUT(DAC3_OUT1) ------> DCM4_Ictrl  用于控制输出电流大小            
 *           *  输出控制信号
 *              PB13                        ------> DCM4_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC6                         ------> DCM4_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PB3                         ------> DCM4_Pdet    用于检测是否有输入电源接入
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC4_IN4_Pin                      GPIO_PIN_14  
#define DCM4_Volt_Pin                     ADC4_IN4_Pin
#define DCM4_Volt_Port                    GPIOB
#define DCM4_Volt_Pin_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM4_Volt_Pin_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()

#define ADC4_IN5_Pin                      GPIO_PIN_15
#define DCM4_Curr_Pin                     ADC4_IN5_Pin
#define DCM4_Curr_Port                    GPIOB
#define DCM4_Curr_Pin_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM4_Curr_Pin_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()

//DAC引脚                          
#define OPAMP1_OUT_Pin                    GPIO_PIN_2
#define DCM4_Ictrl_Pin                    OPAMP1_OUT_Pin
#define DCM4_Ictrl_Port                   GPIOA
#define DCM4_Ictrl_Pin_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM4_Ictrl_Pin_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()

//输出控制信号
#define DCM4_EN_Pin                       GPIO_PIN_13
#define DCM4_EN_Port                      GPIOB
#define DCM4_EN_Pin_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM4_EN_Pin_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()

//输入信号
#define DCM4_Fault_Pin                    GPIO_PIN_6
#define DCM4_Fault_Port                   GPIOC
#define DCM4_Fault_Pin_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM4_Fault_Pin_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM4_Fault_Pin_EXTI_IRQn          EXTI9_5_IRQn

//输入信号
#define DCM4_Pdet_Pin                     GPIO_PIN_3
#define DCM4_Pdet_Port                    GPIOB
#define DCM4_Pdet_Pin_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM4_Pdet_Pin_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()
#define DCM4_Pdet_Pin_EXTI_IRQn           EXTI3_IRQn
#endif

/*********************************************************************************
 * @brief 7、 电源模块 5 的引脚信息
 *           *  ADC输入
 *              PA8     ------> ADC5_IN1    ------> DCM5_Curr   用于检测输出电流
 *              PA9     ------> ADC5_IN2    ------> DCM5_Volt   用于检测输出电压
 *           *  DAC输出
 *              PB12    ------> OPAMP4_VOUT(DAC4_OUT1) ------> DCM5_Ictrl  用于控制输出电流大小    
 *           *  输出控制信号
 *              PC9                         ------> DCM5_EN     电源模块的使能信号        
 *           *  输入信号
 *              PC8                         ------> DCM5_Fault  电源模块的错误指示信号
 *           *  输入信号
 *              PD2                         ------> DCM5_Pdet    用于检测是否有输入电源接入
 */
#ifdef LKHMD500MainB
//ADC引脚
#define ADC5_IN2_Pin                      GPIO_PIN_9  
#define DCM5_Volt_Pin                     ADC5_IN2_Pin
#define DCM5_Volt_Port                    GPIOA
#define DCM5_Volt_Pin_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM5_Volt_Pin_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()

#define ADC5_IN1_Pin                      GPIO_PIN_8
#define DCM5_Curr_Pin                     ADC5_IN1_Pin
#define DCM5_Curr_Port                    GPIOA
#define DCM5_Curr_Pin_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DCM5_Curr_Pin_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()

//DAC引脚                          
#define OPAMP4_VOUT_Pin                   GPIO_PIN_12
#define DCM5_Ictrl_Pin                    OPAMP4_VOUT_Pin
#define DCM5_Ictrl_Port                   GPIOB
#define DCM5_Ictrl_Pin_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define DCM5_Ictrl_Pin_CLK_DISABLE()      __HAL_RCC_GPIOB_CLK_DISABLE()

//输出控制信号
#define DCM5_EN_Pin                       GPIO_PIN_9
#define DCM5_EN_Port                      GPIOC
#define DCM5_EN_Pin_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM5_EN_Pin_CLK_DISABLE()         __HAL_RCC_GPIOC_CLK_DISABLE()

//输入信号
#ifndef DEVBoardG4
#define DCM5_Fault_Pin                    GPIO_PIN_8
#define DCM5_Fault_Port                   GPIOC
#define DCM5_Fault_Pin_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DCM5_Fault_Pin_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DCM5_Fault_Pin_EXTI_IRQn          EXTI9_5_IRQn
#endif

//输入信号
#define DCM5_Pdet_Pin                     GPIO_PIN_2
#define DCM5_Pdet_Port                    GPIOD
#define DCM5_Pdet_Pin_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()
#define DCM5_Pdet_Pin_CLK_DISABLE()       __HAL_RCC_GPIOD_CLK_DISABLE()
#define DCM5_Pdet_Pin_EXTI_IRQn           EXTI2_IRQn
#endif

/*********************************************************************************
 * @brief 8、 按键和led引脚定义
 *          实际板子WorkLed
 *              PC7     ------> WorkLed
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
#define WorkLed_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define WorkLed_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#else                             //实际使用的板子
/* 工作指示灯 */
#define WorkLed_Pin                     GPIO_PIN_7
#define WorkLed_GPIO_Port               GPIOC
#define WorkLed_Pin_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define WorkLed_Pin_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()

#endif



#ifdef __cplusplus
}
#endif


#endif  /* PINCONFIG_H_*/
