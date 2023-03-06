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

//uint32_t wTransferState = TRANSFER_WAIT;
//#include "spi.h"

/**
  * @brief          ��װSPI6д����
  * @param[in]     	data ����������
  * @retval         �յ�������
  */
uint8_t SPI1_WriteData(uint8_t *data,uint16_t size)
{
  printf("transmit spi1 data....\r\n");
	return HAL_SPI_Transmit(&hspi1,data,size, 1000);
}

/**
  * @brief          ��װSPI2��д������Ƭ��SPI Flashʹ�ã�
  * @param[in]     	TxData ����������
  * @retval         RxData �յ�������
  */
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata[5];
    HAL_SPI_Receive(&hspi2, Rxdata, 5, 1000); 
    printf("spi2 read data : %d, %d, %d, %d, %d\r\n", Rxdata[0], Rxdata[1], Rxdata[2], Rxdata[3], Rxdata[4]);      
 	return Rxdata;          		    //�����յ�������		
}

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* SPI1 init function */
void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI1_MODE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;      //SPI_DIRECTION_1LINE;                         //SPI_DIRECTION_2LINES
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;                         //SPI_POLARITY_LOW
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;                              //SPI_PHASE_1EDGE
  hspi1.Init.NSS = SPI_NSS_SOFT;                                      //SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;             //SPI_BAUDRATEPRESCALER_256
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;                                      //7
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    printf("error spi");
    //Error_Handler();
  }
}
/* SPI2 init function */
void MX_SPI2_Init(void)
{
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI2_MODE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;       //SPI_DIRECTION_1LINE;                   //SPI_DIRECTION_2LINES
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;                   //SPI_POLARITY_LOW
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;                        //SPI_PHASE_1EDGE
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;       //SPI_BAUDRATEPRESCALER_256
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    printf("error spi");
    //Error_Handler();
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
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
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;     //GPIO_PULLUP;     //GPIO_NOPULL
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  }
  else if(spiHandle->Instance==SPI2)
  {
    /* SPI2 clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(SPI2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI2_IRQn);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
  if(spiHandle->Instance==SPI1)
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
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
  }
  else if(spiHandle->Instance==SPI2)
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
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    HAL_NVIC_DisableIRQ(SPI2_IRQn);
  }
}

/**
  * @brief  Master Synchronization with Slave.
  * @param  None
  * @retval None
  */
#if 0
void Master_Synchro(SPI_HandleTypeDef hspi)
{
  uint8_t txackbytes = SPI_MASTER_SYNBYTE, rxackbytes = 0x00;
  //do
  //{
  //printf("cs pin : %d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4));
  for (int i =0; i<10; i++) {
    //printf("spi master synchro \r\n");
    /* Call SPI write function to send command to slave */
    if(HAL_SPI_TransmitReceive_IT(&hspi, (uint8_t *)&txackbytes, (uint8_t *)&rxackbytes, 1) != HAL_OK)
    {
      //printf("spi master synchro error\r\n");
      //Error_Handler();
    }
    //while(HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY)
    //{}
    //printf("receive spi data : %d\r\n", rxackbytes);
  }
  //}while(rxackbytes != SPI_SLAVE_SYNBYTE);
  printf("end spi synchro\r\n");
}

/**
  * @brief  Slave synchronization with Master
  * @param  None
  * @retval None
  */
void Slave_Synchro(SPI_HandleTypeDef hspi)
{
  uint8_t txackbyte = SPI_SLAVE_SYNBYTE, rxackbyte = 0x00;

  do
  {
    if (HAL_SPI_TransmitReceive(&hspi, (uint8_t *)&txackbyte, (uint8_t *)&rxackbyte, 1, HAL_MAX_DELAY) != HAL_OK)
    {
      printf("spi slave synchro error");
      //Error_Handler();
    }
  }
  while (rxackbyte != SPI_MASTER_SYNBYTE);
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of Interrupt TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
int sledr_v = 1;
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  uint8_t sRxBuffer[5];
  /* Turn LED on: Transfer in transmission process is correct */
  printf("spi rx callback\r\n");
  sledr_v = 1 - sledr_v;
  LED_R(sledr_v);

  /* Turn LED on: Transfer in reception process is correct */
  wTransferState = TRANSFER_COMPLETE;
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED on: Transfer in transmission process is correct */
  printf("spi txrx callback\r\n");
  /* Turn LED on: Transfer in reception process is correct */
  wTransferState = TRANSFER_COMPLETE;
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
  printf("spi error callback\r\n");
  wTransferState = TRANSFER_ERROR;
  //HAL_SPI_MspDeInit(&hspi2);
  SPI2_Reset();
  HAL_SPI_MspInit(&hspi2);
}

#if 0
void Master2Slave_Communicate(void) 
{
  /* Synchronization between Master and Slave */
  Master_Synchro(hspi1);
    
  /* Receive Data from the Slave ###########################################*/ 
  paddrcmd[0] = (uint8_t) (ADDRCMD_MASTER_READ >> 8);
  paddrcmd[1] = (uint8_t) ADDRCMD_MASTER_READ;
  paddrcmd[2] = (uint8_t) (DATA_LENGTH >> 8);
  paddrcmd[3] = (uint8_t) DATA_LENGTH;
  /* Send Master READ command to slave */
  if(HAL_SPI_Transmit_IT(&SpiHandle, paddrcmd, CMD_LENGTH) != HAL_OK)
  {
    Error_Handler();
  }

  while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
  /* Synchronization between Master and Slave */
  Master_Synchro();
    
  /* Receive ACK from the Slave */
  ackbytes = 0;
  if(HAL_SPI_Receive_IT(&SpiHandle, (uint8_t *)&ackbytes, sizeof(ackbytes)) != HAL_OK)
  {
    Error_Handler();
  }
  while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
  /* Check the received ACK */
  if(ackbytes == SPI_ACK_BYTES)
  {
    /* Synchronization between Master and Slave */
    Master_Synchro();
      
    /* Receive the requested data from the slave */
    if(HAL_SPI_Receive_IT(&SpiHandle, aRxBuffer, DATA_LENGTH) != HAL_OK)
    {
      Error_Handler();
    }
    while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
    /* Synchronization between Master and Slave */
    Master_Synchro();
      
    /* Send ACK to the Slave */
    ackbytes = SPI_ACK_BYTES;
    if(HAL_SPI_Transmit_IT(&SpiHandle, (uint8_t *)&ackbytes, sizeof(ackbytes)) != HAL_OK)
    {
      Error_Handler();
    }
    while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
  }    
  else
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
    
  /* Compare received buffer with one expected from slave */
  if(Buffercmp((uint8_t*)aTxSlaveBuffer, (uint8_t*)aRxBuffer, CMD_LENGTH))
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
  else
  {
    printf("compare faid!");
  }
  /* Synchronization between Master and Slave */
  Master_Synchro();
    
  /* Transmit Data To Slave ################################################*/
  paddrcmd[0] = (uint8_t) (ADDRCMD_MASTER_WRITE >> 8);
  paddrcmd[1] = (uint8_t) ADDRCMD_MASTER_WRITE;
  paddrcmd[2] = (uint8_t) (DATA_LENGTH >> 8);
  paddrcmd[3] = (uint8_t) DATA_LENGTH;
  /* Send Master WRITE command to the slave */
  if(HAL_SPI_Transmit_IT(&SpiHandle, paddrcmd, CMD_LENGTH) != HAL_OK)
  {
    Error_Handler();
  }
  while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
    /* Synchronization between Master and Slave */
  Master_Synchro();
    
  /* Receive ACK from the Slave */
  ackbytes = 0;
  if(HAL_SPI_Receive_IT(&SpiHandle, (uint8_t *)&ackbytes, sizeof(ackbytes)) != HAL_OK)
  {
    Error_Handler();
  }
  while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
  /* Check the received ACK */
  if(ackbytes == SPI_ACK_BYTES)
  {
    /* Synchronization between Master and Slave */
    Master_Synchro();
    /* Send the requested data from the slave */
    if(HAL_SPI_Transmit_IT(&SpiHandle, aTxMasterBuffer, DATA_LENGTH) != HAL_OK)
    {
      Error_Handler();
    }
    while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
    /* Synchronization between Master and Slave */
    Master_Synchro();
      
    /* Receive ACK from the Slave */
    ackbytes = 0;
    if(HAL_SPI_Receive_IT(&SpiHandle, (uint8_t *)&ackbytes, sizeof(ackbytes)) != HAL_OK)
    {
      Error_Handler();
    }
    while(HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY) {}
  }    
  else
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
   
  /* Flush Rx buffer for next transmission */
  Flush_Buffer(aRxBuffer, DATA_LENGTH);
    
}
#endif

#endif


