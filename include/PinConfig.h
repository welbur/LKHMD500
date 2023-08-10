#ifndef _PINCONFIG_H_
#define	_PINCONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "PowerBoardConfig.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include <string.h>


#ifdef DEVBoard                       //开发板上的按键和led引脚定义
#define KEY_Pin                         GPIO_PIN_13
#define KEY_GPIO_Port                   GPIOC
#define KEY_Pin_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY_Pin_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define KEY_Pin_EXTI_IRQn               EXTI0_IRQn

#define RED_Pin                         GPIO_PIN_0
#define RED_GPIO_Port                   GPIOC
#define GREEN_Pin                       GPIO_PIN_1
#define GREEN_GPIO_Port                 GPIOC
#define BLUE_Pin                        GPIO_PIN_2
#define BLUE_GPIO_Port                  GPIOC

#define WorkLed_Pin                     BLUE_Pin
#define WorkLed_GPIO_Port               BLUE_GPIO_Port
#elif defined(DEVBoardYD)             //如果使用源地开发板
#define KEY_Pin                         GPIO_PIN_3
#define KEY_GPIO_Port                   GPIOB
#define KEY_Pin_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define KEY_Pin_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#define KEY_Pin_EXTI_IRQn               EXTI3_IRQn

#define RED_Pin                         GPIO_PIN_13
#define RED_GPIO_Port                   GPIOA
#define GREEN_Pin                       GPIO_PIN_14
#define GREEN_GPIO_Port                 GPIOA
#define BLUE_Pin                        GPIO_PIN_15
#define BLUE_GPIO_Port                  GPIOA

#define WorkLed_Pin                     BLUE_Pin
#define WorkLed_GPIO_Port               BLUE_GPIO_Port
#else                             //实际使用的板子
#define WorkLed_Pin                     GPIO_PIN_0
#define WorkLed_GPIO_Port               GPIOC
#endif


/*********************************************************************************
 * @brief 通过tim输出pwm信号，或者是相位角控制信号
 *  TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    PA9     ------> TIM1_CH2
    PA10     ------> TIM1_CH3
    PA11     ------> TIM1_CH4
 *  TIM3 GPIO Configuration
    PC6     ------> TIM3_CH1
    PC7     ------> TIM3_CH2
    PC8     ------> TIM3_CH3
    PC9     ------> TIM3_CH4
 *  TIM12 GPIO Configuration
    PB15     ------> TIM12_CH2
 */
#if defined (PowerBoard_ACS37800) || defined (PowerBoard_MLX91220) 
/**TIM3 GPIO Configuration
    PC6     ------> TIM3_CH1
    PC7     ------> TIM3_CH2
    PC8     ------> TIM3_CH3
    PC9     ------> TIM3_CH4
    */
#define TIM3_CH1_Pin                          GPIO_PIN_6
#define TIM3_CH2_Pin                          GPIO_PIN_7
#define TIM3_CH3_Pin                          GPIO_PIN_8
#define TIM3_CH4_Pin                          GPIO_PIN_9
#define TIM3_GPIO_Port                        GPIOC
#define TIM3_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOC_CLK_ENABLE()
#define TIM3_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOC_CLK_DISABLE()

/**TIM4 GPIO Configuration
    PB6     ------> TIM4_CH1
    PB7     ------> TIM4_CH2
    PB8     ------> TIM4_CH3
    PB9     ------> TIM4_CH4
    */
#define TIM4_CH1_Pin                          GPIO_PIN_6
#define TIM4_CH2_Pin                          GPIO_PIN_7
#define TIM4_CH3_Pin                          GPIO_PIN_8
#define TIM4_CH4_Pin                          GPIO_PIN_9
#define TIM4_GPIO_Port                        GPIOB
#define TIM4_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()
#define TIM4_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOB_CLK_DISABLE()

/**TIM12 GPIO Configuration
    PB15     ------> TIM12_CH2
    */
