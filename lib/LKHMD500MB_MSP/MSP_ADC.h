#ifndef _MSP_ADC_H_
#define _MSP_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "BoardConfig.h"
//#include "ErrorInfo.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void MX_ADC1_Init(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
