/**
  ****************************(C) COPYRIGHT 2023 ButupMe_TECH*********************
  * @file       SPITransfer.c/h
  * @brief      多块板卡之间的背板协议，以并联式spi连接在一起
  * @note      	
  * @history
  *  Version    Date            Author          Modification
  *  V0.0.1     2023.3.1     	              	1. WU555
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2023 ButupMe_TECH*********************
  */

#include "SPITransfer.h"

SpiTransStatus_TypeDef sTransState[sTransBoard_Max];
uint8_t SpiRxAckByte;

uint8_t spiRxData[spiRxDataLen];
uint8_t sTransStaterx = 0;

uint8_t bytesRead = 0;
int8_t  status    = 0;

void SPITransfer_Init(void) 
{
/*******************************************       先初始化需要用到的GPIO引脚      *************************************/
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 PB5 PB6 PB7 
     *        PB0~PB7定义为spi1 cs的引脚
    */
    SPI1_DIB1_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DIB1_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DIB1_CS_Port, &GPIO_InitStruct);
    SPI1_DIB2_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DIB2_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DIB2_CS_Port, &GPIO_InitStruct);
    SPI1_DIB3_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DIB3_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DIB3_CS_Port, &GPIO_InitStruct);
    SPI1_DIB4_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DIB4_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DIB4_CS_Port, &GPIO_InitStruct);
    SPI1_DQB1_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DQB1_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DQB1_CS_Port, &GPIO_InitStruct);
    SPI1_DQB2_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_DQB2_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_DQB2_CS_Port, &GPIO_InitStruct);
    SPI1_RS485B_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_RS485B_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_RS485B_CS_Port, &GPIO_InitStruct);
    SPI1_MENUB_CS_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1_MENUB_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SPI1_MENUB_CS_Port, &GPIO_InitStruct);
/*******************************************       将所有cs引脚都默认设为高电平     *************************************/
    HAL_GPIO_WritePin(SPI1_DIB1_CS_Port,SPI1_DIB1_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_DIB2_CS_Port,SPI1_DIB2_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_DIB3_CS_Port,SPI1_DIB3_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_DIB4_CS_Port,SPI1_DIB4_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_DQB1_CS_Port,SPI1_DQB1_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_DQB2_CS_Port,SPI1_DQB2_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_RS485B_CS_Port,SPI1_RS485B_CS,GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_MENUB_CS_Port,SPI1_MENUB_CS,GPIO_PIN_SET);
}


/**
  * @brief  Master Synchronization with Slave.
  * @param  currentBoard_TypeDef 当前触发的板子号
  * @retval None                                            如果同步成功，返回true
  */
