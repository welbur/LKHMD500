
#ifndef _SPITRANSFER_C_H_
#define	_SPITRANSFER_C_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
//#include "../LKIOCtrlBox_MSP/MSP_SPI.h"
#include "SlaveBoardConfig.h"


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


extern void *SPITransfer_C_New(SPI_HandleTypeDef *theSPI, uint8_t cs, uint8_t master);
extern SpiTransStatus_TypeDef SPITransfer_C_Master_Spi1_Transfer(void *SpiTrans, activeBoard_TypeDef cBoard, uint8_t boardID);


#ifdef __cplusplus
}
#endif

#endif
