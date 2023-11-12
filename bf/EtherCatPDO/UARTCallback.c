/*
 * BackPanelTrans.h
 *
 * Created: 2023-08-11
 *  Author: WU555
 */

#include "EtherCatPDO.h"
#include "MSP_TIM.h"

/**
 * @brief
 * This is the callback for HAL interrupts of UART TX used by Modbus library.
 * This callback is shared among all UARTS, if more interrupts are used
 * user should implement the correct control flow and verification to maintain
 * Modbus functionality.
 * @ingroup UartHandle UART HAL handler
 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {

	if (_etherCatH->port == UartHandle)
	{
		// notify the end of TX
		_etherCatH->TxEnd = 1;
	}
}

#if 1
/**
 * @brief
 * This is the callback for HAL interrupt of UART RX
 * This callback is shared among all UARTS, if more interrupts are used
 * user should implement the correct control flow and verification to maintain
 * Modbus functionality.
 * @ingroup UartHandle UART HAL handler
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {

	/* BackPanel Trans RTU RX callback BEGIN */

	if (_etherCatH->port == UartHandle)
	{
		//LOG("RecvBufCnt : %ld    \r\n", _etherCatH->RecvBufCnt);
		_etherCatH->RecvBuf[_etherCatH->RecvBufCnt] = _etherCatH->RecvData;
		if (HAL_UART_Receive_IT(_etherCatH->port, &_etherCatH->RecvData, 1) == HAL_OK)
		{
			
			_etherCatH->RecvBufCnt++;
			_etherCatH->RecvLastTime = HAL_getTick_2us();

		}
		else
		{
			LOG_error("BackPanelTrans receive it error!");
		}
	}

	/* BackPanel Trans RTU RX callback END */
}
#endif
