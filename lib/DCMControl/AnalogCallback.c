
#include "DCMCtrl.h"
#include "MSP_TIM.h"
#include "LOG.h"


//when DMA conversion is completed, HAL_ADC_ConvCpltCallback function
// will interrupt the processor. You can find this function in
#if 1
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	// I set adc_conv_complete_flag variable to 1 when,
	// HAL_ADC_ConvCpltCallback function is call.
	ADC_CONV_COMPLETE_FLAG = 1;
//	if (hadc->Instance == ADC2)
//		LOG("HAL_ADC_ConvCpltCallback\r\n");
}
#endif

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc)
{
	LOG("HAL_ADC_ErrorCallback\r\n");
}