/**
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  * @file       BSP_GPIO.c/h
  * @brief      GPIO的二次封装
  * @note      	
  * @history
  *  Version    Date            Author          Modification
  *  V3.0.0     2020.7.14     	              	1. done
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 Boring_TECH*********************
  */
#include "MSP_GPIO.h"

HAL_TickFreqTypeDef TickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
uint32_t asyncTickstart = 0;
LedPara_TypeDef LedPara;

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
        * Free pins are configured automatically as Analog (this feature is enabled through
        * the Code Generation settings)
*/
#if DEVBoard
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level 设置LED*/ 
  HAL_GPIO_WritePin(GPIOC, RED_Pin|GREEN_Pin|BLUE_Pin, GPIO_PIN_SET);
#if 1
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
#endif

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = RED_Pin|GREEN_Pin|BLUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#if 0
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI2_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI2_CS_GPIO_Port, &GPIO_InitStruct);
#endif
}
#endif

/*
 * Sys work led
*/
void WorkLed(void)
{
  if (asyncTickstart == 0) 
  {
    asyncTickstart = HAL_GetTick();
  }
  if ((HAL_GetTick() - asyncTickstart) >= LedPara.LedBlinkFreq)
  {
    if (LedPara.LedStatus == WorkNormal) {
      LED_G_TogglePin;
    } else if (LedPara.LedStatus == WorkError) {
      LED_R_TogglePin;
    }
    asyncTickstart = HAL_GetTick();
  }
}

/**
  * @brief set work led status
  * @param ls: led work status : normal, error
  * @param n : led blink times
  * @retval None
  */
void Set_WorkLed_Status(LedStatus_TypeDef ls, uint32_t bf)
{
  //if (ls == WorkNormal) return;
  LedPara.LedStatus = ls;
  /* Add a freq to guarantee minimum wait */
  if (bf < HAL_MAX_DELAY)
  {
    bf = bf + (uint32_t)(TickFreq);
  }
  LedPara.LedBlinkFreq = bf;
}

/**
  * @brief  Configures EXTI Line0 (connected to PA0 pin) in interrupt mode
  * @param  None
  * @retval None
  */
void EXTILine_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO pins : PC3 PC4 PC5 PC6 PC7 PC8 PC9 PC10
     *  PC3～PC10定义为Slave板到Master板的中断信号
    */
    /********************************     di board 1 int     *********************************/
#if 1 
    DIB_INT_PIN1_CLK_ENABLE();
    GPIO_InitStruct.Pin = DIB_INT_PIN1;                                   
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DIB_INT_PIN1_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DIB_INT_PIN1_EXTI_IRQn, GPIO_EXTI_PP, DIB_INT_PIN1_EXTI_SP);
    HAL_NVIC_EnableIRQ(DIB_INT_PIN1_EXTI_IRQn);
