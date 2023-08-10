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
void osPrintLOG(void *argument);
void transSlaveBoardInfo(void *argument);
void Start_PowerMonitorTask(void *argument);	//读取电量计量的数据
void Start_Slave_BackPanelTransTask(void *argument);
void Start_ReadMasterBDataTask(void *argument);
//void Start_SPI2_TransTask(void *argument);
//void Start_SPI2_WriteDataTask(void *argument);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
void BackPanelTrans_Init(void);
void PowerMonitor_Init(void);




/* 生成随机浮点数，用于测试*/
float randFloat(float min, float max) {
	//srand(time(0));
    return min + (max - min) * ((float)rand() / RAND_MAX);
}


/* 依次读取结构体中的所有元素 */
void structTOArray(PowerMonitorData_t *PMdata, uint8_t PMdataLen, uint8_t *Arr) {
    float *ptr = &PMdata->Vrms;
    for (int i = 0; i < PMdataLen; i++) 
	{
		Arr[i*2] 		= (uint8_t)*(ptr + i);	//(uint8_t)PowerMonitorData[i].Vrms;
    	Arr[i*2+1] 	= (uint8_t)((*(ptr + i) - Arr[i*2]) * 100);
    }
}

/* 等待spi2的 cs信号的值 */
uint8_t waitSPI2CSequal(uint8_t cs_v, uint32_t timeout) 
{ 
	uint32_t txTickstart = xTaskGetTickCount();
	#if (cs_v == 0)
	while (MSP_SPI2_CS_STATUS()) 
	#else
	while (!MSP_SPI2_CS_STATUS()) 
	#endif
	{
		if (xTaskGetTickCount() - txTickstart > timeout)
		{
			LOG("wait CS = %d timeout.....%ld\r\n", cs_v, timeout);
			return 0;
		} 
	}
	return 1;
}

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	mutex = osMutexNew(NULL);
	// 创建事件标志
  	//transSlaveBoardInfo_flagsID = osEventFlagsNew(NULL);
	transSlaveBoardInfo_TaskHandle = osThreadNew(transSlaveBoardInfo, NULL, &transSlaveBoardInfoTask_attributes);

	/* creation of work led Task */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
	

//	osPrintLOG_TaskHandle = osThreadNew(osPrintLOG, NULL, &osPrintLOGTask_attributes);
}

void BackPanelTrans_Init(void)
{
//	sbINFOH.boardID = Get_BoardID();
	LOG("this slave board id is : %d\r\n", sbINFOH.boardID);
	BackPanelTransHandler_t BackPanelTransHandler;
	//BackPanelTransHandler.spiPort 			= hspi2;
	BackPanelTransHandler.slaveBoardID		= sbINFOH.boardID;
	BackPanelTransHandler.printDebug 		= 1;
	BackPanelTransHandler.spiITTimeOut		= 10;
	BackPanelTransHandler.SYNCTimeOut		= 10;
	BackPanelTransHandler.readACKTimeOut 	= 10;
	BackPanelTransHandler.readDataTimeOut 	= 100;
	BackPanelTransHandler.waitCSTimeOut 	= 10;
	//PacketInit();
	CRC_Init(0x9B);
	BPTrans_Init(&BackPanelTransHandler);

	ReadMasterBDataTaskHandle = osThreadNew(Start_Slave_BackPanelTransTask, NULL, &ReadMasterBDataTask_attributes);

#if 0
	//SPI2_TransTaskHandle = osThreadNew(Start_SPI2_TransTask, NULL, &SPI2_TransTask_attributes);
//	CHIPS_SPI1TransTaskHandle = osThreadNew(Start_CHIPS_SPI1TransTask, NULL, &CHIPS_SPI1TransTask_attributes);
	ReadMasterBDataTaskHandle = osThreadNew(Start_ReadMasterBDataTask, NULL, &ReadMasterBDataTask_attributes);
#endif
}

