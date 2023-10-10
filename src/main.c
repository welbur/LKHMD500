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

#include "eventos.h"                                // EventOS Nano头文件
#include "event_def.h"                              // 事件主题的枚举
#include "eos_event.h"                              // LED灯闪烁状态机

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

// BackPanelHandler_t BackPanelH;

#if (EOS_USE_PUB_SUB != 0)
static eos_u32_t eos_sub_table[Event_Max];          // 订阅表数据空间
#endif

uint16_t DeviceStateInfo = DeviceState_Default;  

uint32_t TransLastTime = 0;
uint32_t BPTrans_period = 10;

/* 与LAN9255通信协议的相关参数 */
EPDOHandler_t EtherCatPDOH;
INDATAParaHandle INDataValue;
OUTDATAParaHandle OUTDataValue;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

// void SPI1_CS_GPIO_Init(void);
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
	MX_DMA_Init();
	MX_CRC_Init();

	MX_USART1_UART_Init();
	MX_USART3_UART_Init();

//		MX_TIM3_Init();
//		MX_TIM4_Init();
	//	MX_TIM12_Init();
	MX_TIM6_Init();
	LOG("Initialize all configured peripherals OK!\r\n");

#if 1
	/* 初始化跟LAN9255的通信协议 */
	EtherCatPDOH.devName = "HMD500";
	EtherCatPDOH.port = &huart1;
//	EtherCatPDOH.RecvBuf = BPRecvData;
	EtherCatPDOH.read_LAN9255State_interval = 1500;
	EtherCatPDOH.INData_Para = &INDataValue;
	EtherCatPDOH.OUTData_Para = &OUTDataValue;
	EtherCatPDO_Init(&EtherCatPDOH);
#endif

#if 0
	/* 设置BackPanelTrans的相关参数 */
	BackPanelH.port = &huart1;
	BackPanelH.BoardID = getBoardID();
//	BackPanelH.RecvBuf = BPRecvData;
//	BackPanelH.RecvBufSize = sizeof(BPRecvData) / sizeof(BPRecvData[0]);
//	BackPanelH.SendBuf = BPSendData;
//	BackPanelH.SendBufSize = sizeof(BPSendData) / sizeof(BPSendData[0]);
	
	BackPanelProtocol_Init(&BackPanelH);
#endif

	/* 初始化eventos系统:状态机系统 */
    if (SysTick_Config(SystemCoreClock / 1000) != 0) {
		LOG("eos system core clock error!!!!!\r\n");
	    while (1);
	}
    
	eos_init();                                     // EventOS初始化
#if (EOS_USE_PUB_SUB != 0)
    eos_sub_init(eos_sub_table, Event_Max);         // 订阅表初始化
#endif

#if (EOS_USE_SM_MODE != 0)
    eos_sm_led_init();                              // LED状态机初始化
	eos_sm_printlog_init();							// 打印log信息的状态机初始化
#endif

    eos_run();                                      // EventOS启动
	
	while (1) {
		LOG("main loop\r\n");
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
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 25;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

#ifdef UartPrintf
PUTCHAR_PROTOTYPE
{
	/* e.g. write a character to the USART1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 1);
	return ch;
}
#endif

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM2 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
uint32_t tim6_cache = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2)
	{
		eos_tick();		//eventos系统的嘀嗒时间
		HAL_IncTick();
		tim6_cache++;

	} else if (htim->Instance == TIM6) {
		TIM6tick_2us++;
	}

}

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
