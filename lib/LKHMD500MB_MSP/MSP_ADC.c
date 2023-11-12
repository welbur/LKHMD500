
#include "MSP_ADC.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
ADC_HandleTypeDef hadc4;
ADC_HandleTypeDef hadc5;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
DMA_HandleTypeDef hdma_adc3;
DMA_HandleTypeDef hdma_adc4;
DMA_HandleTypeDef hdma_adc5;

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
#ifndef DEVBoardG4
void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.GainCompensation = 0;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		printf("HAL_ADC_Init error\r\n");
		Error_Handler();
	}

	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		printf("multi mode error\r\n");
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		printf(" HAL_ADC_ConfigChannel channel 1 error\r\n");
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		printf(" HAL_ADC_ConfigChannel channel 2 error\r\n");
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */
}
#endif

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC2_Init(void)
{

	/* USER CODE BEGIN ADC2_Init 0 */

	/* USER CODE END ADC2_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC2_Init 1 */

	/* USER CODE END ADC2_Init 1 */

	/** Common config
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.GainCompensation = 0;
	hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;		//ADC_SCAN_DISABLE;		//ADC_SCAN_ENABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.ContinuousConvMode = ENABLE;	//DISABLE;		//ENABLE;
	hadc2.Init.NbrOfConversion = 2;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.DMAContinuousRequests = ENABLE;
	hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;	//ADC_OVR_DATA_PRESERVED;
	hadc2.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC2_Init 2 */

	/* USER CODE END ADC2_Init 2 */
}

/**
 * @brief ADC3 Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC3_Init(void)
{

	/* USER CODE BEGIN ADC3_Init 0 */

	/* USER CODE END ADC3_Init 0 */

	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC3_Init 1 */

	/* USER CODE END ADC3_Init 1 */

	/** Common config
	 */
	hadc3.Instance = ADC3;
	hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc3.Init.Resolution = ADC_RESOLUTION_12B;
	hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc3.Init.GainCompensation = 0;
	hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc3.Init.LowPowerAutoWait = DISABLE;
	hadc3.Init.ContinuousConvMode = ENABLE;
	hadc3.Init.NbrOfConversion = 2;
	hadc3.Init.DiscontinuousConvMode = DISABLE;
	hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc3.Init.DMAContinuousRequests = ENABLE;
	hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc3.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc3) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc3, &multimode) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC3_Init 2 */

	/* USER CODE END ADC3_Init 2 */
}

/**
 * @brief ADC4 Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC4_Init(void)
{

	/* USER CODE BEGIN ADC4_Init 0 */

	/* USER CODE END ADC4_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC4_Init 1 */

	/* USER CODE END ADC4_Init 1 */

	/** Common config
	 */
	hadc4.Instance = ADC4;
	hadc4.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc4.Init.Resolution = ADC_RESOLUTION_12B;
	hadc4.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc4.Init.GainCompensation = 0;
	hadc4.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc4.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc4.Init.LowPowerAutoWait = DISABLE;
	hadc4.Init.ContinuousConvMode = ENABLE;
	hadc4.Init.NbrOfConversion = 2;
	hadc4.Init.DiscontinuousConvMode = DISABLE;
	hadc4.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc4.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc4.Init.DMAContinuousRequests = ENABLE;
	hadc4.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc4.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc4) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc4, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc4, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC4_Init 2 */

	/* USER CODE END ADC4_Init 2 */
}

/**
 * @brief ADC5 Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC5_Init(void)
{

	/* USER CODE BEGIN ADC5_Init 0 */

	/* USER CODE END ADC5_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC5_Init 1 */

	/* USER CODE END ADC5_Init 1 */

	/** Common config
	 */
	hadc5.Instance = ADC5;
	hadc5.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc5.Init.Resolution = ADC_RESOLUTION_12B;
	hadc5.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc5.Init.GainCompensation = 0;
	hadc5.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc5.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc5.Init.LowPowerAutoWait = DISABLE;
	hadc5.Init.ContinuousConvMode = ENABLE;
	hadc5.Init.NbrOfConversion = 2;
	hadc5.Init.DiscontinuousConvMode = DISABLE;
	hadc5.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc5.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc5.Init.DMAContinuousRequests = ENABLE;
	hadc5.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc5.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc5) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc5, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc5, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC5_Init 2 */

	/* USER CODE END ADC5_Init 2 */
}