void PowerMonitor_Init(void)
{
	sbINFOH.SlaveTranstoMasterMode = SlaveTranstoMasterMode_Trigger;
	ACS37800_t acs37800;
	acs37800.spiPort 				= &hspi1;
	acs37800.printDebug 			= 1;
	acs37800.CurrentCoarseGain 		= 1;
	acs37800.CurrentSensingRange 	= 30.0;	// 电流检测范围 0～30A
	acs37800.DividerResistance 		= 4000000;	//检测电压用，分压电阻的阻值，默认2M
	acs37800.SenseResistance		= 0;
	ACS37800_Init(&acs37800);
	// From the ACS37800 datasheet:
  	// CONFIGURING THE DEVICE FOR AC APPLICATIONS : DYNAMIC CALCULATION OF N
  	// Set bypass_n_en = 0 (default). This setting enables the device to
  	// dynamically calculate N based off the voltage zero crossings.
  	ACS37800_setBypassNenable(0, 1); // Disable bypass_n in shadow memory and eeprom

	PMDataSemaphore = osSemaphoreNew(1, 1, &PMDataSphr_attributes);
	// 等待getSlaveBoardInfo()线程结束
  	//osEventFlagsWait(transSlaveBoardInfo_flagsID, 0x0001, osFlagsWaitAny, osWaitForever);

#if 1
	PowerMonitor_TaskHandle = osThreadNew(Start_PowerMonitorTask, NULL, &PowerMonitorTask_attributes);
#endif
}

/**
 * @brief  等待主板和从板之间的第一次通信完成
 * @param  None
 * @note	刚上电时，主板和从板会先做一次通信，通信完成后，主板会知道有几块从板，分别在哪个槽位，每个从板的有效数据长度是多少
 * @retval None
 */
void waitBackPanelTaskDone(void)
{
#if 0
	// 等待getSlaveBoardInfo()线程结束
  	osEventFlagsWait(transSlaveBoardInfo_flagsID, 0x0001, osFlagsWaitAny, osWaitForever);
	LOG("os event flags wait \r\n");

	ReadMasterBDataTaskHandle = osThreadNew(Start_Slave_BackPanelTransTask, NULL, &ReadMasterBDataTask_attributes);
	PowerMonitor_TaskHandle = osThreadNew(Start_PowerMonitorTask, NULL, &PowerMonitorTask_attributes);
#endif
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
		osMutexAcquire(mutex, osWaitForever);	//打印调试信息用
		//osSemaphoreAcquire(PMDataSemaphore, osWaitForever); // 获取信号量
		//uint8_t dtt;
		//dtt = rand() % 50 + 0;
		LED_G_TogglePin;	//WorkLed_TogglePin;
		//osSemaphoreRelease(PMDataSemaphore); // 释放信号量
		osMutexRelease(mutex);
		//osDelay(5 + dtt);
		osDelay(1000);
	}
}

/**
 * @brief  将需要打印的数据都合并到一个buffer里面，然后再一起打印出来。
 * @param  argument: Not used
 * @retval None
 */
