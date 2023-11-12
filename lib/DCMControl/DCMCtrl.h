#ifndef _DCMCTRL_H_
#define _DCMCTRL_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "BoardConfig.h"
#include "MSP_GPIO.h"


/* 恒流模块的使能引脚 */
#define DCM1_EN(n)			(n?HAL_GPIO_WritePin(DCM1_EN_Port, DCM1_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(DCM1_EN_Port, DCM1_EN_Pin, GPIO_PIN_RESET))
#define DCM2_EN(n)			(n?HAL_GPIO_WritePin(DCM2_EN_Port, DCM2_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(DCM2_EN_Port, DCM2_EN_Pin, GPIO_PIN_RESET))
#define DCM3_EN(n)			(n?HAL_GPIO_WritePin(DCM3_EN_Port, DCM3_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(DCM3_EN_Port, DCM3_EN_Pin, GPIO_PIN_RESET))
#define DCM4_EN(n)			(n?HAL_GPIO_WritePin(DCM4_EN_Port, DCM4_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(DCM4_EN_Port, DCM4_EN_Pin, GPIO_PIN_RESET))
#define DCM5_EN(n)			(n?HAL_GPIO_WritePin(DCM5_EN_Port, DCM5_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(DCM5_EN_Port, DCM5_EN_Pin, GPIO_PIN_RESET))


#define DCM_Vref                300      //参考电压3。3v时 就是330
#define DCM_Vref_Multi          100     //参考电压的倍数
#define DCM_DAC_Resolution      12      //DAC输出分辨率
#define DCM_DAC_MAX          	((1 << DCM_DAC_Resolution) - 1)
#define DCM_ADC_Resolution      12
#define DCM_ADC_MAX             ((1 << DCM_ADC_Resolution) - 1)

#define DCM_SetCurr_Gain			    1250	//设置输出电流是的比例系数：1.6V对应的是20A    1.6 * 12.5 = 20A
#define DCM_OUTCurr_Gain			    755		//读取输出电流的比例系数：  2.65V对应的是20A   2.65 * 7.55 = 20A
#define DCM_OUTVolt_Gain			    833		//读取输出电压的比例系数：  3.0V对应的是25V    3.0 * 8.33 = 25V  
#define DCM_Gain_Multi        			100

#define DCM_ANALOGIN_Size       2  

#define DCM1_OUTVolt			0
#define DCM1_OUTCurr			1
#define DCM2_OUTVolt			0
#define DCM2_OUTCurr			1
#define DCM3_OUTCurr			0
#define DCM3_OUTVolt			1
#define DCM4_OUTVolt			0
#define DCM4_OUTCurr			1
#define DCM5_OUTCurr			0
#define DCM5_OUTVolt			1

extern float DCM_OUTCurr_Max;      // 输出最大电流
extern float DCM_OUTCurr_Min;      // 输出最小电流
extern float DCM_OUTCurr_Limit;   // 判断有无电流的最小值

typedef enum
{
	DCModule_1			= 0,
	DCModule_2		    = 1,
	DCModule_3			= 2,
	DCModule_4			= 3,
	DCModule_5 			= 4,
} DCModule_t;

typedef enum
{
	DCMState_Work		   		= 10201,		//工作
	DCMState_Stop				= 10401,		//停止

	DCMState_OUT_OverCurr		= 30101,		//输出电流过大，报此错误: 30101~30105
	DCMState_IN_NoVolt			= 30201,		//是否有输入电压，没有输入电压，报此错误: 30201~30205
	DCMState_OUT_Break			= 30301,		//输出断路，报此错误: 30301~30305
	DCMState_Fault				= 30401,		//模块故障，报此错误: 30401~30405
//	DCMState_OUT_Short			= 30501,		//输出短路，报此错误: 30501~30505
	DCMState_OverTemp			= 30601,		//模块温度过高
	DCMState_OUT_Energy			= 30701,		//输出能量异常
	DCMState_OUT_UnderVolt		= 30801,		//输出电压过低
	DCMState_OUT_OverVolt		= 30901,		//输出电压过大

	DCMState_Set_OutCurr_Err	= 40101,		//设置输出电流值错误 超过20A，或者小于0.8A
} DCModule_State_t;


typedef enum
{
	Disable_DCM	= 0,
	Enable_DCM  = 1,	
} DCModuleENOUT_t;

enum
{
	DCM_Normal	= 0,
	DCM_Fault  = 1,	
};

enum
{
	DCM_Have_INVolt  = 0,	
	DCM_No_INVolt	 = 1,
};


#if 0
typedef struct
{
	uint8_t EXTIPinCnt[DCModuleNum];
	uint8_t PinDebounde[DCModuleNum];
	uint8_t PinValue[DCModuleNum];
	uint32_t TrigLastTime[DCModuleNum];
}
EXTIPINHandler_t;

EXTIPINHandler_t DCM_Fault_h;
EXTIPINHandler_t DCM_Pdet_h;
#endif


//读取每个模块的输出电流和输出电压，保存在buffer中
typedef struct
{
    uint16_t        DCM1_ANALOGIN[DCM_ANALOGIN_Size];            //保存第一个模块的ADC输入 [Volt, Curr]
    uint16_t        DCM2_ANALOGIN[DCM_ANALOGIN_Size];            //保存第一个模块的ADC输入 [Volt, Curr]
    uint16_t        DCM3_ANALOGIN[DCM_ANALOGIN_Size];            //保存第一个模块的ADC输入 [Curr, Volt]
    uint16_t        DCM4_ANALOGIN[DCM_ANALOGIN_Size];            //保存第一个模块的ADC输入 [Volt, Curr]
    uint16_t        DCM5_ANALOGIN[DCM_ANALOGIN_Size];            //保存第一个模块的ADC输入 [Curr, Volt]
}DCModule_AnalogIn_t;//对象的句柄

uint8_t ADC_CONV_COMPLETE_FLAG;

//DCM恒流模块 执行控制指令时，需要用到的参数
typedef struct
{
	uint8_t DCM_No;
	uint8_t DCM_EnablePin;
	float DCM_SetCurr_V;
}DCModule_OUTCtrl_t;

//卡尔曼滤波
typedef struct
{
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为1
	float Last_out;//上次卡尔曼滤波器输出 初始化值为0
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.0001
    float R;//观测噪声协方差 初始化值为5
}KFP;//Kalman Filter parameter


void dcm_init(void);
uint8_t dcm_enable_output(uint8_t ch, uint8_t en);
uint8_t dcm_read_enable_value(uint8_t ch);
uint8_t dcm_set_outcurrent(uint8_t ch, float value);
float dcm_get_outcurrent(uint8_t ch);
float dcm_get_outvoltage(uint8_t ch);
DCModule_State_t dcm_get_dcmstate(uint8_t ch);
uint8_t dcm_get_dcmfault(uint8_t ch);
uint8_t dcm_read_fault_value(uint8_t ch);
uint8_t dcm_get_pdet(uint8_t ch);
uint8_t dcm_read_pdet_value(uint8_t ch);

void start_analogout(uint8_t ch);
void stop_analogout(uint8_t ch);
void set_analogout(uint8_t ch, uint32_t value);
void start_analogin(uint8_t ch);
void stop_analogin(uint8_t ch);

//float kalmanFilter(float ADC_Value);
float kalmanFilter(KFP *kfp, float input);

char *DCModule_State_Log(DCModule_State_t dcmstate);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  /* _DCMCTRL_H_ */