static uint32_t HAL_RCC_ADC12_CLK_ENABLED = 0;
static uint32_t HAL_RCC_ADC345_CLK_ENABLED = 0;
/**
 * @brief ADC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

#ifdef DEVBoardG4
	if (hadc->Instance == ADC1)
	{
		LOG("no adc1\r\n");
	}
#else
	if (hadc->Instance == ADC1)
	{
		/* USER CODE BEGIN ADC1_MspInit 0 */

		/* USER CODE END ADC1_MspInit 0 */

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
		PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* Peripheral clock enable */
		HAL_RCC_ADC12_CLK_ENABLED++;
		if (HAL_RCC_ADC12_CLK_ENABLED == 1)
		{
			__HAL_RCC_ADC12_CLK_ENABLE();
		}

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
		PA0     ------> ADC1_IN1
		PA1     ------> ADC1_IN2
		*/
		GPIO_InitStruct.Pin = DCM2_Volt_Pin | DCM2_Curr_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC1 DMA Init */
		/* ADC1 Init */
		hdma_adc1.Instance = DMA1_Channel1;
		hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
		if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);
		/* USER CODE BEGIN ADC1_MspInit 1 */

		/* USER CODE END ADC1_MspInit 1 */
	}
#endif
	else if (hadc->Instance == ADC2)
	{
		/* USER CODE BEGIN ADC2_MspInit 0 */

		/* USER CODE END ADC2_MspInit 0 */

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
		PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* Peripheral clock enable */
		HAL_RCC_ADC12_CLK_ENABLED++;
		if (HAL_RCC_ADC12_CLK_ENABLED == 1)
		{
			__HAL_RCC_ADC12_CLK_ENABLE();
		}

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**ADC2 GPIO Configuration
		PC0     ------> ADC2_IN6
		PC1     ------> ADC2_IN7
		*/
		GPIO_InitStruct.Pin = DCM1_Volt_Pin | DCM1_Curr_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* ADC2 DMA Init */
		/* ADC2 Init */
		hdma_adc2.Instance = DMA1_Channel2;
		hdma_adc2.Init.Request = DMA_REQUEST_ADC2;
		hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc2.Init.Mode = DMA_CIRCULAR;
		hdma_adc2.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc2);
		/* USER CODE BEGIN ADC2_MspInit 1 */

		/* USER CODE END ADC2_MspInit 1 */
	}
	else if (hadc->Instance == ADC3)
	{
		/* USER CODE BEGIN ADC3_MspInit 0 */

		/* USER CODE END ADC3_MspInit 0 */

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC345;
		PeriphClkInit.Adc345ClockSelection = RCC_ADC345CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* Peripheral clock enable */
		HAL_RCC_ADC345_CLK_ENABLED++;
		if (HAL_RCC_ADC345_CLK_ENABLED == 1)
		{
			__HAL_RCC_ADC345_CLK_ENABLE();
		}

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**ADC3 GPIO Configuration
		PB0     ------> ADC3_IN12
		PB1     ------> ADC3_IN1
		*/
		GPIO_InitStruct.Pin = DCM3_Volt_Pin | DCM3_Curr_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ADC3 DMA Init */
		/* ADC3 Init */
		hdma_adc3.Instance = DMA1_Channel3;
		hdma_adc3.Init.Request = DMA_REQUEST_ADC3;
		hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc3.Init.Mode = DMA_CIRCULAR;
		hdma_adc3.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc3);
		/* USER CODE BEGIN ADC3_MspInit 1 */

		/* USER CODE END ADC3_MspInit 1 */
	}
	else if (hadc->Instance == ADC4)
	{
		/* USER CODE BEGIN ADC4_MspInit 0 */

		/* USER CODE END ADC4_MspInit 0 */

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC345;
		PeriphClkInit.Adc345ClockSelection = RCC_ADC345CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* Peripheral clock enable */
		HAL_RCC_ADC345_CLK_ENABLED++;
		if (HAL_RCC_ADC345_CLK_ENABLED == 1)
		{
			__HAL_RCC_ADC345_CLK_ENABLE();
		}

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**ADC4 GPIO Configuration
		PB14     ------> ADC4_IN4
		PB15     ------> ADC4_IN5
		*/
		GPIO_InitStruct.Pin = DCM4_Volt_Pin | DCM4_Curr_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ADC4 DMA Init */
		/* ADC4 Init */
		hdma_adc4.Instance = DMA1_Channel4;
		hdma_adc4.Init.Request = DMA_REQUEST_ADC4;
		hdma_adc4.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc4.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc4.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc4.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc4.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc4.Init.Mode = DMA_CIRCULAR;
		hdma_adc4.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_adc4) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc4);
		/* USER CODE BEGIN ADC4_MspInit 1 */

		/* USER CODE END ADC4_MspInit 1 */
	}
	else if (hadc->Instance == ADC5)
	{
		/* USER CODE BEGIN ADC5_MspInit 0 */

		/* USER CODE END ADC5_MspInit 0 */

		/** Initializes the peripherals clocks
		 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC345;
		PeriphClkInit.Adc345ClockSelection = RCC_ADC345CLKSOURCE_SYSCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* Peripheral clock enable */
		HAL_RCC_ADC345_CLK_ENABLED++;
		if (HAL_RCC_ADC345_CLK_ENABLED == 1)
		{
			__HAL_RCC_ADC345_CLK_ENABLE();
		}

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC5 GPIO Configuration
		PA8     ------> ADC5_IN1
		PA9     ------> ADC5_IN2
		*/
		GPIO_InitStruct.Pin = DCM5_Volt_Pin | DCM5_Curr_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC5 DMA Init */
		/* ADC5 Init */
		hdma_adc5.Instance = DMA1_Channel5;
		hdma_adc5.Init.Request = DMA_REQUEST_ADC5;
		hdma_adc5.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc5.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc5.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc5.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc5.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc5.Init.Mode = DMA_CIRCULAR;
		hdma_adc5.Init.Priority = DMA_PRIORITY_LOW;
		if (HAL_DMA_Init(&hdma_adc5) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc5);
		/* USER CODE BEGIN ADC5_MspInit 1 */

		/* USER CODE END ADC5_MspInit 1 */
	}
}

