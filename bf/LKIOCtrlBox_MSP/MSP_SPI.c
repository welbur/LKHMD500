/**
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  * @file       BSP_spi.c/h
  * @brief      ��HAL��SPI�������ж��η�װ������Ӧ���������е���
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V3.0.0     2020.7.14     	              	1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  */
#include "MSP_SPI.h"

/* transfer state */
__IO uint32_t sTxRxFlag;

SPI_HandleTypeDef hspi1; /*主模式，用于读取各类芯片的寄存器数据*/
SPI_HandleTypeDef hspi2; /*从模式，用于跟主控板通信*/

#if defined(D_I_Board) || defined(D_Q_Board)
/* SPI1 init function */
void MX_SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI1_MODE;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES; // SPI_DIRECTION_1LINE;                         //SPI_DIRECTION_2LINES
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;				  // SPI_POLARITY_LOW
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;					  // SPI_PHASE_1EDGE
	hspi1.Init.NSS = SPI_NSS_SOFT;							  // SPI_NSS_HARD_OUTPUT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; // SPI_BAUDRATEPRESCALER_256
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10; // 7
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		LOGE("error spi");
		// Error_Handler();
	}
}
#endif
/* SPI2 init function */
void MX_SPI2_Init(void)
{
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI2_MODE;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES; // SPI_DIRECTION_1LINE;                   //SPI_DIRECTION_2LINES
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; // SPI_POLARITY_LOW
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;	   // SPI_PHASE_1EDGE
	hspi2.Init.NSS = SPI_NSS_SOFT;			   // SPI_NSS_HARD_INPUT;	//SPI_NSS_SOFT;
	// hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;       //SPI_BAUDRATEPRESCALER_256
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;

	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		LOGE("error spi\r\n");
		// Error_Handler();
	}
	LOGI("end spi2 init\r\n");
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
#if defined(D_I_Board) || defined(D_Q_Board)
	if (spiHandle->Instance == SPI1)
	{
		/* SPI1 clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		PA4     ------> SPI1_NSS
		PA5     ------> SPI1_SCK
		PA6     ------> SPI1_MISO
		PA7     ------> SPI1_MOSI
		*/
		GPIO_InitStruct.Pin = SPI1_SCK | SPI1_MISO | SPI1_MOSI;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL; // GPIO_PULLUP;     //GPIO_NOPULL
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(SPI1_Port, &GPIO_InitStruct);
		/* NVIC for SPI */
		HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);
	}
#endif
	if (spiHandle->Instance == SPI2)
	{
		/* SPI2 clock enable */
		__HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**SPI2 GPIO Configuration
		  PB12	 ------> SPI2_NSS
		PB13     ------> SPI2_SCK
		PB14     ------> SPI2_MISO
		PB15     ------> SPI2_MOSI
		*/
		GPIO_InitStruct.Pin = SPI2_SCK | SPI2_MISO | SPI2_MOSI;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
		HAL_GPIO_Init(SPI2_Port, &GPIO_InitStruct);
		/* NVIC for SPI */
		HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(SPI2_IRQn);
		//	LOGI("spi2 mspinit....\r\n");
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
#if defined(D_I_Board) || defined(D_Q_Board)
	if (spiHandle->Instance == SPI1)
	{
		/* Peripheral clock disable */
		//__HAL_RCC_SPI1_CLK_DISABLE();
		__HAL_RCC_SPI1_FORCE_RESET();
		__HAL_RCC_SPI1_RELEASE_RESET();
		/**SPI1 GPIO Configuration
		PA4     ------> SPI1_NSS
		PA5     ------> SPI1_SCK
		PA6     ------> SPI1_MISO
		PA7     ------> SPI1_MOSI
		*/
		HAL_GPIO_DeInit(SPI1_Port, SPI1_SCK | SPI1_MISO | SPI1_MOSI);
	}
#endif
	if (spiHandle->Instance == SPI2)
	{
		/* Peripheral clock disable */
		//__HAL_RCC_SPI2_CLK_DISABLE();
		__HAL_RCC_SPI2_FORCE_RESET();
		__HAL_RCC_SPI2_RELEASE_RESET();
		/**SPI2 GPIO Configuration
		PB13     ------> SPI2_SCK
		PB14     ------> SPI2_MISO
		PB15     ------> SPI2_MOSI
		*/
		HAL_GPIO_DeInit(SPI2_Port, SPI2_SCK | SPI2_MISO | SPI2_MOSI);
		HAL_NVIC_DisableIRQ(SPI2_IRQn);
	}
}