#endif
    /********************************     di board 2 int     *********************************/
    DIB_INT_PIN2_CLK_ENABLE();
    GPIO_InitStruct.Pin = DIB_INT_PIN2;                                   
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DIB_INT_PIN2_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DIB_INT_PIN2_EXTI_IRQn, GPIO_EXTI_PP, DIB_INT_PIN2_EXTI_SP);
    HAL_NVIC_EnableIRQ(DIB_INT_PIN2_EXTI_IRQn);
    /********************************     di board 3 int     *********************************/
    DIB_INT_PIN3_CLK_ENABLE();
    GPIO_InitStruct.Pin = DIB_INT_PIN3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DIB_INT_PIN3_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DIB_INT_PIN3_EXTI_IRQn, GPIO_EXTI_PP, DIB_INT_PIN3_EXTI_SP);
    HAL_NVIC_EnableIRQ(DIB_INT_PIN3_EXTI_IRQn);
    /********************************     di board 4 int     *********************************/
    DIB_INT_PIN4_CLK_ENABLE();
    GPIO_InitStruct.Pin = DIB_INT_PIN4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DIB_INT_PIN4_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DIB_INT_PIN4_EXTI_IRQn, GPIO_EXTI_PP, DIB_INT_PIN4_EXTI_SP);
    HAL_NVIC_EnableIRQ(DIB_INT_PIN4_EXTI_IRQn);
    /********************************     dq board 1 int     *********************************/
    DQB_INT_PIN1_CLK_ENABLE();
    GPIO_InitStruct.Pin = DQB_INT_PIN1;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DQB_INT_PIN1_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DQB_INT_PIN1_EXTI_IRQn, GPIO_EXTI_PP, DQB_INT_PIN1_EXTI_SP);
    HAL_NVIC_EnableIRQ(DQB_INT_PIN1_EXTI_IRQn);
    /********************************     dq board 2 int     *********************************/
    DQB_INT_PIN2_CLK_ENABLE();
    GPIO_InitStruct.Pin = DQB_INT_PIN2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DQB_INT_PIN2_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(DQB_INT_PIN2_EXTI_IRQn, GPIO_EXTI_PP, DQB_INT_PIN2_EXTI_SP);
    HAL_NVIC_EnableIRQ(DQB_INT_PIN2_EXTI_IRQn);
    /********************************     rs485 board int     *********************************/
    RS485B_INT_PIN_CLK_ENABLE();
    GPIO_InitStruct.Pin = RS485B_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(RS485B_INT_PIN_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(RS485B_INT_PIN_EXTI_IRQn, GPIO_EXTI_PP, RS485B_INT_PIN_EXTI_SP);
    HAL_NVIC_EnableIRQ(RS485B_INT_PIN_EXTI_IRQn);
    /********************************     menu board int     *********************************/
    MENUB_INT_PIN_CLK_ENABLE();
    GPIO_InitStruct.Pin = MENUB_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MENUB_INT_PIN_PORT, &GPIO_InitStruct);
    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(MENUB_INT_PIN_EXTI_IRQn, GPIO_EXTI_PP, MENUB_INT_PIN_EXTI_SP);
    HAL_NVIC_EnableIRQ(MENUB_INT_PIN_EXTI_IRQn);
}


#if 0
/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_KEY
  * @param  ButtonMode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability  
  */
void BSP_PB_Init(UsedGPIO_TypeDef UsedGPIO, UsedGPIOMode_TypeDef UsedGPIOMode)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Enable the BUTTON Clock */
  USEDGPIOx_CLK_ENABLE(UsedGPIO);
  
  if(UsedGPIOMode == GPIO_MODE)
  {
    /* Configure Button pin as input */
    GPIO_InitStruct.Pin = USEDGPIO_PIN[UsedGPIO];
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(USEDGPIO_PORT[UsedGPIO], &GPIO_InitStruct);
  }
  
  if(UsedGPIOMode == EXTI_MODE)
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_InitStruct.Pin = USEDGPIO_PIN[UsedGPIO];
    GPIO_InitStruct.Pull = GPIO_PULLUP; //GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; 
    HAL_GPIO_Init(USEDGPIO_PORT[UsedGPIO], &GPIO_InitStruct);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(USEDGPIO_IRQn[UsedGPIO]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(USEDGPIO_IRQn[UsedGPIO]));
    printf("exit pin : %d\r\n", USEDGPIO_PIN[UsedGPIO]);
  }
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *   This parameter should be: BUTTON_KEY
  * @note PB DeInit does not disable the GPIO clock
  */
void BSP_PB_DeInit(UsedGPIO_TypeDef UsedGPIO)
{
    GPIO_InitTypeDef gpio_init_structure;

    gpio_init_structure.Pin = USEDGPIO_PIN[UsedGPIO];
    HAL_NVIC_DisableIRQ((IRQn_Type)(USEDGPIO_IRQn[UsedGPIO]));
    HAL_GPIO_DeInit(USEDGPIO_PORT[UsedGPIO], gpio_init_structure.Pin);
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter should be: BUTTON_KEY  
  * @retval The Button GPIO pin value.
  */
uint32_t BSP_PB_GetState(UsedGPIO_TypeDef UsedGPIO)
{
  return HAL_GPIO_ReadPin(USEDGPIO_PORT[UsedGPIO], USEDGPIO_PIN[UsedGPIO]);
}
#endif
//配置文件在.h文件中