#define TIM12_CH2_Pin                          GPIO_PIN_15
#define TIM12_GPIO_Port                        GPIOB
#define TIM12_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()
#define TIM12_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOB_CLK_DISABLE()
#endif



/*********************************************************************************
 * @brief 过零检测信号输入
 * PA8      ------> ZRC_IN1
 * PA9      ------> ZRC_IN2
 * PA10     ------> ZRC_IN3
 * PA11     ------> ZRC_IN4
 * PA12     ------> ZRC_IN5
 * PA13     ------> ZRC_IN6
 * PA14     ------> ZRC_IN7
 * PA15     ------> ZRC_IN8
 * PD2      ------> ZRC_IN9
 */
#if defined (PowerBoard_ACS37800) || defined (PowerBoard_MLX91220) 
#define ZRC_IN1_Pin                           GPIO_PIN_8
#define ZRC_IN1_Port                          GPIOA
#define ZRC_IN1_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN1_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN2_Pin                           GPIO_PIN_9
#define ZRC_IN2_Port                          GPIOA
#define ZRC_IN2_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN2_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN3_Pin                           GPIO_PIN_10
#define ZRC_IN3_Port                          GPIOA
#define ZRC_IN3_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN3_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN4_Pin                           GPIO_PIN_11
#define ZRC_IN4_Port                          GPIOA
#define ZRC_IN4_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN4_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN5_Pin                           GPIO_PIN_12
#define ZRC_IN5_Port                          GPIOA
#define ZRC_IN5_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN5_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#ifndef DEVBoardYD
#define ZRC_IN6_Pin                           GPIO_PIN_13
#define ZRC_IN6_Port                          GPIOA
#define ZRC_IN6_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN6_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN7_Pin                           GPIO_PIN_14
#define ZRC_IN7_Port                          GPIOA
#define ZRC_IN7_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN7_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define ZRC_IN8_Pin                           GPIO_PIN_15
#define ZRC_IN8_Port                          GPIOA
#define ZRC_IN8_Pin_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define ZRC_IN8_Pin_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()
#endif

#define ZRC_IN9_Pin                           GPIO_PIN_2
#define ZRC_IN9_Port                          GPIOD
#define ZRC_IN9_Pin_CLK_ENABLE()              __HAL_RCC_GPIOD_CLK_ENABLE()
#define ZRC_IN9_Pin_CLK_DISABLE()             __HAL_RCC_GPIOD_CLK_DISABLE()

#endif



/* 
 * cs信号 或者是adc输入信号
*/
#if defined(PowerBoard_ACS37800)
    /**CS GPIO Configuration
    PA0     ------> CS1
    PA1     ------> CS2
    PA2     ------> CS3
    PA3     ------> CS4
    PA4     ------> CS5
    PA5     ------> CS6
    PA6     ------> CS7
    PA7     ------> CS8
    PC4     ------> CS9
    */
/**
 * @brief 定义第一颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 1
 */
#define CHIP_SPI1_CS1                         GPIO_PIN_0
#define CHIP_SPI1_CS1_Port                    GPIOA
#define CHIP_SPI1_CS1_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS1_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第二颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 2
 */
#define CHIP_SPI1_CS2                         GPIO_PIN_1
#define CHIP_SPI1_CS2_Port                    GPIOA
#define CHIP_SPI1_CS2_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS2_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第三颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 3
 */
#define CHIP_SPI1_CS3                         GPIO_PIN_2
#define CHIP_SPI1_CS3_Port                    GPIOA
#define CHIP_SPI1_CS3_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS3_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第四颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 4
 */
#define CHIP_SPI1_CS4                         GPIO_PIN_3
#define CHIP_SPI1_CS4_Port                    GPIOA
#define CHIP_SPI1_CS4_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS4_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第五颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 5
 */
#define CHIP_SPI1_CS5                         GPIO_PIN_4
#define CHIP_SPI1_CS5_Port                    GPIOA
#define CHIP_SPI1_CS5_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS5_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第六颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 6
 */
