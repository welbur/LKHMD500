#ifndef _MSP_CRC_H_
#define _MSP_CRC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "BoardConfig.h"


/* User-defined polynomial */
#define CRC_POLYNOMIAL_16B    0x1021  /* X^16 + X^12 + X^5 + 1, CRC-CCITT generating polynomial */

/* User-defined CRC init value */
/* As the CRC is 16-bit long, the init value is 16-bit long as well */
#define CRC_INIT_VALUE      0   //0x5ABE           //0x5ABE



void MX_CRC_Init(void);
uint16_t CRC16_CCITT(const void *obj, uint16_t length);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
