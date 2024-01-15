
#include "MSP_DAC.h"

DAC_HandleTypeDef hdac1;
DAC_HandleTypeDef hdac2;
DAC_HandleTypeDef hdac3;
DAC_HandleTypeDef hdac4;
OPAMP_HandleTypeDef hopamp1;
OPAMP_HandleTypeDef hopamp4;

/**
 * @brief DAC1 Initialization Function
 * @param None
 * @retval None
 */
void MX_DAC1_Init(void)
{

	/* USER CODE BEGIN DAC1_Init 0 */

	/* USER CODE END DAC1_Init 0 */

	DAC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN DAC1_Init 1 */

	/* USER CODE END DAC1_Init 1 */

	/** DAC Initialization
	 */
	hdac1.Instance = DAC1;
	if (HAL_DAC_Init(&hdac1) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* DAC1 channel OUT1 calibration */
	if (HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT2 config
	 */
	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	/* DAC1 channel OUT2 calibration */
	if (HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DAC1_Init 2 */

	/* USER CODE END DAC1_Init 2 */
}

/**
 * @brief DAC2 Initialization Function
 * @param None
 * @retval None
 */
void MX_DAC2_Init(void)
{

	/* USER CODE BEGIN DAC2_Init 0 */

	/* USER CODE END DAC2_Init 0 */

	DAC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN DAC2_Init 1 */

	/* USER CODE END DAC2_Init 1 */

	/** DAC Initialization
	 */
	hdac2.Instance = DAC2;
	if (HAL_DAC_Init(&hdac2) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac2, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* DAC2 channel OUT1 calibration */
	if (HAL_DACEx_SelfCalibrate(&hdac2, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DAC2_Init 2 */

	/* USER CODE END DAC2_Init 2 */
}

/**
 * @brief DAC3 Initialization Function
 * @param None
 * @retval None
 */
void MX_DAC3_Init(void)
{

	/* USER CODE BEGIN DAC3_Init 0 */

	/* USER CODE END DAC3_Init 0 */

	DAC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN DAC3_Init 1 */

	/* USER CODE END DAC3_Init 1 */

	/** DAC Initialization
	 */
	hdac3.Instance = DAC3;
	if (HAL_DAC_Init(&hdac3) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_INTERNAL;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac3, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* DAC3 channel OUT1 calibration */
	if (HAL_DACEx_SelfCalibrate(&hdac3, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DAC3_Init 2 */

	/* USER CODE END DAC3_Init 2 */
}

/**
 * @brief DAC4 Initialization Function
 * @param None
 * @retval None
 */
void MX_DAC4_Init(void)
{

	/* USER CODE BEGIN DAC4_Init 0 */

	/* USER CODE END DAC4_Init 0 */

	DAC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN DAC4_Init 1 */

	/* USER CODE END DAC4_Init 1 */

	/** DAC Initialization
	 */
	hdac4.Instance = DAC4;
	if (HAL_DAC_Init(&hdac4) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_INTERNAL;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac4, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* DAC4 channel OUT1 calibration */
	if (HAL_DACEx_SelfCalibrate(&hdac4, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DAC4_Init 2 */

	/* USER CODE END DAC4_Init 2 */
}

/**
 * @brief DAC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hdac: DAC handle pointer
 * @retval None
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (hdac->Instance == DAC1)
	{
		/* USER CODE BEGIN DAC1_MspInit 0 */

		/* USER CODE END DAC1_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_DAC1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**DAC1 GPIO Configuration
		PA4     ------> DAC1_OUT1
		PA5     ------> DAC1_OUT2
		*/
		GPIO_InitStruct.Pin = DCM1_Ictrl_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DCM1_Ictrl_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = DCM2_Ictrl_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DCM2_Ictrl_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN DAC1_MspInit 1 */

		/* USER CODE END DAC1_MspInit 1 */
	}
	else if (hdac->Instance == DAC2)
	{
		/* USER CODE BEGIN DAC2_MspInit 0 */

		/* USER CODE END DAC2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_DAC2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**DAC2 GPIO Configuration
		PA6     ------> DAC2_OUT1
		*/
		GPIO_InitStruct.Pin = DCM3_Ictrl_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DCM3_Ictrl_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN DAC2_MspInit 1 */

		/* USER CODE END DAC2_MspInit 1 */
	}
	else if (hdac->Instance == DAC3)
	{
		/* USER CODE BEGIN DAC3_MspInit 0 */

		/* USER CODE END DAC3_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_DAC3_CLK_ENABLE();

		/* USER CODE BEGIN DAC3_MspInit 1 */

		/* USER CODE END DAC3_MspInit 1 */
	}
	else if (hdac->Instance == DAC4)
	{
		/* USER CODE BEGIN DAC4_MspInit 0 */

		/* USER CODE END DAC4_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_DAC4_CLK_ENABLE();
		/* USER CODE BEGIN DAC4_MspInit 1 */

		/* USER CODE END DAC4_MspInit 1 */
	}
}

/**
 * @brief DAC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hdac: DAC handle pointer
 * @retval None
 */
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
	if (hdac->Instance == DAC1)
	{
		/* USER CODE BEGIN DAC1_MspDeInit 0 */

		/* USER CODE END DAC1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_DAC1_CLK_DISABLE();

		/**DAC1 GPIO Configuration
		PA4     ------> DAC1_OUT1
		PA5     ------> DAC1_OUT2
		*/
		HAL_GPIO_DeInit(DCM1_Ictrl_Port, DCM1_Ictrl_Pin);
		HAL_GPIO_DeInit(DCM2_Ictrl_Port, DCM2_Ictrl_Pin);

		/* USER CODE BEGIN DAC1_MspDeInit 1 */

		/* USER CODE END DAC1_MspDeInit 1 */
	}
	else if (hdac->Instance == DAC2)
	{
		/* USER CODE BEGIN DAC2_MspDeInit 0 */

		/* USER CODE END DAC2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_DAC2_CLK_DISABLE();

		/**DAC2 GPIO Configuration
		PA6     ------> DAC2_OUT1
		*/
		HAL_GPIO_DeInit(DCM3_Ictrl_Port, DCM3_Ictrl_Pin);
		/* USER CODE BEGIN DAC2_MspDeInit 1 */

		/* USER CODE END DAC2_MspDeInit 1 */
	}
	else if (hdac->Instance == DAC3)
	{
		/* USER CODE BEGIN DAC3_MspDeInit 0 */

		/* USER CODE END DAC3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_DAC3_CLK_DISABLE();

		/* USER CODE BEGIN DAC3_MspDeInit 1 */

		/* USER CODE END DAC3_MspDeInit 1 */
	}
	else if (hdac->Instance == DAC4)
	{
		/* USER CODE BEGIN DAC4_MspDeInit 0 */

		/* USER CODE END DAC4_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_DAC4_CLK_DISABLE();
		/* USER CODE BEGIN DAC4_MspDeInit 1 */

		/* USER CODE END DAC4_MspDeInit 1 */
	}
}

/**
 * @brief OPAMP1 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP1_Init(void)
{

	/* USER CODE BEGIN OPAMP1_Init 0 */

	/* USER CODE END OPAMP1_Init 0 */

	/* USER CODE BEGIN OPAMP1_Init 1 */

	/* USER CODE END OPAMP1_Init 1 */
	hopamp1.Instance = OPAMP1;
	hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
	hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
	hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC;
	hopamp1.Init.InternalOutput = DISABLE;
	hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
	hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
	if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN OPAMP1_Init 2 */

	/* USER CODE END OPAMP1_Init 2 */
}

/**
 * @brief OPAMP4 Initialization Function
 * @param None
 * @retval None
 */
void MX_OPAMP4_Init(void)
{

	/* USER CODE BEGIN OPAMP4_Init 0 */

	/* USER CODE END OPAMP4_Init 0 */

	/* USER CODE BEGIN OPAMP4_Init 1 */

	/* USER CODE END OPAMP4_Init 1 */
	hopamp4.Instance = OPAMP4;
	hopamp4.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
	hopamp4.Init.Mode = OPAMP_FOLLOWER_MODE;
	hopamp4.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC;
	hopamp4.Init.InternalOutput = DISABLE;
	hopamp4.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
	hopamp4.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
	if (HAL_OPAMP_Init(&hopamp4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN OPAMP4_Init 2 */

	/* USER CODE END OPAMP4_Init 2 */
}

/**
 * @brief OPAMP MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hopamp: OPAMP handle pointer
 * @retval None
 */
void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef *hopamp)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (hopamp->Instance == OPAMP1)
	{
		/* USER CODE BEGIN OPAMP1_MspInit 0 */

		/* USER CODE END OPAMP1_MspInit 0 */

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**OPAMP1 GPIO Configuration
		PA2     ------> OPAMP1_VOUT
		*/
		GPIO_InitStruct.Pin = DCM4_Ictrl_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DCM4_Ictrl_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN OPAMP1_MspInit 1 */

		/* USER CODE END OPAMP1_MspInit 1 */
	}
	else if (hopamp->Instance == OPAMP4)
	{
		/* USER CODE BEGIN OPAMP4_MspInit 0 */

		/* USER CODE END OPAMP4_MspInit 0 */

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**OPAMP4 GPIO Configuration
		PB12     ------> OPAMP4_VOUT
		*/
		GPIO_InitStruct.Pin = DCM5_Ictrl_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DCM5_Ictrl_Port, &GPIO_InitStruct);

		/* USER CODE BEGIN OPAMP4_MspInit 1 */

		/* USER CODE END OPAMP4_MspInit 1 */
	}
}

/**
 * @brief OPAMP MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hopamp: OPAMP handle pointer
 * @retval None
 */
void HAL_OPAMP_MspDeInit(OPAMP_HandleTypeDef *hopamp)
{
	if (hopamp->Instance == OPAMP1)
	{
		/* USER CODE BEGIN OPAMP1_MspDeInit 0 */

		/* USER CODE END OPAMP1_MspDeInit 0 */

		/**OPAMP1 GPIO Configuration
		PA2     ------> OPAMP1_VOUT
		*/
		HAL_GPIO_DeInit(DCM4_Ictrl_Port, DCM4_Ictrl_Pin);

		/* USER CODE BEGIN OPAMP1_MspDeInit 1 */

		/* USER CODE END OPAMP1_MspDeInit 1 */
	}
	else if (hopamp->Instance == OPAMP4)
	{
		/* USER CODE BEGIN OPAMP4_MspDeInit 0 */

		/* USER CODE END OPAMP4_MspDeInit 0 */

		/**OPAMP4 GPIO Configuration
		PB12     ------> OPAMP4_VOUT
		*/
		HAL_GPIO_DeInit(DCM5_Ictrl_Port, DCM5_Ictrl_Pin);

		/* USER CODE BEGIN OPAMP4_MspDeInit 1 */

		/* USER CODE END OPAMP4_MspDeInit 1 */
	}
}
