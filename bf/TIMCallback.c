/*
 * BackPanelTrans.h
 *
 * Created: 2023-08-11
 *  Author: WU555
 */

#include "PWMCtrl.h"
#include "MSP_TIM.h"


uint32_t 			uhCapture1 = 0;
uint8_t				uhCapture_N = 0;

/**
  * @brief  Output Compare callback in non blocking mode 
  * @param  htim : TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint32_t uhCapture = 0;
	//LOG("oc HAL_TIM_OC_DelayElapsedCallback \r\n");
	//uint32_t uhCapture = 0;
  /* TIM3_CH1 toggling with frequency = 195 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	uhCapture1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
	LOG("ch1 uhCapture : %ld, uhCCR1_Val : %ld\r\n", uhCapture1, uhCCR1_Val);
	if (uhCapture1 >= 2000)
	{
		HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_1);
		//uhCapture1 = 200;
	}
	//if (uhCapture == uhCCR1_Val) uhCapture = 600;
	//else uhCapture = uhCCR1_Val;
	/* Set the Capture Compare Register value */
	if (uhCapture_N == 0)
    {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 200));
		uhCapture_N++;
	} else if (uhCapture_N == 1)
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 1000));
		uhCapture_N++;
	} else 
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 200));
		uhCapture_N = 0;
	}
	//HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
  }
  
  /* TIM3_CH2 toggling with frequency = 390 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
	LOG("ch2... uhCapture : %ld,  uhCCR2_Val : %ld\r\n", uhCapture, uhCCR2_Val);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, (uhCapture + uhCCR2_Val));   
  }
  
  /* TIM3_CH3 toggling with frequency = 780 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, (uhCapture + uhCCR3_Val));
  }
  
  /* TIM3_CH4 toggling with frequency = 1560 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, (uhCapture + uhCCR4_Val));
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		LOG("pwm pulse finished time : %ld\r\n", HAL_GetTick());
	}
}
/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM2 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3) {
		LOG("time : %ld\r\n", HAL_GetTick());
	} else if (htim->Instance == TIM2) {
		HAL_IncTick();
		//LOG("hal inctick\r\n");
	} else if (htim->Instance == TIM6) {
		TIM6tick_us++;
	}

}