#define CHIP_SPI1_CS6                         GPIO_PIN_5
#define CHIP_SPI1_CS6_Port                    GPIOA
#define CHIP_SPI1_CS6_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS6_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第七颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 7
 */
#define CHIP_SPI1_CS7                         GPIO_PIN_6
#define CHIP_SPI1_CS7_Port                    GPIOA
#define CHIP_SPI1_CS7_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS7_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第八颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 8
 */
#define CHIP_SPI1_CS8                         GPIO_PIN_7
#define CHIP_SPI1_CS8_Port                    GPIOA
#define CHIP_SPI1_CS8_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define CHIP_SPI1_CS8_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
/**
 * @brief 定义第九颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 9
 */
#define CHIP_SPI1_CS9                         GPIO_PIN_4
#define CHIP_SPI1_CS9_Port                    GPIOC
#define CHIP_SPI1_CS9_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define CHIP_SPI1_CS9_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()

#elif defined(PowerBoard_MLX91220)
    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PC4     ------> ADC1_IN14
    */
/**  @brief  ADC1_IN0 ***/
#define ADC1_IN0_Pin                          GPIO_PIN_0  
/*** @brief  ADC1_IN1 ***/
#define ADC1_IN1_Pin                          GPIO_PIN_1  
/*** @brief  ADC1_IN2 ***/
#define ADC1_IN2_Pin                          GPIO_PIN_2
/*** @brief  ADC1_IN3 ***/
#define ADC1_IN3_Pin                          GPIO_PIN_3
/*** @brief  ADC1_IN4 ***/
#define ADC1_IN4_Pin                          GPIO_PIN_4
/*** @brief  ADC1_IN5 ***/
#define ADC1_IN5_Pin                          GPIO_PIN_5
/*** @brief  ADC1_IN6 ***/
#define ADC1_IN6_Pin                          GPIO_PIN_6
/*** @brief  ADC1_IN7 ***/
#define ADC1_IN7_Pin                          GPIO_PIN_7
#define ADC1_IN0_IN7_Port                     GPIOA
#define ADC1_IN0_IN7_CLK_ENABLE()             __HAL_RCC_GPIOA_CLK_ENABLE()
#define ADC1_IN0_IN7_CLK_DISABLE()            __HAL_RCC_GPIOA_CLK_DISABLE()
                          
/*** @brief  ADC1_IN14 ***/
#define ADC1_IN14_Pin                         GPIO_PIN_4
#define ADC1_IN14_Port                        GPIOC
#define ADC1_IN14_CLK_ENABLE()                __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC1_IN14_CLK_DISABLE()               __HAL_RCC_GPIOC_CLK_DISABLE()
#endif

/*spi1用于读取计量芯片acs37800*/
/**
 * @brief SPI1 的GPIO 引脚 
 *	PB3     ------> SPI1_SCK
 *	PB4     ------> SPI1_MISO
 *	PB5     ------> SPI1_MOSI
 */
#if defined(PowerBoard_ACS37800)
#define SPI1_SCK_PIN                     GPIO_PIN_3
#define SPI1_MISO_PIN                    GPIO_PIN_4
#define SPI1_MOSI_PIN                    GPIO_PIN_5
#define SPI1_GPIO_PORT                   GPIOB
#define SPI1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#endif


/**
 * @brief usart3 的GPIO 引脚 
 * PC10     ------> USART3_TX
 * PC11     ------> USART3_RX
 */
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_GPIO_PORT                 GPIOC
#define USART3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART3_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()


/**
 * @brief can2 的GPIO 引脚 
 * PB12     ------> CAN2_RX
 * PB13     ------> CAN2_TX
 */
#define CAN2_RX_PIN                     GPIO_PIN_12
#define CAN2_TX_PIN                     GPIO_PIN_13              
#define CAN2_GPIO_PORT                  GPIOB 
#define CAN2_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define CAN2_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()


#ifdef __cplusplus
}
#endif

#endif  /* PINCONFIG_H_*/
