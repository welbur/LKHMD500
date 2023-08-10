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
#include "time.h"
#include "cmsis_os.h"
#ifdef RS485_Board
#include "Modbus.h"
#endif
#if defined(PowerBoard_ACS37800)
  #include "ACS37800.h"
#elif defined(PowerBoard_MLX91220)
  #include "MLX91220.h"
#endif
#include "LOG.h"
//#include "ErrorInfo.h"

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
#define SCALE_FACTOR 100
#define randomPMData

osMutexId_t mutex;
//slaveboardINFOHandler_t sbINFOH;

osSemaphoreId_t     PMDataSemaphore;       //用于电量计量数据的信号量

uint8_t _TrigFlag = 0x00;
//uint32_t RPBD_All_Countt = 0;
//uint32_t RPBD_Act_Countt = 0;
#ifdef RS485_Board
uint8_t mod_preamble[MOD_PREAMBLE_SIZE] = {MOD_START_BYTE, 0, 0, 0};
#endif

#ifdef PowerBoard_ACS37800
extern PowerMonitorData_t PowerMonitorData[ACS37800_IC_Num];
#endif

// 事件标志ID
osEventFlagsId_t transSlaveBoardInfo_flagsID;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
	.name = "defaultTask",
	.stack_size = 32 * 4,
	.priority = (osPriority_t)osPriorityLow,
};

osThreadId_t osPrintLOG_TaskHandle;
const osThreadAttr_t osPrintLOGTask_attributes = {
	.name = "osPrintLOGTask",
	.stack_size = 256 * 4,
	.priority = (osPriority_t)osPriorityLow1,
};

osThreadId_t transSlaveBoardInfo_TaskHandle;
const osThreadAttr_t transSlaveBoardInfoTask_attributes = {
	.name = "transSlaveBoardInfoTask",
	.stack_size = 256 * 4,
	.priority = (osPriority_t)osPriorityNormal,
};

/* 1----Definitions for Start Slave Board SPITransTask */
osThreadId_t PowerMonitor_TaskHandle;
const osThreadAttr_t PowerMonitorTask_attributes = {
	.name = "PowerMonitorTask",
	.stack_size = 256 * 4,
	.priority = (osPriority_t)osPriorityNormal, // osPriorityHigh
};

#if 0
/* 1----Definitions for Start Slave Board SPITransTask */
osThreadId_t SPI2_TransTaskHandle;
const osThreadAttr_t SPI2_TransTask_attributes = {
	.name = "SPI2_TransTask",
	.stack_size = 256 * 4,
	.priority = (osPriority_t)osPriorityBelowNormal2, // osPriorityHigh
};
#endif
/* 2----Definitions for Start Slave Board SPITransTask */
osThreadId_t ReadMasterBDataTaskHandle;
const osThreadAttr_t ReadMasterBDataTask_attributes = {
	.name = "ReadMasterBDataTask",
	.stack_size = 256 * 4,
	.priority = (osPriority_t)osPriorityNormal, // osPriorityHigh
};

//Semaphore to access the PowerMonitor Data
const osSemaphoreAttr_t PMDataSphr_attributes = {
    .name = "PMDataSphr"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);



/* 生成随机浮点数，用于测试*/
float randFloat(float min, float max) {
	//srand(time(0));
    return min + (max - min) * ((float)rand() / RAND_MAX);
}




/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	mutex = osMutexNew(NULL);

	/* creation of work led Task */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
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
	/* Infinite loop */
	for (;;)
	{
		//osMutexAcquire(mutex, osWaitForever);	//打印调试信息用
		//osSemaphoreAcquire(PMDataSemaphore, osWaitForever); // 获取信号量
		LED_R_TogglePin;		//LED_R_TogglePin;	//WorkLed_TogglePin;
		//osSemaphoreRelease(PMDataSemaphore); // 释放信号量
		//osMutexRelease(mutex);
		//LOG("StartDefaultTask\r\n");
		//LOG("tim3 ch1 value : %d\r\n", HAL_GPIO_ReadPin(TIM3_GPIO_Port, TIM3_CH1_Pin));
		ReadPinValue();
		osDelay(30);
	}
}
