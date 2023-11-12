/*
 * BackPanelTrans.h
 *
 * Created: 2023-08-11
 *  Author: WU555
 */

#include "EtherCatPDO.h"
#include "MSP_TIM.h"
#include "PinConfig.h"
#include "DCMCtrl.h"
#include "eventos.h"   // EventOS Nano头文件



//DCM_Fault_h.EXTIPinCnt[] = {0, 0, 0, 0, 0};
//memset(DCM_Fault_h.EXTIPinCnt, 0, sizeof(DCM_Fault_h.EXTIPinCnt));

/**
 * @brief
 * This is the callback for HAL interrupts of UART TX used by Modbus library.
 * This callback is shared among all UARTS, if more interrupts are used
 * user should implement the correct control flow and verification to maintain
 * Modbus functionality.
 * @ingroup UartHandle UART HAL handler
 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) 
{
	if (_etherCatH->port == UartHandle)
	{
		// notify the end of TX
		_etherCatH->TxEnd = 1;
	}
}


/**
 * @brief
 * This is the callback for HAL interrupt of UART RX
 * This callback is shared among all UARTS, if more interrupts are used
 * user should implement the correct control flow and verification to maintain
 * Modbus functionality.
 * @ingroup UartHandle UART HAL handler
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) 
{
	if (_etherCatH->port == UartHandle)
	{
		_etherCatH->RecvBuf[_etherCatH->RecvBufCnt] = _etherCatH->RecvData;
		if (HAL_UART_Receive_IT(_etherCatH->port, &_etherCatH->RecvData, 1) == HAL_OK)
		{
			_etherCatH->RecvBufCnt++;
			_etherCatH->RecvLastTime = HAL_getTick_us();
			_etherCatH->RxEnd = 0;
			//LOG("RecvLastTime : %ld\r\n", _etherCatH->RecvLastTime);
		}
		else
		{
			LOG_error("EtherCatPDO receive it error!\r\n");
			Error_Handler();
		}
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
uint32_t tim2_cache = 0;
uint32_t tim6_cache = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		HAL_IncTick();
		tim2_cache++;
	}
	else if (htim->Instance == TIM6)
	{
		TIM6tick_10us += 10;
		
#if 0		
		if (TIM6tick_10us - tim6_cache >= 1000*1000)
		{
			tim6_cache = HAL_getTick_us();
			LOG("t32c............. : %ld\r\n", tim6_cache);
		}
#endif
#if 1
		/* 用于接收串口数据 */
		uint32_t t32c = TIM6tick_10us - _etherCatH->RecvLastTime;
		
		if ((t32c > _etherCatH->t35chars) && (_etherCatH->RxEnd == 0))	//_etherCatH->t35chars)
		{
			//LOG("t32c............. : %ld; ........rxend : %d\r\n", t32c, _etherCatH->RxEnd);
			_etherCatH->RxEnd = 1;
		}
#endif
		/* 用于GPIO输入引脚的防抖动 */

	}
	else if (htim->Instance == TIM7)
	{
		eos_tick(); // eventos系统的嘀嗒时间
	}
}



#if 0
/**
 * @brief EXTI line detection callbacks
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
#if defined(DEVBoardG4)
		case KEY_Pin:
			LOG("DEV button........\r\n");
			break;
#endif

		case DCM1_Fault_Pin:
			DCM_Fault_h.EXTIPinCnt[0]++;
			//LOG("DCM1_Fault_Pin..\r\n");
#if 0
			if (HAL_GPIO_ReadPin(DCM1_Fault_Port, DCM1_Fault_Pin) == 0)
			{
				if (DCM_Fault_h.EXTIPinCnt[0] == 0)
					DCM_Fault_h.TrigLastTime[0] = HAL_getTick_us();
				DCM_Fault_h.EXTIPinCnt[0]++;
				//DCM_Fault_h.PinValue[0] = HAL_GPIO_ReadPin(DCM1_Fault_Port, DCM1_Fault_Pin);
				//DCM_Fault_h.PinDebounde[0] = 0;
				//LOG("DCM1_Fault_Pin.......%d\r\n", DCM_Fault_h.EXTIPinCnt[0]);
			}
			else
			{
				//LOG("TrigLastTime : %ld\r\n", HAL_getTick_us() - DCM_Fault_h.TrigLastTime[0]);
				if ((HAL_getTick_us() - DCM_Fault_h.TrigLastTime[0] > 10*1000) && 
					(DCM_Fault_h.EXTIPinCnt[0] < 5) && (DCM_Fault_h.EXTIPinCnt[0] > 0))
				{
					DCM_Fault_Flag[0] = 1;
					
					LOG("DCM1_Fault_Pin ...................ok..............\r\n");
				}
				DCM_Fault_h.EXTIPinCnt[0] = 0;
			}
#endif
			break;

		default:
			LOG_error("exti gpio pin not found!\r\n");
	}
}
#endif
