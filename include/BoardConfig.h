#ifndef _BOARDCONFIG_H_
#define	_BOARDCONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32g4xx_hal.h"

//#define DEVBoardYD            //DEVBoard
#define DEVBoardG4
#define UartPrintf            //LOG_DEBUG	;UartPrintf
#define LKHMD500MainB        
//#define RTOS_enable

#define DCModuleNum                 5

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

typedef struct
{
	/*直流模块的输入属性：往PLC发送的属性*/
    float 		INData_DCModule_Volt[DCModuleNum];
	float 		INData_DCModule_Curr[DCModuleNum];
	/*设备的状态值*/
	uint8_t		INData_DevState[1];

}INDATAParaHandle;

typedef struct
{
	/*设备的使能信号*/
	uint8_t		OUTData_DevEnable[1];
	/*直流模块的输出属性：从PLC获取到的属性, 包含控制指令和配置参数*/
	float 		OUTData_DCModule_SetCurr[DCModuleNum];
	uint16_t	OUTData_DCModule_SetFreq[DCModuleNum];
    uint8_t 	OUTData_DCModule_SetPWM[DCModuleNum];
}OUTDATAParaHandle;


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
