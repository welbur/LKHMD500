#ifndef _MSP_DAC_H_
#define _MSP_DAC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "BoardConfig.h"
//#include "ErrorInfo.h"

extern DMA_HandleTypeDef hdma_dac1_ch1;
extern DMA_HandleTypeDef hdma_dac1_ch2;
extern DMA_HandleTypeDef hdma_dac2_ch1;
extern DMA_HandleTypeDef hdma_dac3_ch1;
extern DMA_HandleTypeDef hdma_dac4_ch1;

void MX_ADC1_Init(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
