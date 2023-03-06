#ifndef _MSP_SPI_H_
#define _MSP_SPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "MSP_GPIO.h"
//#include "../../include/spi.h"



/* USER CODE BEGIN Private defines */
#define SPI1_MODE SPI_MODE_MASTER         //设置SPI1为主模式
#define SPI2_MODE SPI_MODE_SLAVE          //设置SPI2为从模式
/* USER CODE END Private defines */

enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

/* transfer state */
__IO uint32_t wTransferState;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);

uint8_t SPI1_WriteData(uint8_t *data,uint16_t size);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);


#ifdef __cplusplus
}
#endif

#endif
