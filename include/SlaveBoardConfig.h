
#ifndef _SLAVEBOARD_H_
#define	_SLAVEBOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#if RTOS_enable
#include "FreeRTOS.h"
#include "task.h"
#endif
#include <string.h>

/*设置从板标志*/
/*D_I_1_Board*/
/*D_I_2_Board*/
/*D_I_3_Board*/
/*D_I_4_Board*/
/*D_Q_1_Board*/
/*D_Q_2_Board*/
/*MENU_Board*/
/*RS485_Board*/

/*spi1用于读取di或者dq芯片，或者rs485芯片，或者led扩展芯片的寄存器*/
#if defined(D_I_Board) || defined(D_Q_Board)
//spi1 di(dq) chip 1 cs引脚
#define SPI1_DChip1_CS                         GPIO_PIN_5
#define SPI1_DChip1_CS_Port                    GPIOB
#define SPI1_DChip1_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DChip1_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 di(dq) chip 2 cs引脚
#define SPI1_DChip2_CS                       GPIO_PIN_6
#define SPI1_DChip2_CS_Port                  GPIOB
#define SPI1_DChip2_CS_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DChip2_CS_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 di(dq) chip 3 cs引脚
#define SPI1_DChip3_CS                        GPIO_PIN_7
#define SPI1_DChip3_CS_Port                   GPIOB
#define SPI1_DChip3_CS_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DChip3_CS_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
//spi1 di(dq) chip 4 cs引脚
#define SPI1_DChip4_CS                        GPIO_PIN_8
#define SPI1_DChip4_CS_Port                   GPIOB
#define SPI1_DChip4_CS_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI1_DChip4_CS_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

#define SPI1_CS_ENABLE(__INDEX__)       do{if((__INDEX__) == D_I_Q_Chip_1)    HAL_GPIO_WritePin(SPI1_DChip1_CS_Port, SPI1_DChip1_CS, GPIO_PIN_RESET); else \
                                           if((__INDEX__) == D_I_Q_Chip_2)    HAL_GPIO_WritePin(SPI1_DChip2_CS_Port, SPI1_DChip2_CS, GPIO_PIN_RESET); else \
                                           if((__INDEX__) == D_I_Q_Chip_3)    HAL_GPIO_WritePin(SPI1_DChip3_CS_Port, SPI1_DChip3_CS, GPIO_PIN_RESET); else \
                                           if((__INDEX__) == D_I_Q_Chip_4)    HAL_GPIO_WritePin(SPI1_DChip4_CS_Port, SPI1_DChip4_CS, GPIO_PIN_RESET);  \
                                        }while(0)
#define SPI1_CS_DISABLE(__INDEX__)      do{if((__INDEX__) == D_I_Q_Chip_1)    HAL_GPIO_WritePin(SPI1_DChip1_CS_Port, SPI1_DChip1_CS, GPIO_PIN_SET); else \
                                           if((__INDEX__) == D_I_Q_Chip_2)    HAL_GPIO_WritePin(SPI1_DChip2_CS_Port, SPI1_DChip2_CS, GPIO_PIN_SET); else \
                                           if((__INDEX__) == D_I_Q_Chip_3)    HAL_GPIO_WritePin(SPI1_DChip3_CS_Port, SPI1_DChip3_CS, GPIO_PIN_SET); else \
                                           if((__INDEX__) == D_I_Q_Chip_4)    HAL_GPIO_WritePin(SPI1_DChip4_CS_Port, SPI1_DChip4_CS, GPIO_PIN_SET);  \
                                        }while(0)
        
#endif

/*spi2作为slave板与主控板的通信接口*/
#define SPI2_CS                         GPIO_PIN_10
#define SPI2_CS_Port                    GPIOB
#define SPI2_CS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPI2_CS_CLK_DISABLE()           __HAL_RCC_GPIOB_CLK_DISABLE()