void SPITransfer_Init(void)
{
/*******************************************       先初始化需要用到的GPIO引脚      *************************************/
#if defined(D_I_Board) || defined(D_Q_Board)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pins : PB5 PB6 PB7 PB8
	 *        PB5~PB8定义为spi1 cs的引脚
	 */
	SPI1_DChip1_CS_CLK_ENABLE();
	GPIO_InitStruct.Pin = SPI1_DChip1_CS;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SPI1_DChip1_CS_Port, &GPIO_InitStruct);
	SPI1_DChip2_CS_CLK_ENABLE();
	GPIO_InitStruct.Pin = SPI1_DChip2_CS;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SPI1_DChip2_CS_Port, &GPIO_InitStruct);
	SPI1_DChip3_CS_CLK_ENABLE();
	GPIO_InitStruct.Pin = SPI1_DChip3_CS;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SPI1_DChip3_CS_Port, &GPIO_InitStruct);
	SPI1_DChip4_CS_CLK_ENABLE();
	GPIO_InitStruct.Pin = SPI1_DChip4_CS;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SPI1_DChip1_CS_Port, &GPIO_InitStruct);
	/*******************************************       将所有cs引脚都默认设为高电平     *************************************/
	HAL_GPIO_WritePin(SPI1_DChip1_CS_Port, SPI1_DChip1_CS, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI1_DChip2_CS_Port, SPI1_DChip2_CS, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI1_DChip3_CS_Port, SPI1_DChip3_CS, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI1_DChip4_CS_Port, SPI1_DChip4_CS, GPIO_PIN_SET);
#endif

#if 1
  	/*Configure spi2 cs pins : PB12 */
  	SPI2_CS_CLK_ENABLE();
  	GPIO_InitStruct.Pin = SPI2_CS;                                   
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP; //GPIO_NOPULL;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  	HAL_GPIO_Init(SPI2_CS_Port, &GPIO_InitStruct);
	//HAL_GPIO_WritePin(SPI2_CS_Port, SPI2_CS, GPIO_PIN_SET);
#endif
	//	HAL_GPIO_WritePin(SPI2_CS_Port, SPI2_CS, GPIO_PIN_SET);
	LOGI("spi trans init end....\r\n");
}

/**
 * @brief  MSP_SPI_write_read
 * @param  spiHandle: 指向哪个spi号
 * @param  cs: 指向Slave板的cs引脚
 * @param  txData: 指向需要发送数据的指针
 * @param  txLen: 需要发送数据的长度
 * @note   Master板 通过spi1发送数据给Slave板.
 * @retval 发送成功 返回 1； 发送失败 返回 0
 */
#if 1
uint8_t MSP_SPI_write_read(SPI_HandleTypeDef *spiHandle, uint8_t *txData, uint8_t *rxData, uint16_t dataLen)
{
	for (uint16_t i = 0; i < dataLen; i++)
	{
		if (HAL_SPI_TransmitReceive_IT(spiHandle, (txData + i), (rxData + i), 1) != HAL_OK)
			return 0;
	}
	return 1;
}
#endif

/**
 * @brief  MSP_SPI_write
 * @param  spiHandle: 指向哪个spi号
 * @param  cs: 指向Slave板的cs引脚
 * @param  txData: 指向需要发送数据的指针
 * @param  txLen: 需要发送数据的长度
 * @note   Master板 通过spi1发送数据给Slave板.
 * @retval 发送成功 返回 1； 发送失败 返回 0
 */
#if 1
uint8_t MSP_SPI_write(SPI_HandleTypeDef *spiHandle, uint8_t *txData, uint16_t txLen)
{
	for (uint16_t i = 0; i < txLen; i++)
	{
		if (HAL_SPI_Transmit_IT(spiHandle, (txData + i), 1) != HAL_OK)
			return 0;
	}
	return 1;
}
#endif
#if 0
uint8_t MSP_SPI_write(SPI_HandleTypeDef* spiHandle, uint8_t cs, uint8_t *txData, uint16_t txLen)
{
  for (uint16_t i = 0; i < txLen; i++)
  {
    uint32_t msTickstart = HAL_GetTick();
//	if (spiHandle->Instance==SPI1) SPI1_CS_ENABLE(cs);     //将当前触发的板子的cs信号使能
    if(HAL_SPI_Transmit_IT(spiHandle, (txData+i), 1) != HAL_OK) return 0;
    while(sTxRxFlag != SpiTx_COMPLETE) 
    {
      //if ((HAL_GetTick() - msTickstart) > sTxRx_TimeOut) return 0;
      if ((HAL_GetTick() - msTickstart) > sTxRx_TimeOut) {LOGI("spi tx timeout....\r\n"); return 0;}
    }
//    if (spiHandle->Instance==SPI1) SPI1_CS_DISABLE(cs);    //关闭当前触发的板子的cs信号
    sTxRxFlag = SpiTxRx_WAIT;
  }
  return 1;
}
#endif

