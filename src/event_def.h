
#ifndef EVENT_DEF_H__
#define EVENT_DEF_H__

#include "eventos.h"
#include "EtherCatPDO.h"

#if 0
//#define WaitRecvFrame_ms    100       //发送完指令后， 等待5ms，再处理接收到的数据
#define RepeatSendFrame_ms  1000 //1000       //当未接收到帧回应时，重复发送“发送帧”的时间
#define RestartLAN9255_ms   5000      //重启LAN9255的重复尝试的间隔时间
#define TransDataPeriod_ms  500       //循环写入(和读取)LAN9255数据的时间
//#define ReadDataPeriod_ms   500       //循环读取LAN9255数据的时间
#define ReadStatePeriod_ms  2000      //循环读取LAN9255状态的时间

#define ReadDCMState_ms     100       //循环读取恒流模块的状态的时间

#define WorkLed_Normal_ms   1000
#define workLed_Wait_ms     500
#define WorkLed_Error_ms    100
#endif

//extern EMessageHandler_t _eMsgH;
extern LAN9255State_t L9255State; 

enum
{
	PeriodTime_100ms 	        = 100,
    PeriodTime_200ms            = 200,
    PeriodTime_500ms            = 500,
    PeriodTime_1000ms           = 1000,
    PeriodTime_2000ms           = 2000,
    PeriodTime_3000ms           = 3000, 
    PeriodTime_5000ms           = 5000,
};

enum {
    Event_Test = Event_User,
    Event_WorkLed_Blink,
    Event_PrintLOG,
//    Event_EtherCAT_Process,
    Event_ECAT_Start_InitLAN9255,
    Event_ECAT_END_InitLAN9255,
    Event_ECAT_RestartLAN9255S_SendCMD,
    Event_ECAT_RestartLAN9255S_RecvRSP,
    Event_ECAT_AddParaList_SendCMD,
    Event_ECAT_AddParaList_RecvRSP,
    Event_ECAT_SetMsgTimeout_SendCMD,
    Event_ECAT_SetMsgTimeout_RecvRSP,
    Event_ECAT_StartLAN9255_SendCMD,
    Event_ECAT_StartLAN9255_RecvRSP,
    Event_ECAT_ReadLAN9255State_SendCMD,
    Event_ECAT_ReadLAN9255State_RecvRSP,
//    Event_ECAT_TransDataToLAN9255_SendCMD,
    Event_ECAT_WriteDataToLAN9255_SendCMD,
    Event_ECAT_ReadDataFromLAN9255_SendCMD,
    Event_ECAT_ReadDataFromLAN9255_RecvRSP,
    Event_ECAT_WriteDataToLAN9255_RecvRSP,
    Event_ECAT_SendCMD_TIMEOUT,
    
    Event_ECAT_StartEtherCat,
    Event_ECAT_CancelEtherCat,
    Event_ECAT_StartTransData,
    Event_ECAT_CancelTransData,

    Event_DCM_ReadDCMState,
    Event_DCM_CtrlOUTCurr,

    Event_Button,
    Event_UserTest,
    
    Event_ActEnd,
    
    Event_Max
};

#endif
