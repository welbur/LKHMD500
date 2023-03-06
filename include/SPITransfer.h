#ifndef _SPITRANSFER_H_
#define	_SPITRANSFER_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "Packet_C.h"                 //#include "Packet.h"
#include "stm32f4xx_hal.h"
#include "../LKIOCtrlBox_MSP/MSP_SPI.h"

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

#define sTransBoard_Max                 8       //定义板子的数量 di板4块，dq板2块，rs485板1块，menu板1块
#define sTrans_TimeOut                  20       //5 = 5ms

#define SPI_ACK_BYTES             0xA5A5
#define SPI_NACK_BYTES            0xDEAD
#define SPI_TIMEOUT_MAX           0x1000
#define SPI_SLAVE_SYNBYTE         0x53
#define SPI_MASTER_SYNBYTE        0xAC

/*
#define SPI1_CS_DIB1(n)			(n?HAL_GPIO_WritePin(SPI1_DIB1_CS_Port,SPI1_DIB1_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DIB1_CS_Port,SPI1_DIB1_CS,GPIO_PIN_RESET))
#define SPI1_CS_DIB2(n)			(n?HAL_GPIO_WritePin(SPI1_DIB2_CS_Port,SPI1_DIB2_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DIB2_CS_Port,SPI1_DIB2_CS,GPIO_PIN_RESET))
#define SPI1_CS_DIB3(n)			(n?HAL_GPIO_WritePin(SPI1_DIB3_CS_Port,SPI1_DIB3_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DIB3_CS_Port,SPI1_DIB3_CS,GPIO_PIN_RESET))               
#define SPI1_CS_DIB4(n)			(n?HAL_GPIO_WritePin(SPI1_DIB4_CS_Port,SPI1_DIB4_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DIB4_CS_Port,SPI1_DIB4_CS,GPIO_PIN_RESET))
#define SPI1_CS_DQB1(n)			(n?HAL_GPIO_WritePin(SPI1_DQB1_CS_Port,SPI1_DQB1_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DQB1_CS_Port,SPI1_DQB1_CS,GPIO_PIN_RESET))
#define SPI1_CS_DQB2(n)			(n?HAL_GPIO_WritePin(SPI1_DQB2_CS_Port,SPI1_DQB2_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_DQB2_CS_Port,SPI1_DQB2_CS,GPIO_PIN_RESET))
#define SPI1_CS_RS485B(n)		(n?HAL_GPIO_WritePin(SPI1_RS485B_CS_Port,SPI1_RS485B_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_RS485B_CS_Port,SPI1_RS485B_CS,GPIO_PIN_RESET))
#define SPI1_CS_MENUB(n)		(n?HAL_GPIO_WritePin(SPI1_MENUB_CS_Port,SPI1_MENUB_CS,GPIO_PIN_SET): \
                                   HAL_GPIO_WritePin(SPI1_MENUB_CS_Port,SPI1_MENUB_CS,GPIO_PIN_RESET))
*/
#define SPI1_CS_ENABLE(__INDEX__)       do{if((__INDEX__) == DI_Board_1)    HAL_GPIO_WritePin(SPI1_DIB1_CS_Port,SPI1_DIB1_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == DI_Board_2)    HAL_GPIO_WritePin(SPI1_DIB2_CS_Port,SPI1_DIB2_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == DI_Board_3)    HAL_GPIO_WritePin(SPI1_DIB3_CS_Port,SPI1_DIB3_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == DI_Board_4)    HAL_GPIO_WritePin(SPI1_DIB4_CS_Port,SPI1_DIB4_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == DQ_Board_1)    HAL_GPIO_WritePin(SPI1_DQB1_CS_Port,SPI1_DQB1_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == DQ_Board_2)    HAL_GPIO_WritePin(SPI1_DQB2_CS_Port,SPI1_DQB2_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == RS485_Board)   HAL_GPIO_WritePin(SPI1_RS485B_CS_Port,SPI1_RS485B_CS,GPIO_PIN_RESET); else \
                                           if((__INDEX__) == MENU_Board)    HAL_GPIO_WritePin(SPI1_MENUB_CS_Port,SPI1_MENUB_CS,GPIO_PIN_RESET); \
                                        }while(0)
#define SPI1_CS_DISABLE(__INDEX__)      do{if((__INDEX__) == DI_Board_1)    HAL_GPIO_WritePin(SPI1_DIB1_CS_Port,SPI1_DIB1_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == DI_Board_2)    HAL_GPIO_WritePin(SPI1_DIB2_CS_Port,SPI1_DIB2_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == DI_Board_3)    HAL_GPIO_WritePin(SPI1_DIB3_CS_Port,SPI1_DIB3_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == DI_Board_4)    HAL_GPIO_WritePin(SPI1_DIB4_CS_Port,SPI1_DIB4_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == DQ_Board_1)    HAL_GPIO_WritePin(SPI1_DQB1_CS_Port,SPI1_DQB1_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == DQ_Board_2)    HAL_GPIO_WritePin(SPI1_DQB2_CS_Port,SPI1_DQB2_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == RS485_Board)   HAL_GPIO_WritePin(SPI1_RS485B_CS_Port,SPI1_RS485B_CS,GPIO_PIN_SET); else \
                                           if((__INDEX__) == MENU_Board)    HAL_GPIO_WritePin(SPI1_MENUB_CS_Port,SPI1_MENUB_CS,GPIO_PIN_SET); \
                                        }while(0)



/*                                     
typedef enum 
{
    SpiTrans_Err        = -1,
    SpiTrans_Wait       = 0, 
    SpiTrans_WakeUp     = 1,
    SpiTrans_Continue   = 2,
    SpiTrans_End        = 3
}SpiTransStatus_TypeDef;
*/
typedef enum 
{
  SpiTrans_TimeOut    = -2,
  SpiTrans_Err        = -1,
  SpiTrans_Wait       = 0, 
  SpiTrans_M2S_TxEnd,
  SpiTrans_S2M_RxEnd,
  SpiTrans_WakeUp,
  SpiTrans_Continue,
  SpiTrans_End
}SpiTransStatus_TypeDef;

typedef enum 
{
  NO_Board,
  DI_Board_1,
  DI_Board_2,
  DI_Board_3,
  DI_Board_4,
  DQ_Board_1,
  DQ_Board_2,
  RS485_Board,
  MENU_Board     
}currentBoard_TypeDef;
extern currentBoard_TypeDef     currentBoard;
extern SpiTransStatus_TypeDef   sTransState[sTransBoard_Max];
#define spiRxDataLen  50
extern uint8_t spiRxData[spiRxDataLen];

void SPITransfer_Init(void);
void Master_Synchro(currentBoard_TypeDef cBoard);
void Slave_Synchro(currentBoard_TypeDef cBoard);
void reset(void);

//Packet  packet;


#ifdef __cplusplus
}
#endif

#endif  //_SPITransfer_H_