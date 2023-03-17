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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 32 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
#if 1
/* 1----Definitions for Start DI Board 1 SPITransTask */
osThreadId_t DIB_1_SPITransTaskHandle;
const osThreadAttr_t DIB_1_SPITransTask_attributes = {
  .name = "DIB_1_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1, //osPriorityHigh
};

/* 2-----Definitions for Start DI Board 2 SPITransTask */
osThreadId_t DIB_2_SPITransTaskHandle;
const osThreadAttr_t DIB_2_SPITransTask_attributes = {
  .name = "DIB_2_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal2, //osPriorityHigh
};

/* 3-----Definitions for Start DI Board 3 SPITransTask */
osThreadId_t DIB_3_SPITransTaskHandle;
const osThreadAttr_t DIB_3_SPITransTask_attributes = {
  .name = "DIB_3_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal3, //osPriorityHigh
};
/* 4-----Definitions for Start DI Board 4 SPITransTask */
osThreadId_t DIB_4_SPITransTaskHandle;
const osThreadAttr_t DIB_4_SPITransTask_attributes = {
  .name = "DIB_4_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal4, //osPriorityHigh
};

/* 5-----Definitions for Start DQ Board 1 SPITransTask */
osThreadId_t DQB_1_SPITransTaskHandle;
const osThreadAttr_t DQB_1_SPITransTask_attributes = {
  .name = "DQB_1_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal5, //osPriorityHigh
};
/* 6-----Definitions for Start DQ Board 2 SPITransTask */
osThreadId_t DQB_2_SPITransTaskHandle;
const osThreadAttr_t DQB_2_SPITransTask_attributes = {
  .name = "DQB_2_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal6, //osPriorityHigh
};
/* 7-----Definitions for Start  Menu Board SPITransTask */
osThreadId_t MenuB_SPITransTaskHandle;
const osThreadAttr_t MenuB_SPITransTask_attributes = {
  .name = "MenuB_SPITransTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal, //osPriorityHigh
};
/* 8-----Definitions for Start RS485 Board SPITransTask */
osThreadId_t RS485B_SPITransTaskHandle;
const osThreadAttr_t RS485B_SPITransTask_attributes = {
  .name = "RS485B_SPITransTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7, //osPriorityHigh
};
#endif
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Start_DIB_1_SPITransTask(void *argument);
void Start_DIB_2_SPITransTask(void *argument);
void Start_DIB_3_SPITransTask(void *argument);
void Start_DIB_4_SPITransTask(void *argument);
void Start_DQB_1_SPITransTask(void *argument);
void Start_DQB_2_SPITransTask(void *argument);
void Start_MenuB_SPITransTask(void *argument);
void Start_RS485B_SPITransTask(void *argument);

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
  DIB_1_SPITransTaskHandle  = osThreadNew(Start_DIB_1_SPITransTask, NULL, &DIB_1_SPITransTask_attributes);

  DIB_2_SPITransTaskHandle  = osThreadNew(Start_DIB_2_SPITransTask, NULL, &DIB_2_SPITransTask_attributes);

  DIB_3_SPITransTaskHandle  = osThreadNew(Start_DIB_3_SPITransTask, NULL, &DIB_3_SPITransTask_attributes);
  DIB_4_SPITransTaskHandle  = osThreadNew(Start_DIB_4_SPITransTask, NULL, &DIB_4_SPITransTask_attributes);

  DQB_1_SPITransTaskHandle  = osThreadNew(Start_DQB_1_SPITransTask, NULL, &DQB_1_SPITransTask_attributes);
  DQB_2_SPITransTaskHandle  = osThreadNew(Start_DQB_2_SPITransTask, NULL, &DQB_2_SPITransTask_attributes);
  MenuB_SPITransTaskHandle  = osThreadNew(Start_MenuB_SPITransTask, NULL, &MenuB_SPITransTask_attributes);
  RS485B_SPITransTaskHandle = osThreadNew(Start_RS485B_SPITransTask, NULL, &RS485B_SPITransTask_attributes);
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
void Start_DIB_1_SPITransTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
}
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

