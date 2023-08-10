#ifndef _MSP_SPI_H_
#define _MSP_SPI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "PowerBoardConfig.h"
//#include "ErrorInfo.h"


/* USER CODE BEGIN Private defines */
#define SPI1_MODE SPI_MODE_MASTER         //设置SPI1为主模式
#define SPI2_MODE SPI_MODE_SLAVE          //设置SPI2为从模式
#define SET_SPIMODE_MASTER				1
#define SET_SPIMODE_SLAVE				0

#ifdef PowerBoard_ACS37800
#define SPI1_CS_ENABLE(__CHIPID__)       do{if((__CHIPID__) == CHIPID_1)    HAL_GPIO_WritePin(PowerB_SPI1_CS1_Port,PowerB_SPI1_CS1,GPIO_PIN_RESET); else \
                                        	if((__CHIPID__) == CHIPID_2)    HAL_GPIO_WritePin(PowerB_SPI1_CS2_Port,PowerB_SPI1_CS2,GPIO_PIN_RESET); else \
                                           	if((__CHIPID__) == CHIPID_3)    HAL_GPIO_WritePin(PowerB_SPI1_CS3_Port,PowerB_SPI1_CS3,GPIO_PIN_RESET); else \
                                           	if((__CHIPID__) == CHIPID_4)    HAL_GPIO_WritePin(PowerB_SPI1_CS4_Port,PowerB_SPI1_CS4,GPIO_PIN_RESET); else \
											if((__CHIPID__) == CHIPID_5)    HAL_GPIO_WritePin(PowerB_SPI1_CS5_Port,PowerB_SPI1_CS5,GPIO_PIN_RESET); else \
											if((__CHIPID__) == CHIPID_6)    HAL_GPIO_WritePin(PowerB_SPI1_CS6_Port,PowerB_SPI1_CS6,GPIO_PIN_RESET); else \
											if((__CHIPID__) == CHIPID_7)    HAL_GPIO_WritePin(PowerB_SPI1_CS7_Port,PowerB_SPI1_CS7,GPIO_PIN_RESET); else \
											if((__CHIPID__) == CHIPID_8)    HAL_GPIO_WritePin(PowerB_SPI1_CS8_Port,PowerB_SPI1_CS8,GPIO_PIN_RESET); else \
											if((__CHIPID__) == CHIPID_9)    HAL_GPIO_WritePin(PowerB_SPI1_CS9_Port,PowerB_SPI1_CS9,GPIO_PIN_RESET);  \
										}while(0)
#define SPI1_CS_DISABLE(__CHIPID__)      do{if((__CHIPID__) == CHIPID_1)    HAL_GPIO_WritePin(PowerB_SPI1_CS1_Port,PowerB_SPI1_CS1,GPIO_PIN_SET); else \
                                           	if((__CHIPID__) == CHIPID_2)    HAL_GPIO_WritePin(PowerB_SPI1_CS2_Port,PowerB_SPI1_CS2,GPIO_PIN_SET); else \
                                           	if((__CHIPID__) == CHIPID_3)    HAL_GPIO_WritePin(PowerB_SPI1_CS3_Port,PowerB_SPI1_CS3,GPIO_PIN_SET); else \
                                           	if((__CHIPID__) == CHIPID_4)    HAL_GPIO_WritePin(PowerB_SPI1_CS4_Port,PowerB_SPI1_CS4,GPIO_PIN_SET); else \
											if((__CHIPID__) == CHIPID_5)    HAL_GPIO_WritePin(PowerB_SPI1_CS5_Port,PowerB_SPI1_CS5,GPIO_PIN_SET); else \
											if((__CHIPID__) == CHIPID_6)    HAL_GPIO_WritePin(PowerB_SPI1_CS6_Port,PowerB_SPI1_CS6,GPIO_PIN_SET); else \
											if((__CHIPID__) == CHIPID_7)    HAL_GPIO_WritePin(PowerB_SPI1_CS7_Port,PowerB_SPI1_CS7,GPIO_PIN_SET); else \
											if((__CHIPID__) == CHIPID_8)    HAL_GPIO_WritePin(PowerB_SPI1_CS8_Port,PowerB_SPI1_CS8,GPIO_PIN_SET); else \
											if((__CHIPID__) == CHIPID_9)    HAL_GPIO_WritePin(PowerB_SPI1_CS9_Port,PowerB_SPI1_CS9,GPIO_PIN_SET);  \
                                        }while(0)
#endif

/*
enum {
	SpiTxRx_WAIT,
	SpiTx_COMPLETE,
	SpiRx_COMPLETE,
	SpiTxRx_ERROR
};*/


extern SPI_HandleTypeDef hspi1;

void MX_SPI1_Init(void);

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle);

uint8_t MSP_SPI_write(SPI_HandleTypeDef* spiHandle, uint8_t *txData, uint16_t txLen);
uint8_t MSP_SPI_read(SPI_HandleTypeDef* spiHandle, uint8_t *rxData, uint16_t rxLen);


#ifdef __cplusplus
}
#endif

#endif
