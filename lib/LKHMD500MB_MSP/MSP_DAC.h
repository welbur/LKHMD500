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

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;
extern DAC_HandleTypeDef hdac3;
extern DAC_HandleTypeDef hdac4;
extern OPAMP_HandleTypeDef hopamp1;
extern OPAMP_HandleTypeDef hopamp4;

void MX_DAC1_Init(void);
void MX_DAC2_Init(void);
void MX_DAC3_Init(void);
void MX_DAC4_Init(void);
void MX_OPAMP1_Init(void);
void MX_OPAMP4_Init(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
