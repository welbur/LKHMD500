#ifndef _MSP_SPI_H_
#define _MSP_SPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stdio.h"
//#include "SPITransfer_C.h"
#include "SlaveBoardConfig.h"
//#include "MSP_GPIO.h"
//#include "../../include/spi.h"


/* USER CODE BEGIN Private defines */
#define SPI1_MODE 						SPI_MODE_MASTER         //设置SPI1为主模式
#define SPI2_MODE 						SPI_MODE_SLAVE          //设置SPI2为从模式
#define SET_SPIMODE_MASTER				1
#define SET_SPIMODE_SLAVE				0

    /**SPI1 GPIO Configuration
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
/* USER CODE END Private defines */
#if defined(D_I_Board) || defined(D_Q_Board)
#define SPI1_Port                   GPIOA
#define SPI1_NSS					GPIO_PIN_4
#define SPI1_SCK					GPIO_PIN_5
#define SPI1_MISO					GPIO_PIN_6
#define SPI1_MOSI					GPIO_PIN_7

#endif

#define SPI2_Port                   GPIOB
//#define SPI2_NSS					GPIO_PIN_12
#define SPI2_SCK					GPIO_PIN_13
#define SPI2_MISO					GPIO_PIN_14
#define SPI2_MOSI					GPIO_PIN_15

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

uint8_t MSP_SPI_write(SPI_HandleTypeDef* spiHandle, uint8_t *txData, uint16_t txLen);
uint8_t MSP_SPI_read(SPI_HandleTypeDef* spiHandle, uint8_t *rxData, uint16_t rxLen);

uint8_t SPI1_WriteData(uint8_t *data,uint16_t size);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);


#ifdef __cplusplus
}
#endif

#endif
