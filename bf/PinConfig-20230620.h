#ifndef _PINCONFIG_H_
#define	_PINCONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
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

/*spi1用于读取计量芯片acs37800 或者 电流传感器mlx91220的寄存器*/
//#if defined(PowerBoard_ACS37800) || defined(PowerBoard_MLX91220)
/**
 * @brief 定义第一颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 1
 */
#define CHIP_SPI1_CS1                         GPIO_PIN_0
#define CHIP_SPI1_CS1_Port                    GPIOB
#define CHIP_SPI1_CS1_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS1_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第二颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 2
 */
#define CHIP_SPI1_CS2                         GPIO_PIN_1
#define CHIP_SPI1_CS2_Port                    GPIOB
#define CHIP_SPI1_CS2_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS2_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第三颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 3
 */
#define CHIP_SPI1_CS3                         GPIO_PIN_2
#define CHIP_SPI1_CS3_Port                    GPIOB
#define CHIP_SPI1_CS3_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS3_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第四颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 4
 */
#define CHIP_SPI1_CS4                         GPIO_PIN_3
#define CHIP_SPI1_CS4_Port                    GPIOB
#define CHIP_SPI1_CS4_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS4_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第五颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 5
 */
#define CHIP_SPI1_CS5                         GPIO_PIN_4
#define CHIP_SPI1_CS5_Port                    GPIOB
#define CHIP_SPI1_CS5_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS5_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第六颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 6
 */
#define CHIP_SPI1_CS6                         GPIO_PIN_5
#define CHIP_SPI1_CS6_Port                    GPIOB
#define CHIP_SPI1_CS6_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS6_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第七颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 7
 */
#define CHIP_SPI1_CS7                         GPIO_PIN_6
#define CHIP_SPI1_CS7_Port                    GPIOB
#define CHIP_SPI1_CS7_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS7_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @brief 定义第八颗IC的片选cs信号 
 * @brief Power Board  -->  CHIP 8
 */
#define CHIP_SPI1_CS8                         GPIO_PIN_7
#define CHIP_SPI1_CS8_Port                    GPIOB
#define CHIP_SPI1_CS8_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define CHIP_SPI1_CS8_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()

/**
 * @brief SPI1 的GPIO 引脚 
 *	PA5     ------> SPI1_SCK
 *	PA6     ------> SPI1_MISO
 *	PA7     ------> SPI1_MOSI
 */
#define SPI1_SCK_PIN                     GPIO_PIN_5
#define SPI1_MISO_PIN                    GPIO_PIN_6
#define SPI1_MOSI_PIN                    GPIO_PIN_7
#define SPI1_GPIO_PORT                   GPIOA
#define SPI1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPI1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

/**
 * @brief SPI2作为slave板与主控板的通信接口
 * @brief SPI2 的GPIO 引脚 
 *  PB10     ------> SPI2_CS
 *  PB13     ------> SPI2_SCK
 *	PB14     ------> SPI2_MISO
 *	PB15     ------> SPI2_MOSI
 */
#define SPI2_CS                         GPIO_PIN_10
#define SPI2_CS_Port                    GPIOB
#define SPI2_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
#define SPI2_CS_EXTI_IRQn               EXTI15_10_IRQn
#define SPI2_CS_EXTI_SP                 7

#define SPI2_SCK_PIN                     GPIO_PIN_13
#define SPI2_MISO_PIN                    GPIO_PIN_14
#define SPI2_MOSI_PIN                    GPIO_PIN_15
#define SPI2_GPIO_PORT                   GPIOB
#define SPI2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/**
 * @brief PowerBoard发送给BaseBoard的中断信号
 *	PB1     ------> INT
 */
#define PB2BB_INT_Pin GPIO_PIN_1
#define PB2BB_INT_GPIO_Port GPIOB
//#define PB2BB_INT_ToMasterB(n)			(n?HAL_GPIO_WritePin(PB2BB_INT_GPIO_Port,PB2BB_INT_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(PB2BB_INT_GPIO_Port,PB2BB_INT_Pin,GPIO_PIN_RESET))

/**
 * @brief usart1 的GPIO 引脚 ----预留
 * PA9      ------> USART1_TX
 * PA10     ------> USART1_RX
 */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_GPIO_PORT                 GPIOA
#define USART1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

/**
 * @brief usart2 的GPIO 引脚 -----预留
 * PA2     ------> USART2_TX
 * PA3     ------> USART2_RX
 */
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_GPIO_PORT                 GPIOA
#define USART2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

/**
 * @brief usart3 的GPIO 引脚 
 * PC10     ------> USART3_TX
 * PC11     ------> USART3_RX
 */
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_GPIO_PORT                 GPIOC
#define USART3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/**
 * @brief 用于设置boardID
 * PC3     ------> bID0
 * PC4     ------> bID1
 * bID1 bID0
 *   1    0     表示此板子的ID为 2
 */
#define bID0_Pin                        GPIO_PIN_3
#define bID1_Pin                        GPIO_PIN_4
#define bID_GPIO_Port                   GPIOC
#define bID_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
#define bID_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOC_CLK_DISABLE()

#ifdef __cplusplus
}
#endif

#endif  /* PINCONFIG_H_*/
