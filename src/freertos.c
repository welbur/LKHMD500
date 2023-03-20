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
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 32 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
#if 1
/* 1----Definitions for Start Slave Board SPITransTask */
osThreadId_t DIBoard_TransTaskHandle;
const osThreadAttr_t DIBoard_TransTask_attributes = {
  .name = "DIBoard_TransTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1, //osPriorityHigh
};
#endif
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Start_DIBoard_TransTask(void *argument);


void usDelay(uint32_t us)
{
    uint32_t ticks;
    uint32_t lastWakeTime;
    ticks = us / portTICK_PERIOD_US;
    lastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&lastWakeTime, ticks);
}

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* creation of work led Task */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  /* creation of TaskmyTaskSlave */
#if 1
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
  /* Infinite loop */
  for(;;)
  {
    LED_G_TogglePin;
    //HAL_GPIO_TogglePin(GPIOC, GREEN_Pin);
    //printf("work led task\r\n");
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

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
		/*1-------------------------有触发信号后，读取相关slave板的所有数据*/
		if (SlaveBoardStatus.activeBoard != NO_Board)
		{
      //printf("SlaveBoardStatus.activeBoard : %d\r\n", SlaveBoardStatus.activeBoard);
			for (int i = 0 ; i < 4 ; i++)
			{
				uint8_t boardID = whichBoard_Enable(SlaveBoardStatus.activeBoard, i);
				//uint8_t boardID = cBoard;
        //printf("boardid : %d\r\n", boardID);
				if (boardID)
				{
					void *sTrans = SPITransfer_C_New(&hspi1, boardID, SET_SPIMODE_MASTER);
					SlaveBoardStatus.sTransState[i] = SPITransfer_C_Master_Spi1_Transfer(sTrans, boardID);
					printf("current board %d status : .....%d......~~~~~~~~~~~~%ld\r\n", i, SlaveBoardStatus.sTransState[i], ++countt);
				}
			}
			SlaveBoardStatus.activeBoard = NO_Board;
		}
    osDelay(1);
    //printf("------------------------------------------------------------------\r\n");
    //usDelay(100);
    //vTaskDelay(1000);
    //printf("end di board...\r\n");
  }
}
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
/****************   3-----DI Board 3     ********************
* @brief Function implementing the DI Board 3 SpiTrans thread.
* @param argument: Not used
* @retval None
*/
void Start_DIB_3_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
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

