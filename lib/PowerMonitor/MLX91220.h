/*
 * MLX91220.h
 *
 * Created: 2023-06-21
 *  Author: WU555
 */

#ifndef MLX91220_H_
#define MLX91220_H_

// #include "FreeRTOS.h"
// #include "task.h"
#include "stm32f4xx_hal.h"
#include "PowerBoardConfig.h"
#include "PinConfig.h"
#include "LOG.h"
#include "RunningStatistics_C.h"

//#define TWO_PI    6.28318530717958647693
//#define PI        3.14159265358979323846

#define MLX91220_IC_Num 			9
//#define constrain(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
//#/define sq(x) ((x)*(x))


//uint64_t GetusTime(void);
void FiltersInit(void);

#endif /* MLX91220_H_ */