/**
 * @brief ADC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
#ifdef DEVBoardG4
	if (hadc->Instance == ADC1)
	{
	}
#else
	if (hadc->Instance == ADC1)
	{
		/* USER CODE BEGIN ADC1_MspDeInit 0 */

		/* USER CODE END ADC1_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_ADC12_CLK_ENABLED--;
		if (HAL_RCC_ADC12_CLK_ENABLED == 0)
		{
			__HAL_RCC_ADC12_CLK_DISABLE();
		}

		/**ADC1 GPIO Configuration
		PA0     ------> ADC1_IN1
		PA1     ------> ADC1_IN2
		*/
		HAL_GPIO_DeInit(GPIOA, DCM2_Volt_Pin | DCM2_Curr_Pin);

		/* ADC1 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
		/* USER CODE BEGIN ADC1_MspDeInit 1 */

		/* USER CODE END ADC1_MspDeInit 1 */
	}
#endif
	else if (hadc->Instance == ADC2)
	{
		/* USER CODE BEGIN ADC2_MspDeInit 0 */

		/* USER CODE END ADC2_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_ADC12_CLK_ENABLED--;
		if (HAL_RCC_ADC12_CLK_ENABLED == 0)
		{
			__HAL_RCC_ADC12_CLK_DISABLE();
		}

		/**ADC2 GPIO Configuration
		PC0     ------> ADC2_IN6
		PC1     ------> ADC2_IN7
		*/
		HAL_GPIO_DeInit(GPIOC, DCM1_Volt_Pin | DCM1_Curr_Pin);

		/* ADC2 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
		/* USER CODE BEGIN ADC2_MspDeInit 1 */

		/* USER CODE END ADC2_MspDeInit 1 */
	}
	else if (hadc->Instance == ADC3)
	{
		/* USER CODE BEGIN ADC3_MspDeInit 0 */

		/* USER CODE END ADC3_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_ADC345_CLK_ENABLED--;
		if (HAL_RCC_ADC345_CLK_ENABLED == 0)
		{
			__HAL_RCC_ADC345_CLK_DISABLE();
		}

		/**ADC3 GPIO Configuration
		PB0     ------> ADC3_IN12
		PB1     ------> ADC3_IN1
		*/
		HAL_GPIO_DeInit(GPIOB, DCM3_Volt_Pin | DCM3_Curr_Pin);

		/* ADC3 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
		/* USER CODE BEGIN ADC3_MspDeInit 1 */

		/* USER CODE END ADC3_MspDeInit 1 */
	}
	else if (hadc->Instance == ADC4)
	{
		/* USER CODE BEGIN ADC4_MspDeInit 0 */

		/* USER CODE END ADC4_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_ADC345_CLK_ENABLED--;
		if (HAL_RCC_ADC345_CLK_ENABLED == 0)
		{
			__HAL_RCC_ADC345_CLK_DISABLE();
		}

		/**ADC4 GPIO Configuration
		PB14     ------> ADC4_IN4
		PB15     ------> ADC4_IN5
		*/
		HAL_GPIO_DeInit(GPIOB, DCM4_Volt_Pin | DCM4_Curr_Pin);

		/* ADC4 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
		/* USER CODE BEGIN ADC4_MspDeInit 1 */

		/* USER CODE END ADC4_MspDeInit 1 */
	}
	else if (hadc->Instance == ADC5)
	{
		/* USER CODE BEGIN ADC5_MspDeInit 0 */

		/* USER CODE END ADC5_MspDeInit 0 */
		/* Peripheral clock disable */
		HAL_RCC_ADC345_CLK_ENABLED--;
		if (HAL_RCC_ADC345_CLK_ENABLED == 0)
		{
			__HAL_RCC_ADC345_CLK_DISABLE();
		}

		/**ADC5 GPIO Configuration
		PA8     ------> ADC5_IN1
		PA9     ------> ADC5_IN2
		*/
		HAL_GPIO_DeInit(GPIOA, DCM5_Volt_Pin | DCM5_Curr_Pin);

		/* ADC5 DMA DeInit */
		HAL_DMA_DeInit(hadc->DMA_Handle);
		/* USER CODE BEGIN ADC5_MspDeInit 1 */

		/* USER CODE END ADC5_MspDeInit 1 */
	}
}
