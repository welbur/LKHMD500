#include "MSP_FMAC.h"

FMAC_HandleTypeDef hfmac;

/* FMAC configuration structure */
FMAC_FilterConfigTypeDef sFmacConfig;


/* Array of filter coefficients A (feedback coefficients) in Q1.15 format */
static int16_t aFilterCoeffA[FMAC_COEFF_VECTOR_A_SIZE] = {A1,A2,A3};

/* Array of filter coefficients B (feed-forward taps) in Q1.15 format */
static int16_t aFilterCoeffB[FMAC_COEFF_VECTOR_B_SIZE] = {-B0,-B1,-B2,-B3};

/* Array of output data to preload in Q1.15 format */
static int16_t aOutputDataToPreload[FMAC_COEFF_VECTOR_A_SIZE] = {0x0000, 0x0000, 0x0000};

/* Expected number of calculated samples */
uint16_t ExpectedCalculatedOutputSize = (uint16_t) 1;
uint32_t *Fmac_Wdata;
int16_t Fmac_output;


/**
 * @brief FMAC Initialization Function
 * @param None
 * @retval None
 */
void MX_FMAC_Init(void)
{

	hfmac.Instance = FMAC;
	if (HAL_FMAC_Init(&hfmac) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief FMAC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hfmac: FMAC handle pointer
 * @retval None
 */
void HAL_FMAC_MspInit(FMAC_HandleTypeDef *hfmac)
{
	if (hfmac->Instance == FMAC)
	{
		/* USER CODE BEGIN FMAC_MspInit 0 */

		/* USER CODE END FMAC_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_FMAC_CLK_ENABLE();
		/* FMAC interrupt Init */
		HAL_NVIC_SetPriority(FMAC_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(FMAC_IRQn);
		/* USER CODE BEGIN FMAC_MspInit 1 */

		/* USER CODE END FMAC_MspInit 1 */
	}
}

/**
 * @brief FMAC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hfmac: FMAC handle pointer
 * @retval None
 */
void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef *hfmac)
{
	if (hfmac->Instance == FMAC)
	{
		/* USER CODE BEGIN FMAC_MspDeInit 0 */

		/* USER CODE END FMAC_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_FMAC_CLK_DISABLE();

		/* FMAC interrupt DeInit */
		HAL_NVIC_DisableIRQ(FMAC_IRQn);
		/* USER CODE BEGIN FMAC_MspDeInit 1 */

		/* USER CODE END FMAC_MspDeInit 1 */
	}
}

void VT_FMAC_Config(void)
{
	/*## Configure the FMAC peripheral ###########################################*/
	sFmacConfig.InputBaseAddress = FMAC_INPUT_BUFFER_BASE;
	sFmacConfig.InputBufferSize = FMAC_INPUT_BUFFER_SIZE;
	sFmacConfig.InputThreshold = FMAC_INPUT_THRESHOLD;
	sFmacConfig.CoeffBaseAddress = FMAC_COEFFICIENT_BUFFER_BASE;
	sFmacConfig.CoeffBufferSize = FMAC_COEFFICIENT_BUFFER_SIZE;
	sFmacConfig.OutputBaseAddress = FMAC_OUTPUT_BUFFER_BASE;
	sFmacConfig.OutputBufferSize = FMAC_OUTPUT_BUFFER_SIZE;
	sFmacConfig.OutputThreshold = FMAC_OUTPUT_THRESHOLD;
	sFmacConfig.pCoeffA = aFilterCoeffA;
	sFmacConfig.CoeffASize = FMAC_COEFF_VECTOR_A_SIZE;
	sFmacConfig.pCoeffB = aFilterCoeffB;
	sFmacConfig.CoeffBSize = FMAC_COEFF_VECTOR_B_SIZE;
	sFmacConfig.Filter = FMAC_FUNC_IIR_DIRECT_FORM_1;
	sFmacConfig.InputAccess = FMAC_BUFFER_ACCESS_NONE;
	sFmacConfig.OutputAccess = FMAC_BUFFER_ACCESS_IT;
	sFmacConfig.Clip = FMAC_CLIP_ENABLED;
	sFmacConfig.P = FMAC_COEFF_VECTOR_B_SIZE;
	sFmacConfig.Q = FMAC_COEFF_VECTOR_A_SIZE;
	sFmacConfig.R = KC_shift;

	if (HAL_FMAC_FilterConfig(&hfmac, &sFmacConfig) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}

	/*## Preload the input and output buffers ####################################*/
	if (HAL_FMAC_FilterPreload(&hfmac, NULL, FMAC_INPUT_BUFFER_SIZE,
							   aOutputDataToPreload, FMAC_COEFF_VECTOR_A_SIZE) != HAL_OK)
	{
		/* Configuration Error */
		Error_Handler();
	}
	/* Start calculation of IIR filter */
	if (HAL_FMAC_FilterStart(&hfmac, &Fmac_output, &ExpectedCalculatedOutputSize) != HAL_OK)
	{
		/* Processing Error */
		Error_Handler();
	}

	/* Perform an ADC automatic self-calibration and enable ADC */
//	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

	/* Start ADC and DMA */
	Fmac_Wdata = (uint32_t *)FMAC_WDATA;
//	HAL_ADC_Start_DMA(&hadc1, Fmac_Wdata, 1);
}
