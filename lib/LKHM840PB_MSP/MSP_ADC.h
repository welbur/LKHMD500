#ifndef _MSP_ADC_H_
#define _MSP_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "PowerBoardConfig.h"
//#include "ErrorInfo.h"

extern ADC_HandleTypeDef hadc1;

void MX_ADC1_Init(void);

#ifdef __cplusplus
}
#endif

#endif
