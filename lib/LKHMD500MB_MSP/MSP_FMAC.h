#ifndef _MSP_FMAC_H_
#define _MSP_FMAC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "LOG.h"
#include "PinConfig.h"
#include "BoardConfig.h"


/* Filter parameter P: number of feed-forward taps or coefficients in the range [2:64] */
#define FMAC_COEFF_VECTOR_B_SIZE     4

/* Filter parameter Q: number of feedback coefficients in the range [1:COEFF_VECTOR_B_SIZE-1] */
#define FMAC_COEFF_VECTOR_A_SIZE     3

/* Throughput parameter: extra space in the input buffer (minimum: 0) */
#define FMAC_MEMORY_PARAMETER_D1     1

/* Throughput parameter: extra space in the output buffer (minimum: 1) */
#define FMAC_MEMORY_PARAMETER_D2     2

/* Throughput parameter: watermark threshold for the input buffer */
#define FMAC_INPUT_THRESHOLD                FMAC_THRESHOLD_1

/* Throughput parameter: watermark threshold for the output buffer (inferior or equal to MEMORY_PARAMETER_D1) */
#define FMAC_OUTPUT_THRESHOLD               FMAC_THRESHOLD_1

/* FMAC internal memory configuration: base address of the coefficient buffer */
#define FMAC_COEFFICIENT_BUFFER_BASE        0

/* FMAC internal memory configuration: size of the two coefficient buffers */
#define FMAC_COEFFICIENT_BUFFER_SIZE        FMAC_COEFF_VECTOR_B_SIZE + FMAC_COEFF_VECTOR_A_SIZE

/* FMAC internal memory configuration: base address of the input buffer */
#define FMAC_INPUT_BUFFER_BASE              FMAC_COEFFICIENT_BUFFER_SIZE

/* FMAC internal memory configuration: size of the input buffer */
#define FMAC_INPUT_BUFFER_SIZE              FMAC_COEFF_VECTOR_B_SIZE + FMAC_MEMORY_PARAMETER_D1

/* FMAC internal memory configuration: base address of the input buffer */
#define FMAC_OUTPUT_BUFFER_BASE             FMAC_COEFFICIENT_BUFFER_SIZE + FMAC_INPUT_BUFFER_SIZE

/* FMAC internal memory configuration: size of the input buffer */
#define FMAC_OUTPUT_BUFFER_SIZE             FMAC_COEFF_VECTOR_A_SIZE + FMAC_MEMORY_PARAMETER_D2

/* FMAC WDATA register address (for DMA transfer) */
#define FMAC_WDATA (0x40021418)

/* coefficients for 200kHz power supply at 170 MHz */
/* Fixed point coefficients */
#define B0 ((int16_t)0x599C)
#define B1 ((int16_t)0xB177)
#define B2 ((int16_t)0xA6BB)
#define B3 ((int16_t)0x4EE0)
#define A1 ((int16_t)0x0616)
#define A2 ((int16_t)0xFE93)
#define A3 ((int16_t)0xFF57)

#define KC_shift (5)


void MX_FMAC_Init(void);
void VT_FMAC_Config(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif      /* MSP_FMAC_H  */