#if 0
void osPrintLOG(void *argument)
{
	for (;;)
	{
		osMutexAcquire(mutex, osWaitForever);
		if (LOG_MSG[0] != 0)
		{	
			//if (LOG_MSG >= MSG_LENGTH)
			LOG("~~~~~~~~~~RTOS Print LOG~~~~~~~~~~\r\n%s", LOG_MSG);
			LOG("\r\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
			memset(LOG_MSG, 0, MSG_LENGTH);
		}
		osMutexRelease(mutex);
		osDelay(10);
	}
}
#endif


void transSlaveBoardInfo(void *argument)
{
	//uint8_t slaveBoardID = Get_BoardID();
	uint8_t acbpDataLen = (PMFloatDataNum*2+PMIntDataNum)*ACS37800_IC_Num;
	uint8_t bpDataLen = acbpDataLen;
	osMutexAcquire(mutex, osWaitForever);
	LOG("start transfer slave board info to master board  \r\n");
	for (;;)
	{
		uint8_t wData[DefaultbpCMDLen] = {sbINFOH.boardID, bpDataLen, 0, 3};
		uint8_t rData[DefaultbpCMDLen];
		HAL_SPI_MspInit(&hspi2);
		PB2BB_INT_ToMasterB(1);
		PB2BB_INT_ToMasterB(0);
		BackPanelTransStatus_TypeDef bpstatus;
		bpstatus = BackPanelTrans_Slave_WriteAndRead(sbINFOH.boardID, wData, rData, DefaultbpCMDLen);
		LOG("out.Errcode : %d, ErrorInfo : %s\r\n", bpstatus, Error_LOG[Error_Offset+bpstatus]);
		HAL_SPI_MspDeInit(&hspi2);
		LOG("rdata : %d, %d\r\n", rData[0], rData[1]);
		LOG(".................................\r\n");
		if (rData[0] == Default_SlaveTranstoMasterMode_Loop && rData[1] == Default_SlaveTranstoMasterMode_Loop_Time)
		{
			LOG("transfer success, sleep 1s....................\r\n");
			// 设置事件标志
  			//osEventFlagsSet(transSlaveBoardInfo_flagsID, 0x0001);
			osMutexRelease(mutex);
			osThreadExit();
		}
		osDelay(500);
	}
}


/**
 * @brief  spi1为主模式，通过SPI1与di/dq/rs485芯片通讯，每个板都有8颗芯片，通过cs选择
 * @param  argument: Not used
 * @retval None
 */
void Start_PowerMonitorTask(void *argument)
{
	srand(time(0));
	/* Infinite loop */
	for (;;)
	{
	  	//osEventFlagsWait(transSlaveBoardInfo_flagsID, 0x0001, osFlagsWaitAny, osWaitForever);
		osMutexAcquire(mutex, osWaitForever);
		//uint8_t TrigFlag = 0x00;
		//LOG("start power monitor task \r\n");
		osSemaphoreAcquire(PMDataSemaphore, osWaitForever); // 获取信号量
		#ifdef randomPMData
		for (int i = 0; i < ACS37800_IC_Num; i++)
		{
			//PowerMonitorData[i].Vrms 		= randFloat(1.0f, 50.0f);		//rand() % 50.1 + 0.1;
			//LOG("random PowerMonitorData[%d].Vrms : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Vrms*100));//PowerMonitorData[i].Vrms);
			PowerMonitorData[i].Irms 		= randFloat(1.0f, 50.0f);		//rand() % 50.1 + 0.1;
			PowerMonitorData[i].Pactive 	= randFloat(1.0f, 50.0f);		//rand() % 50 + 0;
			PowerMonitorData[i].Preactive 	= randFloat(1.0f, 50.0f);		//rand() % 50 + 0;
			PowerMonitorData[i].Pfactor 	= randFloat(1.0f, 50.0f);		//rand() % 50 + 0;
			PowerMonitorData[i].PosAngle 	= rand() % 50 + 1;
			PowerMonitorData[i].ACSstatus 	= rand() % 50 + 1;
			if (sbINFOH.SlaveTranstoMasterMode == SlaveTranstoMasterMode_Trigger)
			{
				//TrigFlag = 0x00;
				if (PowerMonitorData[i].Irms < 20)	//触发条件应该在初始化的时候设置好
				{
					_TrigFlag += (1 << i);
					//LOG("i = %d, trigflag %d\r\n", i, TrigFlag);
				}
			}
		}
		#else
		for (int i = 0; i < ACS37800_IC_Num; i++)
		{
			ACS37800_readRMS(&PowerMonitorData[i].Vrms, &PowerMonitorData[i].Irms); // Read the RMS voltage and current
			ACS37800_readPowerActiveReactive(&PowerMonitorData[i].Pactive, &PowerMonitorData[i].Preactive); // Read the power active and reactive
			ACS37800_readPowerFactor(&PowerMonitorData[i].Papparent, &PowerMonitorData[i].Pfactor, &PowerMonitorData[i].PosAngle, &PowerMonitorData[i].PosPF); // Read the power factor
			ACS37800_readErrorFlags(&PowerMonitorData[i].ACSstatus); // Read the error flags
		}
		#endif
		//LOG("~~~~~~~~~~~~~~~~~~trigflag : %d~~~~~~~~~~~~~~~\r\n", sbINFOH.TrigFlag);
		osSemaphoreRelease(PMDataSemaphore); // 释放信号量
#if 0
		if (sbINFOH.SlaveTranstoMasterMode == SlaveTranstoMasterMode_Trigger)
		{
			if (TrigFlag && DevButton)		//DevButton用于测试用，只在开发板上有用
			{
				DevButton = 0;
				LOG("goto write data to master ....%ld\r\n", xTaskGetTickCount());
				//PB2BB_INT_ToMasterB(1);
				uint8_t wDataLen = (PMFloatDataNum*2+PMIntDataNum)*ACS37800_IC_Num;
				uint8_t wData[wDataLen];
				for (int i = 0; i < ACS37800_IC_Num; i++)
				{
					#if 0
					LOG("PowerMonitorData[%d].Vrms : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Vrms*100));
					LOG("PowerMonitorData[%d].Irms : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Irms*100));
					LOG("PowerMonitorData[%d].Pactive : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Pactive*100));
					LOG("PowerMonitorData[%d].Preactive : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Preactive*100));
					LOG("PowerMonitorData[%d].Papparent : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Papparent*100));
					LOG("PowerMonitorData[%d].Pfactor : %d\r\n", i, (uint16_t)(PowerMonitorData[i].Pfactor*100));
					LOG("PowerMonitorData[%d].PosAngle : %d\r\n", i, PowerMonitorData[i].PosAngle);
					LOG("PowerMonitorData[%d].PosPF : %d\r\n", i, PowerMonitorData[i].PosPF);
					LOG("PowerMonitorData[%d].ACSstatus : %d\r\n", i, PowerMonitorData[i].ACSstatus);
					#endif
					structTOArray(&PowerMonitorData[i], PMFloatDataNum, &wData[i*(PMFloatDataNum*2+PMIntDataNum)]);	//6个浮点数值
					uint8_t intstarrtAddr = (PMFloatDataNum*2) + (i*(PMFloatDataNum*2+PMIntDataNum));
					wData[intstarrtAddr] 		= PowerMonitorData[i].PosAngle;
					wData[intstarrtAddr+1] 		= PowerMonitorData[i].PosPF;
					wData[intstarrtAddr+2]		= PowerMonitorData[i].ACSstatus;
					//LOG("............wData[%d] : %d\r\n", (intstarrtAddr+2), wData[intstarrtAddr+2]);
					#if 0
					//wData[i*2] 		= (uint8_t)PowerMonitorData[i].Vrms;
    				//wData[i*2+1] 	= (uint8_t)((PowerMonitorData[i].Vrms - wData[i*2]) * 100);
					//wData[i*2+2] 	= (uint8_t)PowerMonitorData[i].Irms;
    				//wData[i*2+3] 	= (uint8_t)((PowerMonitorData[i].Irms - wData[i*2+2]) * 100);
					//wData[i*2+4] 	= (uint8_t)PowerMonitorData[i].Pactive;
    				//wData[i*2+5] 	= (uint8_t)((PowerMonitorData[i].Pactive - wData[i*2+4]) * 100);
					//wData[i*2+6] 	= (uint8_t)PowerMonitorData[i].Preactive;
    				//wData[i*2+7] 	= (uint8_t)((PowerMonitorData[i].Preactive - wData[i*2+6]) * 100);
					//wData[i*2+8] 	= (uint8_t)PowerMonitorData[i].Papparent;
    				//wData[i*2+9] 	= (uint8_t)((PowerMonitorData[i].Papparent - wData[i*2+8]) * 100);
					//wData[i*2+10] 	= (uint8_t)PowerMonitorData[i].Pfactor;
    				//wData[i*2+11] 	= (uint8_t)((PowerMonitorData[i].Pfactor - wData[i*2+10]) * 100);
					#endif
				}

				#if 0
				LOG("wdata : ");
				for (uint16_t i = 0; i < wDataLen; i++)
				{
					LOG("%d ", wData[i]);	//wData[i]
				}
				LOG("\r\n...wDataLen : %d\r\n", wDataLen);
				#endif
				HAL_SPI_MspInit(&hspi2);
				while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {}
				
				PB2BB_INT_ToMasterB(1);
				PB2BB_INT_ToMasterB(0);
				//LOG("cs = %d ...%ld\r\n", MSP_SPI2_CS_STATUS(), xTaskGetTickCount());
				//waitSPI2CSequal(0, 10);
				//LOG("cs = %d ...%ld\r\n", MSP_SPI2_CS_STATUS(), xTaskGetTickCount());
				BackPanelTransStatus_TypeDef bpstatus;
				bpstatus = BackPanelTrans_Slave_writeDataTo_Master(wData, wDataLen);
				HAL_SPI_MspDeInit(&hspi2);
				//LOG("Errcode : %d, ErrorInfo : %s\r\n", bpstatus, Error_LOG[Error_Offset+bpstatus]);
				//LOG("cs = %d ...%ld\r\n", MSP_SPI2_CS_STATUS(), xTaskGetTickCount());
				//waitSPI2CSequal(1, 15);
				//LOG("cs = %d ...%ld\r\n", MSP_SPI2_CS_STATUS(), xTaskGetTickCount());
			}
		}
#endif	

		osMutexRelease(mutex);
		osDelay(50);		//经测试小于50ms 通信有异常 2022-05-24
	}
}


/**
 * @brief  跟主板之间通过spi传输数据，读取指令的时候，同时发送数据给主板
 * @param  argument: Not used
 * @retval None
 */
#if 1
void Start_Slave_BackPanelTransTask(void *argument)
{	
	/* Infinite loop */
	for (;;)
	{
		osMutexAcquire(mutex, osWaitForever);
		//LOG("start slave back panel trans\r\n");
		osSemaphoreAcquire(PMDataSemaphore, osWaitForever);
		if (sbINFOH.SlaveTranstoMasterMode == SlaveTranstoMasterMode_Trigger)
		{
			if (_TrigFlag && DevButton)		//DevButton用于测试用，只在开发板上有用
			{
				DevButton = 0;
				LOG("goto write data to master ....%ld\r\n", xTaskGetTickCount());
				//PB2BB_INT_ToMasterB(1);
				uint8_t wDataLen = (PMFloatDataNum*2+PMIntDataNum)*ACS37800_IC_Num;
				uint8_t wData[wDataLen];
				uint8_t rData[120];
				for (int i = 0; i < ACS37800_IC_Num; i++)
				{
					structTOArray(&PowerMonitorData[i], PMFloatDataNum, &wData[i*(PMFloatDataNum*2+PMIntDataNum)]);	//6个浮点数值
					uint8_t intstarrtAddr = (PMFloatDataNum*2) + (i*(PMFloatDataNum*2+PMIntDataNum));
					wData[intstarrtAddr] 		= PowerMonitorData[i].PosAngle;
					wData[intstarrtAddr+1] 		= PowerMonitorData[i].PosPF;
					wData[intstarrtAddr+2]		= PowerMonitorData[i].ACSstatus;
					//LOG("............wData[%d] : %d\r\n", (intstarrtAddr+2), wData[intstarrtAddr+2]);

				}
				#if 0
				LOG("wdata : ");
				for (uint16_t i = 0; i < wDataLen; i++)
				{
					LOG("%d ", wData[i]);	//wData[i]
				}
				LOG("\r\n...wDataLen : %d\r\n", wDataLen);
				#endif
				HAL_SPI_MspInit(&hspi2);
				
				PB2BB_INT_ToMasterB(1);
				PB2BB_INT_ToMasterB(0);
				BackPanelTransStatus_TypeDef bpstatus;
				bpstatus = BackPanelTrans_Slave_WriteAndRead(sbINFOH.boardID, wData, rData, wDataLen);
				LOG("out.Errcode : %d, ErrorInfo : %s\r\n", bpstatus, Error_LOG[Error_Offset+bpstatus]);
				HAL_SPI_MspDeInit(&hspi2);
			}
		}
		osSemaphoreRelease(PMDataSemaphore); // 释放信号量
		osMutexRelease(mutex);
		osDelay(10);
	}
}
#endif

/**
 * @brief  spi2为从模式，通过SPI2与主控板通讯，将从板数据上传给主控板
 * @param  argument: Not used
 * @retval None
 */
#if 0	//wwb
void Start_SPI2_TransTask(void *argument)
{
	/* Infinite loop */
	for (;;)
	{
		if (CHIPH[D_I_Q_Chip_1].isChipEnable == 1)
		{
			HAL_SPI_MspInit(&hspi2);
			TrigINT_ToMasterB(1);
			CHIPH[D_I_Q_Chip_1].isChipEnable = 0;
			TrigINT_ToMasterB(0);
			LED_R(1);
			void *sTrans = SPITransfer_C_New(&CHIPH[D_I_Q_Chip_1], &hspi2, SET_SPIMODE_SLAVE);
			SPITransfer_C_Slave_Spi2_Transfer(sTrans, TxFlag, CHIPH[D_I_Q_Chip_1].DChipID);
			/*发送完spi数据后，需要加一条打印指令，要不会报错(因为主控也加了，要去都得去掉)*/
			LOGI("write data to master complete.........%ld\r\n", HAL_GetTick());
			HAL_SPI_MspDeInit(&hspi2);
		}
		LED_R(0);
		osDelay(2);
	}
}
#endif


#if 0	//用于测试读取chip数据
void Start_SPI2_TransTask(void *argument)
{
	/* Infinite loop */
	for (;;)
	{
		if (CHIPH[D_I_Q_Chip_1].isChipEnable == 1)
		//if (0)
		{
			HAL_SPI_MspInit(&hspi2);
			void *sTrans = SPITransfer_C_New(&CHIPH[D_I_Q_Chip_1], &hspi2, SET_SPIMODE_SLAVE);
			SPITransfer_C_Slave_Spi2_Transfer(sTrans, TxFlag, CHIPH[D_I_Q_Chip_1].DChipID);
			/*发送完spi数据后，需要加一条打印指令，要不会报错(因为主控也加了，要去都得去掉)*/
			LOGI("write data to master complete.........%ld\r\n", HAL_GetTick());
			HAL_SPI_MspDeInit(&hspi2);
		}
		LED_R(0);
		osDelay(2);
	}
}
#endif

/**
 * @brief  spi2为从模式，通过SPI2与主控板通讯，从主板读取数据
 * @param  argument: Not used
 * @retval None
 */
#if 1
void Start_ReadMasterBDataTask(void *argument)
{
	/* Infinite loop */
	for (;;)
	{
		if (!MSP_SPI2_CS_STATUS()) 
		{
			SPI2_CS_Trig = 0;
			//HAL_NVIC_DisableIRQ(SPI2_CS_EXTI_IRQn);
			uint16_t rDataLen;
			uint8_t *rData = NULL;
			LOG("........................................goto BackPanelTrans_Slave_readDataFrom_Master...%d\r\n", MSP_SPI2_CS_STATUS());
			BackPanelTrans_Slave_readDataFrom_Master(rData, &rDataLen);
			LOG("read data from master : ");
			//for (int i = 0; i < rDataLen; i++)
			//{
			//	LOG("%02X ", rData[i]);
			//}
			//LOG("\r\n");
			//HAL_NVIC_EnableIRQ(SPI2_CS_EXTI_IRQn);
		}
		osDelay(50);
	}
}
#endif