void Master_Synchro(currentBoard_TypeDef cBoard)
{
  uint8_t txackbytes = SPI_MASTER_SYNBYTE;  //, rxackbytes = 0x00;
  uint32_t msTickstart = HAL_GetTick();
  void *packet = Packet_C_New();
  printf("current board : --------------------------------------------------------------------%d\r\n", cBoard);
  //while((rxackbytes != SPI_SLAVE_SYNBYTE) && (cBoard != 0));
  while(cBoard != 0)
  {
    /* Send Master ACK signal to slave */
    SPI1_CS_ENABLE(cBoard);     //将当前触发的板子的cs信号使能
    if(HAL_SPI_Transmit(&hspi1, &txackbytes, 1, 10) != HAL_OK) sTransState[cBoard] = SpiTrans_Err;
    while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
    SPI1_CS_DISABLE(cBoard);    //关闭当前触发的板子的cs信号

    //从slave板接收数据
	  uint8_t recChar = 0xF0;
	  bytesRead = 0;

    uint8_t rxi = 0;
	  do 
    {
  		//recChar = _spi->receiveOneData();
      SPI1_CS_ENABLE(cBoard);     //将当前触发的板子的cs信号使能
      if(HAL_SPI_Receive(&hspi1, &recChar, 1, 10) != HAL_OK) sTransState[cBoard] = SpiTrans_Err;
      while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
      SPI1_CS_DISABLE(cBoard);    //关闭当前触发的板子的cs信号
      spiRxData[rxi++] = recChar;
		  bytesRead                = Packet_C_parse(packet, recChar, 1);                            //packet.parse(recChar);
		  status                   = Packet_C_getPacketStatus(packet);                           //packet.status;
		  if (status != CONTINUE_C) {
        printf("status : %d\r\n", status);
			  if (status < 0) reset();
			  break;
		  }
	  }while(recChar != 129); //0xAA);
    printf("read spi data len : %d\r\n", rxi);
    for (int i = 0; i < rxi; i++) {
      printf("%d, ", spiRxData[i]);
    }
    printf("-------------%ld\r\n", HAL_GetTick());
    printf("status : %d\r\n", status);
    rxi = 0;
#if 0
    //for (int i = 0; i < spiRxDataLen; i++)
    uint8_t rxi = 0;
    do
    {
      SPI1_CS_ENABLE(cBoard);     //将当前触发的板子的cs信号使能
      if(HAL_SPI_Receive(&hspi1, &spiRxData[rxi++], 1, 10) != HAL_OK) sTransState[cBoard] = SpiTrans_Err;
      while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
      SPI1_CS_DISABLE(cBoard);    //关闭当前触发的板子的cs信号
      //printf("read spi data  : %d\r\n", spiRxData[rxi-1]);
    }while(spiRxData[rxi-1] != 0xAA);
    printf("read spi data len : %d\r\n", rxi);
    for (int i = 0; i < rxi; i++) {
      printf("%d, ", spiRxData[i]);
    }
    printf("-------------%ld\r\n", HAL_GetTick());
    if (spiRxData[rxi-1] == 0xAA) {rxi = 0; break;}
#endif

    if ((HAL_GetTick() - msTickstart) > sTrans_TimeOut) 
    {
      //printf("tick : %ld\r\n", HAL_GetTick());
      printf("spi trans timeout\r\n");
      sTransState[cBoard] = SpiTrans_TimeOut;
      break;
    }
  }
  //printf("end spi synchro\r\n");
  //return true;
}

/**
  * @brief  Slave synchronization with Master
  * @param  currentBoard_TypeDef 当前触发的板子号
  * @retval None
  */
void Slave_Synchro(currentBoard_TypeDef cBoard)
{
  uint8_t txackbyte = SPI_SLAVE_SYNBYTE, rxackbyte = 0x00;
  do
  {
    if (HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&txackbyte, (uint8_t *)&rxackbyte, 1, HAL_MAX_DELAY) != HAL_OK)
    {
      printf("spi slave synchro error");
      //Error_Handler();
    }
  }
  while (rxackbyte != SPI_MASTER_SYNBYTE);
}
#if 0
void spiTrans_write(SPI_HandleTypeDef *hspi, uint8_t *wData, uint16_t size) 
{
  SPI1_CS_ENABLE(cBoard);     //将当前触发的板子的cs信号使能
  if(HAL_SPI_Transmit(hspi, wData, size, 10) != HAL_OK);
  while(HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY) {}
  SPI1_CS_DISABLE(cBoard);    //关闭当前触发的板子的cs信号
}

void spiTrans_read(SPI_HandleTypeDef *hspi, uint8_t *rData, uint16_t size) 
{
  SPI1_CS_ENABLE(cBoard);     //将当前触发的板子的cs信号使能
  if(HAL_SPI_Receive(hspi, rData, size, 10) != HAL_OK);
  while(HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY) {}
  SPI1_CS_DISABLE(cBoard);    //关闭当前触发的板子的cs信号
}
#endif

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
  /* Turn LED on: Transfer in transmission process is correct */
  printf("\r\nspi rx callback\r\n");
  sTransStaterx = 1;
  /* Turn LED on: Transfer in reception process is correct */
  //sTransState[currentBoard] = SpiTrans_S2M_RxEnd;
  
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED on: Transfer in transmission process is correct */
  printf("spi tx callback\r\n");
  /* Turn LED on: Transfer in reception process is correct */
  sTransState[currentBoard] = SpiTrans_M2S_TxEnd;
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
  sTransState[currentBoard] = SpiTrans_Err;
  HAL_SPI_MspDeInit(hspi);
  HAL_SPI_MspInit(hspi);
}

//void spiTrans_Error()

void reset(void)
{
	//_spi->transfer(0xFF);
  void *packet = Packet_C_New();
  Packet_C_reset(packet);                         //packet.reset
  status = Packet_C_getPacketStatus(packet);    	//status = packet.status;
	printf("spi transfer reset.......%d\r\n", status);
}