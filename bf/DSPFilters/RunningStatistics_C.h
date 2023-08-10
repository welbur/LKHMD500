
#ifndef _RUNNINGSTATISTICS_C_H_
#define	_RUNNINGSTATISTICS_C_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "LOG.h"
#include "PowerBoardConfig.h"


/*
 * 用于c调用
 */

extern void *RunningStatistics_C_New();
extern void RunningStatistics_C_delete(void *RunningStats);
extern void RunningStatistics_C_setWindowSecs(void *RunningStats, float windowLen);
extern void RunningStatistics_C_getACtrms(void *RunningStats, float acValue, float *AmpsTrms);


//extern void *SPITransfer_C_New(SlaveBoardHandler_t *slavebH, SPI_HandleTypeDef *theSPI, uint8_t master);
//extern void SPITransfer_C_Master_Spi1_Transfer(void *SpiTrans, uint8_t TxRxFlag, BoardID_TypeDef boardID);


#ifdef __cplusplus
}
#endif

#endif  //_RUNNINGSTATISTICS_C_H_