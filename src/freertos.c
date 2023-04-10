/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "SPITransfer_C.h"
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t countt = 0;
uint8_t mod_preamble[MOD_PREAMBLE_SIZE]   = {MOD_START_BYTE, 0, 0, 0};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
#if 1
/* 1----Definitions for Start Slave Board SPITransTask */
osThreadId_t SPI2_TransTaskHandle;
const osThreadAttr_t SPI2_TransTask_attributes = {
  .name = "SPI2_TransTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1, //osPriorityHigh
};
#endif
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Start_DIBoard_TransTask(void *argument);
void Start_SPI2_TransTask(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* creation of work led Task */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  SPI2_TransTaskHandle  = osThreadNew(Start_SPI2_TransTask, NULL, &SPI2_TransTask_attributes);
  /* creation of TaskmyTaskSlave */
#if 0
  DIBoard_TransTaskHandle  = osThreadNew(Start_DIBoard_TransTask, NULL, &DIBoard_TransTask_attributes);

#endif
  /* USER CODE END Init */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          :
  * Description        :
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/**
  * @}
  */

/**
  * @}
  */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  	/* USER CODE BEGIN StartDefaultTask */
//	uint32_t msTime = HAL_GetTick();
  	/* Infinite loop */
  	for(;;)
  	{
		uint8_t dtt;
//		uint32_t msTime = HAL_GetTick();
		dtt = rand() % 10 + 0;
//		printf("%d, %ld\r\n", dtt, msTime);
//		if (HAL_GetTick() - msTime > dtt) {
			CHIPH[D_I_Q_Chip_1].isChipEnable = 1;
//			msTime = HAL_GetTick();
//		}
		
//		printf("%d, %d\r\n", dt, CHIPH[D_I_Q_Chip_1].isChipEnable);
    	if (CHIPH[D_I_Q_Chip_1].isChipEnable == 1) {
      		LED_G_TogglePin;
    	} else {
	      	LED_B_TogglePin;
    	}

    	//HAL_GPIO_TogglePin(GPIOC, GREEN_Pin);
    	//printf(".........................work led task......................\r\n");
    	//dtt += 10;
		osDelay(1+dtt);
  }
  /* USER CODE END StartDefaultTask */
}

#if 0
/****************   1-----DI Board 1     ********************
* @brief Function implementing the DI Board 1 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DIBoard_TransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    uint8_t re_arr_size = MOD_PREAMBLE_SIZE;    //暂用数组前面的4个元素，作为包头使用
    uint8_t re_arr[128];
//    
		/*1-------------------------有触发信号后，读取相关slave板的所有数据*/
		for (int i = 0 ; i < SlaveBoard_Max ; i++)
		{
			//uint8_t boardID = whichBoard_Enable(SlaveBoardStatus.activeBoard, i);
      
			if (SlaveBoardH[i].isBoardEnable)
			{
        printf("board %d enable : %d\r\n", SlaveBoardH[i].BoardID, SlaveBoardH[i].isBoardEnable);
				void *sTrans = SPITransfer_C_New(&SlaveBoardH[i], &hspi1, SET_SPIMODE_MASTER);
				SPITransfer_C_Master_Spi1_Transfer(sTrans, SlaveBoardH[i].BoardID);
				printf("current board %d status : .....%d......~~~~~~~~~~~~%ld\r\n", i, SlaveBoardH[i].spiTransState, ++countt);
        SlaveBoardH[i].isBoardEnable = 0;
        if (SlaveBoardH[i].spiTransState == SpiTrans_End) {
          /*从spi通道读到数据后，把slave板从1-8所有的数据都读出来后，合并在一起，然后再发给modbus主机(pn板)*/
          COPY_ARRAY(re_arr + re_arr_size, SlaveBoardH[i].spiRx_uartTx_u8regs, SlaveBoardH[i].spiRx_uartTx_u8regs_size);
          re_arr_size += SlaveBoardH[i].spiRx_uartTx_u8regs_size;
        }
			}
		}


    if ( re_arr_size != MOD_PREAMBLE_SIZE) 
    {
      //printf("rec111 data : ");
  	  //for (int j = 0 ; j < re_arr_size ; j++) printf("%02X ", re_arr[j]);
  	  //printf(".......%d\r\n", re_arr_size);
      /*增加包头*/
      re_arr[0] = mod_preamble[0], re_arr[1] = mod_preamble[1];
      re_arr[2] = re_arr_size - MOD_PREAMBLE_SIZE;    //有效数据的长度
      re_arr[3] = MB_FC_READ_REGISTERS;               //功能指令码
      /*放到modbus里面去发送数据*/
      ModbusH.spiRx_uartTx_u8regs = re_arr;
      ModbusH.spiRx_uartTx_u8regs_size = re_arr_size;
      spiRxUartTxBuffer(&ModbusH);
    }

		//SlaveBoardStatus.activeBoard = NO_Board;
    osDelay(1);
    //printf("------------------------------------------------------------------\r\n");
    //printf("end di board...\r\n");
  }
}
#endif