#define SlaveBoard_Max                  8       //
#define sTrans_TimeOut                  100       //5 = 5ms
#define sTxRx_TimeOut                   1000
#define spiRxDataLen                    100

//复制一个数组到另一个数组
#define COPY_ARRAY(dest, src, len) memcpy(dest, src, (len) * sizeof((src)[0]))
// 板ID位操作函数
//#define Enable_Board(reg, bit)   ((reg) |= (1 << (bit)))
//#define Disable_Baord(reg, bit) ((reg) &= ~(1 << (bit)))
#define whichBoard_Enable(reg, bit)  ((reg) & (1 << (bit)))         //(reg & (1 << bit)) >> bit
//#define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))
#define MOD_START_BYTE                  0x7E
//#define MOD_CMD_CODE                    0x03
#define MOD_PREAMBLE_SIZE               4
extern uint8_t mod_preamble[MOD_PREAMBLE_SIZE];  //   = {MOD_START_BYTE, 0, 0, 0};
//uint8_t mod_postamble[2] = {0, STOP_BYTE};

typedef enum 
{
  SpiTrans_TimeOut          = -5,
  SpiTrans_S2M_RxData_Err   = -4,
  SpiTrans_S2M_RxAck_Err    = -3,
  SpiTrans_M2S_TxAck_Err    = -2,
  SpiTrans_Err              = -1,
  SpiTrans_Wait             = 0, 
  SpiTrans_M2S_TxAck_End,
  SpiTrans_S2M_RxData_End,
  SpiTrans_WakeUp,
  SpiTrans_Continue,
  SpiTrans_End
}SpiTransStatus_TypeDef;

#if defined(D_I_Board) || defined(D_Q_Board)
typedef enum 
{
//  NO_Board      = 0,
  D_I_Q_Chip_1    = 0,     //= 0b00000001,
  D_I_Q_Chip_2    ,     //= 0b00000010,	//2,
  D_I_Q_Chip_3    ,     //= 0b00000100,
  D_I_Q_Chip_4    ,     //= 0b00001000, 
}DChipID_TypeDef;

#endif
typedef enum 
{
    Read_RegData        = 0x11,
    Read_CurrentData    = 0x22,
    Config_Para         = 0xCE,  
}slaveBoardCmdCode_TypeDef;

typedef enum 
{
    Read_DI0_DI7        = 0x01,
    Read_DI8_DI15       = 0x02,
    Read_DI16_DI23      = 0x03,
    Read_DI24_DI31      = 0x04,

}slaveBoardRegAddr_TypeDef;


/**
 * @struct D_I_BoardHandler_t
 * @brief
 * DI board handler structure
 * Contains all the variables required for Modbus daemon operation
 */
typedef struct
{
  DChipID_TypeDef           DChipID;
  uint8_t                   isChipEnable;
  SpiTransStatus_TypeDef    spiTransState;

  uint8_t                  *spi1Rx_spi2Tx_u8regs;
//  uint8_t                  *spi1Tx_spi2Rx_u8regs;
  uint8_t                  spi1Rx_spi2Tx_u8regs_size;
//  uint8_t                  spiTx_uartRx_u8regs_size;

}CHIPHandler_t;

/*  DI Board cmd
 *  cmd 由5个字节组成：boardID + CmdCode + RegAddr + para1 + para2
 *                             |   |
 *            0x11：读取的数据 __|   |
 *            0x22：配置的参数 ______|
 *  byte1 byte2 byte3 byte4 byte5
*/
//activeBoard_TypeDef             boardID;
//slaveBoardCmdCode_TypeDef       sBoardCmdCode;
//slaveBoardRegAddr_TypeDef       sBoardRegAddr;
//slaveBoardPara1_TypeDef         sBoardPara_1;
//slaveBoardPara2_TypeDef         sBoardPara_2;       //备用


#ifdef __cplusplus
}
#endif

#endif
