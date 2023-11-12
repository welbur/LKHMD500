/*
 * BackPanelTrans.h
 *
 * Created: 2023-08-11
 *  Author: WU555
 */

#include "PWMCtrl.h"


uint8_t 			DevButton = 0;	//测试用 

/**
 * @brief EXTI line detection callbacks
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		#if defined(DEVBoard) || defined(DEVBoardYD)
		case KEY_Pin:
			//CHIPH[D_I_Q_Chip_1].isChipEnable = 1;
			LOG_info("DEV button........%ld\r\n", HAL_GetTick());
			//uhCapture = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_OCPOLARITY_HIGH);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCCR1_Val));
			if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK)
  			{
    			/* PWM Generation Error */
    			Error_Handler();
  			}
			//if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2) != HAL_OK)
  			//{
    			/* PWM Generation Error */
    		//	Error_Handler();
  			//}
			//if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_3) != HAL_OK)
  			//{
    			/* PWM Generation Error */
    	//		Error_Handler();
  		//	}
		//	if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_4) != HAL_OK)
  		//	{
    			/* PWM Generation Error */
    	//		Error_Handler();
  		//	}
			//HAL_TIM_Base_Start_IT(&htim14);
			break;
		#endif
		case ZRC_IN9_Pin:

			LOG_info("zrc int 9, start delay ms \r\n");
			break;
		default:
			LOG_error("int gpio pin not found!");
	}
}
