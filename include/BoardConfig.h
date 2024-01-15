#ifndef _BOARDCONFIG_H_
#define	_BOARDCONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"

/* 以下参数用于校准恒流模块参数用 */
#define ee_use_StartFrame            0xA5A5                                   //用于确认板子是否已经校准过，校准过的板子不需要重复校准

//校准模式
typedef enum 
{
	DCM_CalibrateMode_ByUART 	    = 0,        //通过串口校准，pc端工具发送指令来校准
	DCM_CalibrateMode_ByKEY         = 1,        //外部按键触发校准
    DCM_CalibrateMode_ByPLC         = 2,        //PLC发送过来的校准要求
} DCM_CalibrateMode_t;

typedef enum 
{
	DCM_CalibrateState_Wait 	            = 0,        //等待进入校准模式
	DCM_CalibrateState_Doing                = 1,        //进入校准模式
    DCM_CalibrateState_SavePara             = 2,     //进入保存参数模式
    DCM_CS_OutCurrGain_Saved                = 3,        //输出电流反馈的比例系数 保存完成
    DCM_CalibrateState_Done                 = 4,        //校准完成
} DCM_CalibrateState_t;

/*******************************************************************************************************************/

#define DIVIDE_AND_ROUND(a, b) ((a) / (b) < 1 ? 1 : (a) / (b))

#define word(a, b) (((uint16_t)(a) << 8) | (b))		//2个uint8_t合并为一个uint16_t
#define lowByte(x) ((uint8_t)(x))					//读取uint16_t中的低八位
#define highByte(x) ((uint8_t)((x) >> 8))		//读取uint16_t中的高八位
#define Dword(a, b) (((uint32_t)(a) << 16) | (b))	//((uint32_t)value1 << 16) | value2
#define DFword(a, b) (((uint64_t)(a) << 32) | (b))	

//#define DCM4_Reversed_DCM5      //临时版本

//#define DEVBoardG4
#define USBPrintf      //USBPrintf        //UartPrintf            //LOG_DEBUG	;UartPrintf     //USBPrintf
#define LKHMD500MainB    

#define USE_AdjustFunc      //使用输出电流校准功能  
#define USE_EEPROMFunc          //使用EEPROM掉电保存参数功能  

#define DCModuleNum                 5


union VoidData
{
    void *p;
    uint8_t i8;
    uint16_t i16;
    uint32_t i32;
    float f;
    double d;
    char c;
};


/*设备状态码*/
typedef enum 
{
	DeviceState_Default 	        = 0,
	DeviceState_ConnectedPLC        = 10201,
} DeviceState_t;

/*设备错误码*/
typedef enum 
{
	DeviceError_noConnectedPLC	    = 30201,
	DeviceError_OpenSSRFail		    = 30301,
    DeviceError_CloseSSRFail		= 30302,
	DeviceError_KTempReadFail       = 30401,
    DeviceError_ReadDataFail        = 30501,   //从PLC读取数据有误
    DeviceError_PIDParam            = 30601,   //PID参数设置有错
} DeviceError_t;

/*错误等级*/
typedef enum 
{
    NOError         = 0,        //没有错误
    SWError         = 1,        //软件运行超出预期的错误
    HWError         = 2,        //硬件错误！包含：SSR控制错误、 热电偶读不到温度等
} ErrorLevel_t;

extern uint16_t DeviceStateInfo;    // = DeviceState_Default; 

#if 0
typedef struct
{
    uint16_t addr_offset;
    void     *data;
}DATAParaHandle;
#endif

// 定义数据类型枚举
typedef enum {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    // 其他可能的数据类型...
} DataTypeHandle;

typedef struct
{
	/*直流模块的输入属性：往PLC发送的属性*/
//    float 		INData_DCModule_Volt[DCModuleNum];
	float 		INData_DCModule_Curr[DCModuleNum];
	/*每个通道的状态值*/
	uint16_t		INData_DCMState[DCModuleNum];

}INDATAValueHandle;
INDATAValueHandle INDataValue;

typedef struct
{
	/*每个通道的使能信号*/
	uint8_t 	OUTData_DCModule_Enable[DCModuleNum];
	/*直流模块的输出属性：从PLC获取到的属性, 包含控制指令和配置参数*/
	float		OUTData_DCModule_SetCurr[DCModuleNum];
#ifdef USE_AdjustFunc
    /* 用于校准恒流模块的输出电流 */
    uint8_t		OUTData_DCModule_AdjustOUTCurr[DCModuleNum];
#endif
}OUTDATAValueHandle;
OUTDATAValueHandle OUTDataValue;

enum {
    IN_DCModule_Volt,
    IN_DCModule_Curr,
    IN_DevState,

    OUT_DevEnable,
    OUT_DCModule_SetCurr,
    OUT_DCModule_SetFreq,
    OUT_DCModule_SetPWM,

    INOUTDATA_Max
};


#ifdef __cplusplus
}
#endif

#endif  /* BOARDCONFIG_H_*/
