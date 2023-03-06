/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DATA_LENGTH                                 ((uint16_t)0x0020)   
/* Size of buffer */
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if 1
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#endif
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

  const char wbuf[] = "BORING-TECH STM32 TEST";
  char rbuf[50];

  const char wbuf1[] = "BORING-TECH STM32 TEST1";
  char rbuf1[20];

/* Buffer used for transmission */
uint8_t aTxBuffer[] = {100, 200}; //"****SPI - Two Boards communication based on Interrupt ****";

/* Buffer used for reception */
//uint8_t aRxBuffer[BUFFERSIZE];
uint8_t aRxBuffer[DATA_LENGTH];


currentBoard_TypeDef   currentBoard = NO_Board;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();

  EXTILine_Config();
  SPITransfer_Init();
  
//  wTransferState = TRANSFER_WAIT;
  Set_WorkLed_Status(WorkNormal, 1000);

  //打开串口接收中断
  if(HAL_UART_Receive_IT(&huart1, &rx1_buf, 1) != HAL_OK) Error_Handler();

  printf("wait spi2 receive \r\n");
  //打开SPI2接收中断
  /* Receive the requested data from the master */
  //if(HAL_SPI_Receive_IT(&hspi2, aRxBuffer, 5) != HAL_OK) Error_Handler();

  //while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY)
  //{}
  //printf("spi2 data : %d, %d, %d, %d, %d\r\n", aRxBuffer[0], aRxBuffer[1], aRxBuffer[2], aRxBuffer[3], aRxBuffer[4]);
  printf("start loop\r\n");
  HAL_Delay(2000);		
  /* Infinite loop */
  while (1)
  {
    //作为从设备，从主设备读取数据
    /*
    if(HAL_SPI_Receive_IT(&hspi2, aRxBuffer, 5) != HAL_OK) Error_Handler();
    while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {}
    printf("spi2 111 data : %d, %d, %d, %d, %d\r\n", aRxBuffer[0], aRxBuffer[1], aRxBuffer[2], aRxBuffer[3], aRxBuffer[4]);
    */

    WorkLed();
    //printf("current board : %d, strans state : %d\r\n", currentBoard, sTransState[currentBoard]);
    if (currentBoard != NO_Board && sTransState[currentBoard] != SpiTrans_End) {
      Master_Synchro(currentBoard);
      currentBoard = NO_Board;
    }
		//HAL_Delay(3000);		
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
#if 1
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif

PUTCHAR_PROTOTYPE
{
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
uint8_t ledr_v = 1, ledb_v = 1;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  ledr_v = 1 - ledr_v;
  ledb_v = 1 - ledb_v;
  switch (GPIO_Pin) 
  {
    case KEY_Pin:
      printf("DEV button\r\n");
      currentBoard = DI_Board_1;
      break;
    case DIB_INT_PIN1:
      printf("di board 1 int pin\r\n");
      currentBoard = DI_Board_1;
      //sTransState[DI_Board_1] =  SpiTrans_WakeUp;
      //Master_Synchro(DI_Board_1);
      break;
    case DQB_INT_PIN1:
      printf("DQ board 1 int pin\r\n");
      //sTransState[DQ_Board_1] =  SpiTrans_WakeUp;
      break;
    default:
      printf("int gpio pin not found!");
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  printf("error handler!\r\n");
  Set_WorkLed_Status(WorkError, 500);
  /* USER CODE END Error_Handler_Debug */
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
