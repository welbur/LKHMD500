#ifndef _MSP_SPI_H_
#define _MSP_SPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "SlaveBoardConfig.h"
//#include "MSP_GPIO.h"
//#include "../../include/spi.h"

//spi1 DI_Board 1 cs引脚
#define SPI1_DIB1_CS                         GPIO_PIN_0
#define SPI1_DIB1_CS_Port                    GPIOB
#define SPI1_DIB1_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DIB1_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 DI_Board 2 cs引脚
#define SPI1_DIB2_CS                         GPIO_PIN_1
#define SPI1_DIB2_CS_Port                    GPIOB
#define SPI1_DIB2_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DIB2_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 DI_Board 3 cs引脚
#define SPI1_DIB3_CS                         GPIO_PIN_2
#define SPI1_DIB3_CS_Port                    GPIOB
#define SPI1_DIB3_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DIB3_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 DI_Board 4 cs引脚
#define SPI1_DIB4_CS                         GPIO_PIN_3
#define SPI1_DIB4_CS_Port                    GPIOB
#define SPI1_DIB4_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DIB4_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 DQ_Board 1 cs引脚
#define SPI1_DQB1_CS                         GPIO_PIN_4
#define SPI1_DQB1_CS_Port                    GPIOB
#define SPI1_DQB1_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DQB1_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 DQ_Board 2 cs引脚
#define SPI1_DQB2_CS                         GPIO_PIN_5
#define SPI1_DQB2_CS_Port                    GPIOB
#define SPI1_DQB2_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DQB2_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 RS485_Board cs引脚
#define SPI1_RS485B_CS                       GPIO_PIN_6
#define SPI1_RS485B_CS_Port                  GPIOB
#define SPI1_RS485B_CS_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_RS485B_CS_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 MENU_board 1 cs引脚
#define SPI1_MENUB_CS                        GPIO_PIN_7
#define SPI1_MENUB_CS_Port                   GPIOB
#define SPI1_MENUB_CS_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_MENUB_CS_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

/* USER CODE BEGIN Private defines */
#define SPI1_MODE SPI_MODE_MASTER         //设置SPI1为主模式
#define SPI2_MODE SPI_MODE_SLAVE          //设置SPI2为从模式
#define SET_SPIMODE_MASTER				1
#define SET_SPIMODE_SLAVE				0

#define sTransBoard_Max                 8       //定义板子的数量 di板4块，dq板2块，rs485板1块，menu板1块
#define sTrans_TimeOut                  50       //5 = 5ms
#define sTxRx_TimeOut                   1
#define spiRxDataLen                    50
/* USER CODE END Private defines */




enum {
	SpiTxRx_WAIT,
	SpiTx_COMPLETE,
	SpiRx_COMPLETE,
	SpiTxRx_ERROR
};


extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle);

void SPITransfer_Init(void);

uint8_t MSP_SPI_write(SPI_HandleTypeDef* spiHandle, uint8_t cs, uint8_t *txData, uint16_t txLen);
uint8_t MSP_SPI_read(SPI_HandleTypeDef* spiHandle, uint8_t cs, uint8_t *rxData, uint16_t rxLen);

uint8_t SPI1_WriteData(uint8_t *data,uint16_t size);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);


#ifdef __cplusplus
}
#endif

#endif
