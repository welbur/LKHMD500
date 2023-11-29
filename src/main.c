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
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "stdarg.h"

#include "eventos.h"   // EventOS Nano头文件
#include "event_def.h" // 事件主题的枚举
#include "eos_event.h" // LED灯闪烁状态机


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* 用于打印LOG */
uint8_t LOGTxBuffer[APP_TX_DATA_SIZE];

#define IWDG_WINDOW IWDG_WINDOW_DISABLE
#define IWDG_RELOAD 4095			//(uwLsiFreq / 32)

IWDG_HandleTypeDef hiwdg;
uint32_t IwdgStatus = 0;
uint32_t uwLsiFreq;

#if (EOS_USE_PUB_SUB != 0)
static eos_u32_t eos_sub_table[Event_Max]; // 订阅表数据空间
#endif

uint16_t DeviceStateInfo = DeviceState_Default;

uint32_t TransLastTime = 0;
uint32_t BPTrans_period = 10;

/* 与LAN9255通信协议的相关参数 */
EPDOHandler_t EtherCatPDOH;
INDATAValueHandle INDataValue;
OUTDATAValueHandle OUTDataValue;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_IWDG_Init(void);
// static void GetLSIFrequency(void);

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

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_CRC_Init();

	MX_TIM6_Init();
	MX_TIM7_Init();

	MX_USB_Device_Init();
	
	/*##-1- Check if the system has resumed from IWDG reset ####################*/
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != 0x00u)
	{
		/* IWDGRST flag set: Turn LED2 on and set IwdgStatus */
		IwdgStatus = 1;
		WorkLed(1);
		/* Insert 4s delay */
		HAL_Delay(3000);
		/* Notification done: Turn LED2 off */
		WorkLed(0);
	}

	/* Clear reset flags in any cases */
	__HAL_RCC_CLEAR_RESET_FLAGS();
	IwdgStatus = 0;

	/*##-2- Configure & Start the IWDG peripheral #########################################*/
	MX_IWDG_Init();


	/* Initialize all configured peripherals */
	
	HAL_Delay(2000); // 需要等待一会再初始化串口，要不刚上电的时候，串口无法正常初始化----原因未知，待排查
	MX_USART1_UART_Init();

	MX_USART3_UART_Init();

	MX_DAC1_Init();
	MX_DAC2_Init();
	MX_DAC3_Init();
	MX_DAC4_Init();
	MX_OPAMP1_Init();
	MX_OPAMP4_Init();

#ifndef DEVBoardG4
	MX_ADC1_Init();
#endif
	MX_ADC2_Init();
	MX_ADC3_Init();
	MX_ADC4_Init();
	MX_ADC5_Init();

	//MX_USB_Device_Init();
	LOG("Initialize all configured peripherals OK!\r\n");


	/* 初始化恒流模块相关参数 */
	dcm_init();

#if 1
	/*1、 第一步，先添加属性表，主要是OBJ 和 body的主要参数*/
	// 输入属性表，Record类型，一个obj可以对应多个body
	EtherCatPDO_AddOBJ(INDataValue.INData_DCModule_Curr, TYPE_FLOAT, INDIR, DCModuleNum, "DCM Curr\0");
	EtherCatPDO_AddOBJ(INDataValue.INData_DCMState, TYPE_UINT16, INDIR, DCModuleNum, "DCM State\0");

	// 输出属性表，variable类型，一个obj对应一个body
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_Enable[0], TYPE_UINT8, OUTDIR, 1, "DCM_1 Active\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_Enable[1], TYPE_UINT8, OUTDIR, 1, "DCM_2 Active\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_Enable[2], TYPE_UINT8, OUTDIR, 1, "DCM_3 Active\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_Enable[3], TYPE_UINT8, OUTDIR, 1, "DCM_4 Active\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_Enable[4], TYPE_UINT8, OUTDIR, 1, "DCM_5 Active\0");

	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_SetCurr[0], TYPE_FLOAT, OUTDIR, 1, "Set DCM_1 Curr\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_SetCurr[1], TYPE_FLOAT, OUTDIR, 1, "Set DCM_2 Curr\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_SetCurr[2], TYPE_FLOAT, OUTDIR, 1, "Set DCM_3 Curr\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_SetCurr[3], TYPE_FLOAT, OUTDIR, 1, "Set DCM_4 Curr\0");
	EtherCatPDO_AddOBJ(&OUTDataValue.OUTData_DCModule_SetCurr[4], TYPE_FLOAT, OUTDIR, 1, "Set DCM_5 Curr\0");

	/*2、 第二步，初始化跟LAN9255的通信协议 */
	EtherCatPDOH.devName = "HMD500";
	EtherCatPDOH.port = &huart1;
	EtherCatPDOH.read_LAN9255State_interval = 1500;

	EtherCatPDO_Init(&EtherCatPDOH);
#endif


	/* 初始化eventos系统:状态机系统 */
	if (SysTick_Config(SystemCoreClock / 1000) != 0)
	{
		LOG("eos system core clock error!!!!!\r\n");
		while (1)
			;
	}

	eos_init(); // EventOS初始化
#if (EOS_USE_PUB_SUB != 0)
	eos_sub_init(eos_sub_table, Event_Max); // 订阅表初始化
#endif

#if (EOS_USE_SM_MODE != 0)
	eos_sm_led_init();		   // LED状态机初始化(打印LOG，需要测试某功能的时候也在这个状态中添加)
	eos_sm_EtherCatPDO_init(); // EtherCatPDO状态机初始化
	eos_sm_HMD500_init();	   // HMD500状态机初始化
#endif

	eos_run(); // EventOS启动

	while (1)
	{
		//LOG("main loop\r\n");
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
	RCC_OscInitStruct.PLL.PLLN = 25;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
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

#ifdef USBPrintf
void usb_printf(const char *fmt, ...)
{
	va_list args;
	uint16_t length;
	//uint8_t buff[APP_TX_DATA_SIZE];

	va_start(args, fmt);
	length = vsnprintf((char *)LOGTxBuffer, APP_TX_DATA_SIZE, fmt, args);
	va_end(args);

	uint32_t CDC_Trans_TS = HAL_GetTick();
	while (CDC_Transmit_FS(LOGTxBuffer, length) == USBD_BUSY)
	{
		if (HAL_GetTick() - CDC_Trans_TS > 10)
			break;
	}
}
#endif

/**
 * @brief IWDG Initialization Function
 * @param None
 * @retval None
 */
static void MX_IWDG_Init(void)
{

	/* USER CODE BEGIN IWDG_Init 0 */

	/* USER CODE END IWDG_Init 0 */

	/* USER CODE BEGIN IWDG_Init 1 */

	/* USER CODE END IWDG_Init 1 */
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
	hiwdg.Init.Window = IWDG_WINDOW;
	hiwdg.Init.Reload = IWDG_RELOAD;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN IWDG_Init 2 */

	/* USER CODE END IWDG_Init 2 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	WorkLed(0);
	IwdgStatus = 1;
	//LOG("Error_Handle ....%d\r\n", IwdgStatus);
	__disable_irq();
	while (1)
	{
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