#if 0
/****************   1-----DI Board 1     ********************
* @brief Function implementing the DI Board 1 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_SlaveBoard_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
		/*1-------------------------有触发信号后，读取相关slave板的所有数据*/
		if (SlaveBoardStatus.activeBoard != NO_Board)
		{
			for (int i = 1 ; i <= 8 ; i++)
			{
				uint8_t boardID = whichBoard_Enable(SlaveBoardStatus.activeBoard, i);
				//uint8_t boardID = cBoard;
				if (boardID)
				{
					void *sTrans = SPITransfer_C_New(&hspi1, boardID, SET_SPIMODE_MASTER);
					SlaveBoardStatus.sTransState[i] = SPITransfer_C_Master_Spi1_Transfer(sTrans, boardID);
					printf("current board %d status : .....%d.....\r\n", i, SlaveBoardStatus.sTransState[i]);
				}
			}
			SlaveBoardStatus.activeBoard = NO_Board;
		}
    //osDelay(1000);
  }
}
#endif
#if 0
/****************   2-----DI Board 2     ********************
* @brief Function implementing the DI Board 2 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DIB_2_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
		if (xSemaphoreTake(ModbusH.ModBusSphrHandle , portMAX_DELAY) == pdTRUE)
    {
      printf("ModbusH.u16regs[0] : %X, %X, %X, %X\r\n",  ModbusH.u16regs[ 0x0000 ], ModbusH.u16regs[ 0x0001 ], ModbusH.u16regs[ 0x0002 ], ModbusH.u16regs[ 0x0003 ]);
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, ModbusH.u16regs[0] & 0x1);
		  xSemaphoreGive(ModbusH.ModBusSphrHandle);
    }
    osDelay(1000);
  }
}
#endif
/****************   3-----DI Board 3     ********************
* @brief Function implementing the DI Board 3 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
#if 0
void Start_SPI2_TransTask(void *argument)
{
  uint8_t sTxRxFlag = SpiTxRx_WAIT;
	/* Infinite loop */
  	for(;;)
  	{
    	if (CHIPH[D_I_Q_Chip_1].isChipEnable == 1) {
			  TrigINT_ToMasterB(0);
			  CHIPH[D_I_Q_Chip_1].isChipEnable = 0;
        uint8_t txbuf = 0, rxData = 0, rxbuf[128];
        uint32_t msTickstart = HAL_GetTick();
        TrigINT_ToMasterB(1);
        rxbuf[0] = 0, rxbuf[1] = 0, rxbuf[2] = 0, rxbuf[3] = 0, rxbuf[4] = 0;
        //printf("rxbuf : %d", rxbuf[0]);
        do {
          if (!HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {//{printf("spi cs2 error....\r\n");continue;}
            if(HAL_SPI_Receive(&hspi2, rxbuf, 5, 20) != HAL_OK) {
              printf("spi MSP_SPI_read timeout....\r\n"); 
              //break;
            }
            while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {}
            //if(HAL_SPI_Receive(&hspi2, rxbuf, 5, 10) != HAL_OK) {printf("spi MSP_SPI_read timeout....\r\n"); break;}
            //while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {}
            //printf("cscscscs.....\r\n");
            //if (sTxRxFlag == SpiRx_COMPLETE) {{ printf("SpiRx_COMPLETE  read data\r\n"); break;}}
            if (HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {printf("spi2 cs is 1. end read data\r\n"); break;}
            
          }
//          if ((HAL_GetTick() - msTickstart) > 10)	{		//sTxRx_TimeOut) {
//      		  printf("\r\n....spi rx timeout....\r\n");
//      		  break;
//    	    }
    	  }while((HAL_GetTick() - msTickstart) < 50);  //(rxData != 0xAC);
        printf("msTickstart : %ld\r\n", (HAL_GetTick() - msTickstart));
        //if (sTxRxFlag == SpiRx_COMPLETE) {
          printf("rxdata : %02X, %02X, %02X, %02X, %02X\r\n", rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4]);
        //}
      }
    	osDelay(1);
  	}
}
#endif
#if 1

void Start_SPI2_TransTask(void *argument)
{
  	//int dt = 1;
	/* Infinite loop */
  	for(;;)
  	{
//		printf("write data to master start~~~~~~~~~~~~~~~%ld\r\n", HAL_GetTick());
//    	if (testi < 10) {
    	if (CHIPH[D_I_Q_Chip_1].isChipEnable == 1) {
        	HAL_SPI_MspInit(&hspi2);
			TrigINT_ToMasterB(1);
		  	CHIPH[D_I_Q_Chip_1].isChipEnable = 0;
			TrigINT_ToMasterB(0);
			LED_R(1);
        	void *sTrans = SPITransfer_C_New(&CHIPH[D_I_Q_Chip_1], &hspi2, SET_SPIMODE_SLAVE);
			SPITransfer_C_Slave_Spi2_Transfer(sTrans, CHIPH[D_I_Q_Chip_1].DChipID);
//    	  	testi++;
        	//dt = rand() % 10 + 2;  //ranint(10, 100);
        	/*发送完spi数据后，需要加一条打印指令，要不会报错(因为主控也加了，要去都得去掉)*/
        	printf("write data to master complete.........%ld\r\n", HAL_GetTick());
        	HAL_SPI_MspDeInit(&hspi2);
      	}
//    	} else CHIPH[D_I_Q_Chip_1].isChipEnable = 0;
      	LED_R(0);
      	osDelay(2);
  	}
}
#endif
/****************   4-----DI Board 4     ********************
* @brief Function implementing the DI Board 4 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DIB_4_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
/****************   5-----DQ Board 1     ********************
* @brief Function implementing the DQ Board 1 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DQB_1_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
/****************   6-----DQ Board 2     ********************
* @brief Function implementing the DQ Board 2 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DQB_2_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
/****************   7-----Menu Board     ********************
* @brief Function implementing the Menu Board SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_MenuB_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
/****************   8-----RS485 Board     ********************
* @brief Function implementing the RS485 Board SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_RS485B_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

