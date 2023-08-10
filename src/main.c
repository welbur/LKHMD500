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
#include "CO_app_STM32.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t 			DevButton = 0;	//测试用 
uint8_t				SPI2_CS_Trig = 0;	//spi2 片选信号的触发
uint32_t 			uhCapture1 = 0;
uint8_t				uhCapture_N = 0;
//uint8_t 			_slaveBoardID;
#ifdef RS485_Board
/*modbus相关参数*/
modbusHandler_t ModbusH;
uint16_t ModbusDATA[128];
#endif

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
void Error_Handler(void);

//void SPI1_CS_GPIO_Init(void);
/* Private user code ---------------------------------------------------------*/
/**/

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
	EXTILine_Config();
	
	MX_USART3_UART_Init();
	MX_DMA_Init();
#ifdef PowerBoard_ACS37800
	MX_SPI1_Init();
	SPI1_CS_GPIO_Init();
#endif

	MX_CAN2_Init();

//	FiltersInit();

	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_TIM12_Init();
	MX_TIM14_Init();
	LOG("Initialize all configured peripherals OK!\r\n");

#ifdef RS485_Board
	/* Modbus 从站初始化Slave initialization */
	ModbusH.uModbusType = MB_SLAVE;
	ModbusH.port = &huart2;
	ModbusH.u8id = 1; // slave ID,  For master it must be 0
	ModbusH.u16timeOut = 1000;
	ModbusH.EN_Port = NULL; // No RS485   //ModbusH.EN_Port = NULL;
	ModbusH.EN_Pin = 0;
	// ModbusH2.EN_Port = LD2_GPIO_Port; // RS485 Enable
	// ModbusH2.EN_Pin = LD2_Pin; // RS485 Enable
	ModbusH.u16regs = ModbusDATA;
	ModbusH.u16regsize = sizeof(ModbusDATA) / sizeof(ModbusDATA[0]);
	ModbusH.xTypeHW = USART_HW_DMA;
	LOGI("start modbus rtu master...\r\n");
	// Initialize Modbus library
	ModbusInit(&ModbusH);
	// Start capturing traffic on serial Port
	ModbusStart(&ModbusH);
	/***********/
#endif
//	HAL_TIM_Base_Start_IT(canopenNodeSTM32->timerHandle);
	/* Infinite loop */
//	osKernelInitialize(); /* Call init function for freertos objects (in freertos.c) */
	

//	MX_FREERTOS_Init();
	LOG("start freertos\r\n");
	/* Start scheduler */
//	osKernelStart();
	
	//waitBackPanelTaskDone();
	uint32_t msticktime = HAL_GetTick();
	while (1)
	{
		if ((HAL_GetTick() - msticktime) > 100)
		{
			msticktime = HAL_GetTick();
			//if (HAL_GPIO_ReadPin(TIM3_GPIO_Port, TIM3_CH1_Pin) == 1)
	  			LOG("tim3 ch1 value : %d\r\n", HAL_GPIO_ReadPin(TIM3_GPIO_Port, TIM3_CH1_Pin));
			//if (HAL_GPIO_ReadPin(TIM3_GPIO_Port, TIM3_CH2_Pin) == 1)
			//	LOG("tim3 ch2.... value : %d\r\n", HAL_GPIO_ReadPin(TIM3_GPIO_Port, TIM3_CH2_Pin));
			//HAL_Delay(100);
		}
	}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  * 		   /////////HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 100
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 3
  * @param  None
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
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

#ifdef UartPrintf
PUTCHAR_PROTOTYPE
{
	/* e.g. write a character to the USART1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 1);
	return ch;
}
#endif

#ifdef LOG_DEBUG
void Addto_osPrintf(char *format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[100];
	vsprintf(buffer, format, args);
	va_end(args);
	// char str[100] = "Output: ";
	strcat(LOG_MSG, buffer);
	// printf("%s\n", str);
}
#endif


#if 1
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}
#endif


/**
  * @brief  Output Compare callback in non blocking mode 
  * @param  htim : TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint32_t uhCapture = 0;
	//LOG("oc HAL_TIM_OC_DelayElapsedCallback \r\n");
	//uint32_t uhCapture = 0;
  /* TIM3_CH1 toggling with frequency = 195 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
  {
	uhCapture1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
	LOG("ch1 uhCapture : %ld, uhCCR1_Val : %ld\r\n", uhCapture1, uhCCR1_Val);
	if (uhCapture1 >= 2000)
	{
		HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_1);
		//uhCapture1 = 200;
	}
	//if (uhCapture == uhCCR1_Val) uhCapture = 600;
	//else uhCapture = uhCCR1_Val;
	/* Set the Capture Compare Register value */
	if (uhCapture_N == 0)
    {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 200));
		uhCapture_N++;
	} else if (uhCapture_N == 1)
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 1000));
		uhCapture_N++;
	} else 
	{
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCapture1 + 200));
		uhCapture_N = 0;
	}
	//HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
  }
  
  /* TIM3_CH2 toggling with frequency = 390 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
	LOG("ch2... uhCapture : %ld,  uhCCR2_Val : %ld\r\n", uhCapture, uhCCR2_Val);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, (uhCapture + uhCCR2_Val));   
  }
  
  /* TIM3_CH3 toggling with frequency = 780 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, (uhCapture + uhCCR3_Val));
  }
  
  /* TIM3_CH4 toggling with frequency = 1560 Hz */
  if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
  {
    uhCapture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    /* Set the Capture Compare Register value */
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, (uhCapture + uhCCR4_Val));
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		LOG("pwm pulse finished time : %ld\r\n", HAL_GetTick());
	}
}
/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM2 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		LOG("time : %ld\r\n", HAL_GetTick());
	}
	if (htim->Instance == TIM2)
	{
		HAL_IncTick();
		//LOG("hal inctick\r\n");
	}
  	// Handle CANOpen app interrupts
  	if (htim->Instance == TIM14)		//canopenNodeSTM32->timerHandle) 
	{
		LOG("can open app interrupt\r\n");
      	canopen_app_interrupt();
  	}
}


/**
 * @brief EXTI line detection callbacks
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		#if defined(DEVBoard) || defined(DEVBoardYD)
		case KEY_Pin:
			//CHIPH[D_I_Q_Chip_1].isChipEnable = 1;
			LOGI("DEV button........%ld\r\n", HAL_GetTick());
			//uhCapture = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_1, TIM_OCPOLARITY_HIGH);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uhCCR1_Val));
			if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK)
  			{
    			/* PWM Generation Error */
    			Error_Handler();
  			}
			//if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2) != HAL_OK)
  			//{
    			/* PWM Generation Error */
    		//	Error_Handler();
  			//}
			//if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_3) != HAL_OK)
  			//{
    			/* PWM Generation Error */
    	//		Error_Handler();
  		//	}
		//	if (HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_4) != HAL_OK)
  		//	{
    			/* PWM Generation Error */
    	//		Error_Handler();
  		//	}
			//HAL_TIM_Base_Start_IT(&htim14);
			break;
		#endif
		case ZRC_IN9_Pin:

			LOG("zrc int 9, start delay ms \r\n");
			break;
		default:
			LOGE("int gpio pin not found!");
	}
}


#ifdef USE_FULL_ASSERT
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
	   tex: LOGE("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