/**
 * @brief  MSP_SPI_read
 * @param  spiHandle: 指向哪个spi号
 * @param  cs: 指向Slave板的cs引脚
 * @param  rxData: 指向需要发送数据的指针
 * @param  rxLen: 需要发送数据的长度
 * @note   通过spi号接收指定数量的数据.
 * @retval 发送成功 返回 1； 发送失败 返回 0
 */
#if 0
uint8_t MSP_SPI_read(SPI_HandleTypeDef* spiHandle, uint8_t cs, uint8_t *rxData, uint16_t rxLen)
{
	
  for (uint16_t i = 0; i < rxLen; i++)
  {
    //if (spiHandle->Instance==SPI1) SPI1_CS_ENABLE(cs);     //将当前触发的板子的cs信号使能
    if(HAL_SPI_Receive(spiHandle, (rxData+i), 1, 20) != HAL_OK) return 0;
    //while(sTxRxFlag != SpiRx_COMPLETE) {}
	while(HAL_SPI_GetState(spiHandle) != HAL_SPI_STATE_READY) {}
    //if (spiHandle->Instance==SPI1) SPI1_CS_DISABLE(cs);    //关闭当前触发的板子的cs信号
    
  }
  //LOGI("spi read :  %d, ", rxData[0]);
  return 1;
}
#endif
#if 1
uint8_t MSP_SPI_read(SPI_HandleTypeDef *spiHandle, uint8_t *rxData, uint16_t rxLen)
{
	for (uint16_t i = 0; i < rxLen; i++)
	{
		if (HAL_SPI_Receive(spiHandle, (rxData + i), 1, 1) != HAL_OK)
		{
			// LOGE("spi MSP_SPI_read timeout....\r\n");
			return 0;
		}
		LOGI("read spi data : %d", *rxData);
		while (HAL_SPI_GetState(spiHandle) != HAL_SPI_STATE_READY)
		{
		}
	}
	return 1;
}
#endif
#if 0
uint8_t MSP_SPI_read(SPI_HandleTypeDef* spiHandle, uint8_t cs, uint8_t *rxData, uint16_t rxLen)
{
	uint8_t txbuf = 0;
  for (uint16_t i = 0; i < rxLen; i++)
  {
    if (spiHandle->Instance==SPI1) SPI1_CS_ENABLE(cs);     //将当前触发的板子的cs信号使能
	LOGI(" spi2 cs :  %d\r\n", HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS));
    if(HAL_SPI_TransmitReceive_IT(spiHandle, &txbuf, (rxData+i), 1) != HAL_OK) {LOGI("spi MSP_SPI_read timeout....\r\n"); return 0;}

    while(HAL_SPI_GetState(spiHandle) != HAL_SPI_STATE_READY) {}
    if (spiHandle->Instance==SPI1) SPI1_CS_DISABLE(cs);    //关闭当前触发的板子的cs信号
    
  }
  //LOGI("spi read :  %d, ", rxData[0]);
  return 1;
}
#endif

/**
 * @brief  MSP_SPI_read
 * @param  spiHandle: 指向哪个spi号
 * @param  cs: 指向Slave板的cs引脚
 * @param  rxData: 指向需要发送数据的指针
 * @param  rxLen: 需要发送数据的长度
 * @note   通过spi号接收指定数量的数据.
 * @retval 发送成功 返回 1； 发送失败 返回 0
 */
uint8_t MSP_SPI2_CS_STATUS()
{
	return HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS);
}

/**
 * @brief  TxRx Transfer completed callback.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report end of Interrupt TxRx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	//  LOGI("spi txrx callback\r\n");
	sTxRxFlag = SpiRx_COMPLETE;
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	//LOGI("spi tx callback\r\n");
	sTxRxFlag = SpiTx_COMPLETE;
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	// LOGI("spi rx callback\r\n");
	sTxRxFlag = SpiRx_COMPLETE;
}

/**
 * @brief  SPI error callbacks.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	LOGE("spi error callback\r\n");
	sTxRxFlag = SpiTxRx_ERROR;
	HAL_SPI_MspDeInit(hspi);
	HAL_SPI_MspInit(hspi);
}
