/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "MSP_CAN.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan2;

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  * @note 
  * CAN_MODE_NORMAL             正常模式：      CAN外设正常地向CAN总线发送数据并从CAN总线上接收数据。
  * CAN_MODE_LOOPBACK           回环模式：      CAN外设正常向CAN总线发送数据，同时接收自己发送的数据，但不从CAN总线上接收数据。在学习CAN外设的时候非常有用，特别是在没有专门的USB转CAN模块也没有两块开发板的时候。
  * CAN_MODE_SILENT             静默模式：      CAN外设不向CAN总线发送数据，仅从CAN总线上接收数据，但不会应答。一般用于检测CAN总线的流量。
  * CAN_MODE_SILENT_LOOPBACK    静默回环模式：   CAN外设不会往CAN总线收发数据，仅给自己发送。一般用于自检。
  *
  * TimeTriggeredMode     时间触发模式：        会自动为报文生成时间戳，没有研究过，暂时默认关闭。
  * AutoBusOff            自动总线关闭管理：    没有研究过，暂时默认关闭。
  * AutoWakeUp            自动唤醒模式：       使能后，当CAN外设在休眠状态时如果CAN总线有数据，则自动唤醒CAN外设。
  * AutoRetransmission    自动重发：          使能后，如果因为仲裁失败（总线冲突）或是其他原因导致发送失败，会自动重发。建议使能。
  * ReceiveFifoLocked     接收FIFO锁定模式：   如果使能，当接收FIFO满时，下一条数据会被丢失。如果不使能，则覆盖前面的数据。
  * TransmitFifoPriority  发送FIFO优先级：     当发送邮箱中同时有多个帧，是按照先进先出的顺序发送还是按照ID的优先级发送。如果不使能，则按照ID优先级发送
  */
void MX_CAN2_Init(void)
{
  hcan2.Instance = CAN1;
  hcan2.Init.Prescaler = 10;
  hcan2.Init.Mode = CAN_MODE_NORMAL;  //CAN_MODE_LOOPBACK;  //CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_8TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = ENABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    LOG("can init error\r\n");   //Error_Handler();
  }
  LOG("hcan2 instance esr : %ld\r\n", hcan2.Instance);

}


/**
* @brief CAN MSP Initialization
* This function configures the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    CAN2_GPIO_CLK_ENABLE();   //__HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = CAN2_RX_PIN|CAN2_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(CAN2_GPIO_PORT, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN2_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_SCE_IRQn);

  }

}

/**
* @brief CAN MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN2)
  {
    __HAL_RCC_CAN2_CLK_DISABLE();

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(CAN2_GPIO_PORT, CAN2_RX_PIN|CAN2_TX_PIN);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_SCE_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
