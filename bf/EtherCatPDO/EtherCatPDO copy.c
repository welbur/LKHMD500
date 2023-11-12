/*
  EtherCatPDO.cpp - Modbus RTU Slave library for Arduino

    Copyright (C) 2018 Sfera Labs S.r.l. - All rights reserved.

  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  See file LICENSE.txt for further informations on licensing terms.
*/

#include "EtherCatPDO.h"
#include "FrameFormat.h"
#include "eos_event.h"
#include "eventos.h"
#include "event_def.h"

// extern uint16_t DeviceStateInfo;

byte _wData[200];
uint16_t _wDataidx = 0;
// uint16_t _rDataAddr[10];
// uint16_t _rDataAddridx = 0;

uint16_t KeyOUTData_TimeOut = 1000;
uint16_t KeyOUTDataAddr_PLCToMCU[2] = {ObjAddr_DevEnable, ObjAddr_DCModule_Curr};

/*用于数据帧格式的变量*/
bool InitCompleted = false;
bool readySendData = false;
bool writeDataSuccess = false;
bool readDataSuccess = false;
// uint8_t DeviceInitComplete = 0;
// uint8_t SendCMDStatus = CMDState_startInit;

bool FirstPowerUP = true;
bool Enable_ReadLAN9255State = true;

uint8_t ParaList_Num;    //            = 0;   //用于将属性表发送给PLC的计数
uint8_t ObjList_Num;     //             = 0;    //用于添加属性表的计数
uint8_t Obj_INDATA_Num;  //          = 0;
uint8_t Obj_OUTDATA_Num; //         = 0;
// const char *ObjName[ParaList_MaxNum] = {ObjName_LoadStatus, ObjName_INOW, ObjName_IRMS, ObjName_SSRCtrl};
uint16_t FrameMsgID = StartMsgID;
static uint16_t MaxMsgID = 1023;
uint16_t CurrentMsgID;
uint16_t FrameActiveDataLen;

EPDOHandler_t *_etherCatH;
EMessageHandler_t _eMsgH;
LAN9255State_t _lan9255State; 
// uint8_t RecvBuffer[MAX_BUFFER];
// uint8_t RecvBufferCnt;

// const uint16_t _t35chars = 1750;
const char *ntoc[9] = {"_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"}; //"," 3"," 4"," 5"," 6"," 7"," 8"," 9"};

OBJHandle OBJ_H[20];

void EtherCatPDO_Init(EPDOHandler_t *ecatH)
{
    /* 初始化接收缓冲区 和发送缓冲区*/
    // RecvBufferCnt = 0;
    // memset(RecvBuffer, 0, MAX_BUFFER);
    LOG("indata : %f\n", ecatH->INData_Para->INData_DCModule_Curr[1]);

    ecatH->TxEnd = 1;       //txend = 1表示发送完成

    ecatH->RecvBufCnt = 0;
    ecatH->SendBufCnt = 0;
    ecatH->RecvData = 0;
    memset(ecatH->RecvBuf, 0, MAX_BUFFER); // ecatH->RecvBufSize);
    memset(ecatH->SendBuf, 0, MAX_BUFFER); // ecatH->SendBufSize);

    /* 设置t35chars时间 */
    ecatH->t35chars = T35Chars * 5;

    ecatH->RecvLastTime = 0;

    /**/
    ecatH->read_LAN9255State_ts = 0;
    if (ecatH->read_LAN9255State_interval == 0)
        ecatH->read_LAN9255State_interval = 1000;

    //    DeviceName = devName;
    /* 读取LAN9255模块状态的间隔时间 */
    //    _readStateTS = readStateTS;

    _etherCatH = ecatH;

    /*初始化相关变量*/
    _wDataidx = 0;    // 用于发送数据帧时，计算发送数据的数量
    ParaList_Num = 0; // 用于将属性表发送给PLC的计数
    ObjList_Num = 0;  // 用于添加属性表的计数
    Obj_INDATA_Num = 0;
    Obj_OUTDATA_Num = 0;

    /*添加属性表*/
    EtherCatPDO_AddOBJ(ObjAddr_DCModule_Volt, INDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "DCM Voltage\0");
    EtherCatPDO_AddOBJ(ObjAddr_DCModule_Curr, INDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "DCM Current\0");
    EtherCatPDO_AddOBJ(ObjAddr_DevState, INDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, 1, "Device State\0");

    EtherCatPDO_AddOBJ(ObjAddr_DevEnable, OUTDIR, BodyDataType_DEFTYPE_UINT8, BodyDataBitLen_UINT8, 1, "Enable Device\0");

    EtherCatPDO_AddOBJ(ObjAddr_DCModule1_SetCurr, OUTDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "Set DCM_1 Current\0");
    EtherCatPDO_AddOBJ(ObjAddr_DCModule2_SetCurr, OUTDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "Set DCM_2 Current\0");
    EtherCatPDO_AddOBJ(ObjAddr_DCModule3_SetCurr, OUTDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "Set DCM_3 Current\0");
    EtherCatPDO_AddOBJ(ObjAddr_DCModule4_SetCurr, OUTDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "Set DCM_4 Current\0");
    EtherCatPDO_AddOBJ(ObjAddr_DCModule5_SetCurr, OUTDIR, BodyDataType_DEFTYPE_REAL32, BodyDataBitLen_REAL32, DCModuleNum, "Set DCM_5 Current\0");

//    EtherCatPDO_AddOBJ(ObjAddr_DCModule1_SetFreq, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_1 Frequency\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule2_SetFreq, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_2 Frequency\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule3_SetFreq, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_3 Frequency\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule4_SetFreq, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_4 Frequency\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule5_SetFreq, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_5 Frequency\0");

//    EtherCatPDO_AddOBJ(ObjAddr_DCModule1_SetPWM, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_1 PWM\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule2_SetPWM, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_2 PWM\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule3_SetPWM, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_3 PWM\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule4_SetPWM, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_4 PWM\0");
//    EtherCatPDO_AddOBJ(ObjAddr_DCModule5_SetPWM, OUTDIR, BodyDataType_DEFTYPE_UINT16, BodyDataBitLen_UINT16, DCModuleNum, "Set DCM_5 PWM\0");


    // Receive data from serial port for Modbus using interrupt
    if (HAL_UART_Receive_IT(_etherCatH->port, &_etherCatH->RecvData, 1) != HAL_OK) {
        Error_Handler();
    }

    LOG_info("EtherCatPDOClass begin\r\n");
}

void EtherCatPDO_AddOBJ(uint16_t objaddr, uint8_t in_out, uint16_t bodydatatype, uint16_t bodydatabitlen, uint16_t N, const char *name)
{
    //OBJHandle OBJ_Cache;
    uint16_t bodyaccess = 0;
    uint16_t objcode = 0;
    uint16_t objnum = 0;
    uint16_t bodynum = 0;
    char objname[30], bodyname[30];

    /*设置body的读取方向*/
    if (in_out == INDIR)
    {
        bodyaccess = ACCESS_READ_IN;
        objcode = ObjCode_Record;
        objnum = 1; // 目前输入属性都是用的Record，可以做成：1个对象里面多个body
        bodynum = N;
        Obj_INDATA_Num++;
        strcpy(bodyname, name);
    }
    else if (in_out == OUTDIR)
    {
        bodyaccess = ACCESS_READWRITE_OUT;
        objcode = ObjCode_Variable;
        objnum = N;
        bodynum = 1;
        Obj_INDATA_Num ++;      //= objnum;
        strcpy(bodyname, "\0");
    }

    OBJHandle OBJ_Cache = {
        .ObjAddr = objaddr,
        .ObjCode = objcode,
        .ObjNum = objnum,
        .BodyDataType = bodydatatype,
        .BodyAccess = bodyaccess,
        .BodyDataBitLen = bodydatabitlen,
        .BodyNum = bodynum,
        //    .OBName = name
    };
    strncpy(OBJ_Cache.ObjName, name, strlen(name) + 1);      //sizeof(OBJ_Cache.OBName) - 1);
    strncpy(OBJ_Cache.BodyName, bodyname, strlen(bodyname) + 1); 
    OBJ_H[ObjList_Num] = OBJ_Cache;
    ObjList_Num++;
    
}



/********************************************************************************
 * @brief 3、跟LAN9255的通讯：EtherCatPDO event sm
 * 
 ********************************************************************************/
#if 1       //(EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */

typedef struct eos_sm_ecat_tag {
    eos_sm_t super;
    uint8_t repeatCnt;
//    EMessageHandler_t *EMsgH;
    LAN9255State_t status;
} eos_sm_ecat_t;

//EMessageHandler_t _EMsgH;
eos_sm_ecat_t sm_EtherCatPDO;

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e);
//static eos_ret_t EtherCatPDO_state_RestartLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e);
//static eos_ret_t EtherCatPDO_state_WaitInitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e);
static eos_ret_t EtherCatPDO_state_InitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e);
static eos_ret_t EtherCatPDO_state_DataTransferWithPLC(eos_sm_ecat_t * const me, eos_event_t const * const e);


/* api ---------------------------------------------------- */
void eos_sm_EtherCatPDO_init(void)
{
    LOG("eos sm EtherCatPDO init \r\n");
    eos_sm_init(&sm_EtherCatPDO.super, 0, EOS_NULL);
    eos_sm_start(&sm_EtherCatPDO.super, EOS_STATE_CAST(EtherCatPDO_state_init));
   
    sm_EtherCatPDO.status = LAN9255State_Default;
}

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_ECAT_RestartLAN9255S_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_RestartLAN9255S_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_AddParaList_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_AddParaList_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_SetMsgTimeout_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_SetMsgTimeout_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_StartLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_StartLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_RecvRSP);
#endif

    eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, ReadStatePeriod_ms);
    eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, WriteDataPeriod_ms);
    LOG("Event_EtherCatPDO state init \r\n");
    return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
}

/**
 * @brief 重启LAN9255
 * 
 * @param me 
 * @param e 
 * @return eos_ret_t 
 */
#if 0
static eos_ret_t EtherCatPDO_state_RestartLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            me->status = LAN9255State_RestartLAN9255;
            me->repeatCnt = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        case Event_ECAT_RestartLAN9255S_SendCMD:
            RestartLAN9255();
            me->repeatCnt += 1;
            if (me->repeatCnt >= 3)
            {
                EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);
            //return EOS_Ret_Handled;     //EOS_TRAN(EtherCatPDO_state_RestartLAN9255);
            }
            
            return EOS_Ret_Handled;

        case Event_ECAT_RestartLAN9255S_RecvRSP:
            me->repeatCnt = 0;
            /*需要发送一个rst复位信号 引脚复位*/
            
            EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

/**
 * @brief 等待初始化
 * 
 * @param me 
 * @param e 
 * @return eos_ret_t 
 */
#if 0
static eos_ret_t EtherCatPDO_state_WaitInitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            LOG_info("EtherCatPDO_state_WaitInitLAN9255\r\n");
            me->status = LAN9255State_WaitInit;
            me->repeatCnt = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_SendCMD:
            ReadLan9255State();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
                return EOS_TRAN(EtherCatPDO_state_RestartLAN9255);
            
            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_RecvRSP:
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[_eMsgH.data[0]]);
            if (_eMsgH.data[0] == s_WaitInit)
            {
                me->repeatCnt = 0;
                eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, RepeatSendFrame_ms);
                return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            }
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

/**
 * @brief 
 * 
 * @param me 
 * @param e 
 * @return eos_ret_t 
 */
static eos_ret_t EtherCatPDO_state_InitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            LOG_info("EtherCatPDO_state_InitLAN9255\r\n");
            me->status = LAN9255State_Initting;
            me->repeatCnt = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        /*********************      初始化过程中，通讯异常，就重启LAN9255      *********************/
        case Event_ECAT_RestartLAN9255S_SendCMD:
            //RestartLAN9255();
            LOG("test.....444444\r\n");
            me->repeatCnt += 1;
            if (me->repeatCnt >= 3)
            {
                me->repeatCnt = 0;
                LOG("test.....555555\r\n");
                //EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);
                eos_event_time_cancel(Event_ECAT_RestartLAN9255S_SendCMD);
                eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, ReadStatePeriod_ms);
            }
            return EOS_Ret_Handled;

        case Event_ECAT_RestartLAN9255S_RecvRSP:
            me->repeatCnt = 0;
            /*需要发送一个rst复位信号 引脚复位*/
            
            EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);

            /*复位之后，重新启动循环事件：读取LAN9255状态*/
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, ReadStatePeriod_ms);
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        /*********************      刚上电的时候，先读取LAN9255状态      *********************/
        case Event_ECAT_ReadLAN9255State_SendCMD:
            ReadLan9255State();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, RestartLAN9255_ms);
                //return EOS_TRAN(EtherCatPDO_state_RestartLAN9255);
            }
            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_RecvRSP:
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[_eMsgH.data[0]]);
            if (_eMsgH.data[0] == s_WaitInit)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, RepeatSendFrame_ms);
                //return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            }
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        /*********************      添加属性表      *********************/
        case Event_ECAT_AddParaList_SendCMD:
            
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_AddParaList_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, RestartLAN9255_ms);
                //return EOS_TRAN(EtherCatPDO_state_RestartLAN9255);
            }
            AddParaList(ParaList_Num);
            return EOS_Ret_Handled;       

        case Event_ECAT_AddParaList_RecvRSP:
            LOG_info("MsgType is 0x02 : Add ParaList!......result : Add [ %s ] to paraList %s\r\n", OBJ_H[ParaList_Num].ObjName, Result_Log[_eMsgH.data[2]]);
            if (_eMsgH.data[2] == 1)
            {
                me->repeatCnt = 0;
                ParaList_Num++;
            }
            if (ParaList_Num >= ObjList_Num)
            {
                ParaList_Num = 0;
                eos_event_time_cancel(Event_ECAT_AddParaList_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限制
                eos_event_pub_period(Event_ECAT_SetMsgTimeout_SendCMD, RepeatSendFrame_ms);        
            }
            return EOS_Ret_Handled;
        
        /*********************      设置关键数据的超时时间      *********************/
        case Event_ECAT_SetMsgTimeout_SendCMD:
            setMsgCheckTimeout();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_SetMsgTimeout_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, RestartLAN9255_ms);
                //return EOS_TRAN(EtherCatPDO_state_RestartLAN9255);
            }
            return EOS_Ret_Handled;         

        case Event_ECAT_SetMsgTimeout_RecvRSP:
            LOG_info("MsgType is 0x0A : Set Msg Time Out!......result : ");
            if (KeyOUTData_TimeOut == word(_eMsgH.data[1], _eMsgH.data[0]))
            {
                me->repeatCnt = 0;
                LOG("Success!\r\n");
                eos_event_time_cancel(Event_ECAT_SetMsgTimeout_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限�
                eos_event_pub_period(Event_ECAT_StartLAN9255_SendCMD, RepeatSendFrame_ms);
            }
            else
                LOG("Fail!\r\n");
            return EOS_Ret_Handled;

        /*********************      启动LAN9255      *********************/
        case Event_ECAT_StartLAN9255_SendCMD:
            StartDevice();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_StartLAN9255_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, RestartLAN9255_ms);
                //return EOS_TRAN(EtherCatPDO_state_RestartLAN9255);  
            }          
            return EOS_Ret_Handled;

        case Event_ECAT_StartLAN9255_RecvRSP:
            LOG_info("MsgType is 0x03 : Start Device!......result : %s\r\n", Result_Log[_eMsgH.data[0]]);
            if (_eMsgH.data[0] == 1) 
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_StartLAN9255_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限
                eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, ReadStatePeriod_ms);
                return EOS_TRAN(EtherCatPDO_state_DataTransferWithPLC);
            }
            return EOS_Ret_Handled;

        default:
            return EOS_SUPER(eos_state_top);
    }
}

#if 1
static eos_ret_t EtherCatPDO_state_DataTransferWithPLC(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            LOG_info("EtherCatPDO_state_DataTransferWithPLC\r\n");
            me->status = LAN9255State_ConnectedToPLC;
            //me->repeatCnt = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_SendCMD:
            ReadLan9255State();
            return EOS_Ret_Handled;
            
        case Event_ECAT_ReadLAN9255State_RecvRSP:
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[_eMsgH.data[0]]);
            if (_eMsgH.data[0] == s_WaitInit) 
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, RepeatSendFrame_ms);
                return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            } 
            else if (_eMsgH.data[0] == s_Connected) 
            {
                me->status = LAN9255State_ConnectedToPLC;
            }
            else if (_eMsgH.data[0] == s_DisConnected)
            {
                me->status = LAN9255State_DisconnectedToPLC;
            }
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

#if 1
        case Event_ECAT_WriteDataToLAN9255_SendCMD:
            LOG_info("Write Data To LAN9255 : ");
            for (int i = 0; i < 5; i++)
            {
                LOG(" %f", _etherCatH->INData_Para->INData_DCModule_Curr[i]);
            }
            LOG("\r\n");
            WriteDataToLAN9255(WriteDataPDO_MsgType);
            //eos_event_pub_period(Event_ECAT_ReadLAN9255State_RecvRSP, WriteDataPeriod_ms);        //, (void *)&v, sizeof(uint8_t));
            return EOS_Ret_Handled;

        case Event_ECAT_WriteDataToLAN9255_RecvRSP:
            LOG("write data to LAN9255 used SDO\r\n");

            return EOS_Ret_Handled;

#endif

        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

#endif



void EtherCatPDO_process()
{
    EMessageHandler_t eMsgH;
    uint8_t recvState;  // = EtherCatPDO_ReceiveFrame(&eMsgH);
    if (recvState > 0)
    {
        _eMsgH = eMsgH;
        LOG("emsgh msgtype: %x\r\n", _eMsgH.MsgType);
        if ((_eMsgH.MsgType - MsgTypeBack) == RestartLAN9255_MsgType)  // 0x81 - 0x80 = 0x01
            eos_event_pub_topic(Event_ECAT_RestartLAN9255S_RecvRSP);        
        else if ((_eMsgH.MsgType - MsgTypeBack) == GetStatus_MsgType)       // 0x81 - 0x80 = 0x01
            eos_event_pub_topic(Event_ECAT_ReadLAN9255State_RecvRSP);
        else if ((_eMsgH.MsgType - MsgTypeBack) == AddParaList_MsgType)          // 0x81 - 0x80 = 0x01
            eos_event_pub_topic(Event_ECAT_AddParaList_RecvRSP);
        else if ((_eMsgH.MsgType - MsgTypeBack) == SetMsgTimeOut_MsgType)            // 0x81 - 0x80 = 0x01
            eos_event_pub_topic(Event_ECAT_SetMsgTimeout_RecvRSP);
        else if ((_eMsgH.MsgType - MsgTypeBack) == StartDevice_MsgType)      // 0x81 - 0x80 = 0x01
            eos_event_pub_topic(Event_ECAT_StartLAN9255_RecvRSP);
        
    }
#if 0
    /*间隔一秒，读取模块状态*/
    if (Enable_ReadLAN9255State && (HAL_GetTick() - _etherCatH->read_LAN9255State_ts > _etherCatH->read_LAN9255State_interval))
    {
        _etherCatH->read_LAN9255State_ts = HAL_GetTick();
        ReadLan9255State();
    }
#endif
}

/**
 * @brief 循环读取串口的数据帧(9255发送过来的)
 * 跟 ProtocolAnalyze()配合使用，读取完数据帧，再解析
 */
//uint8_t EtherCatPDO_ReceiveFrame(EMessageHandler_t *emsgH)
uint8_t EtherCatPDO_ReceiveFrame(void)
{
    if (_etherCatH->RecvBufCnt <= 0 || (HAL_getTick_2us() - _etherCatH->RecvLastTime < _etherCatH->t35chars))
    {
        return 0;
    }
    LOG("current tick us : %ld\r\n", HAL_getTick_2us() - _etherCatH->RecvLastTime);

    uint8_t recCnt = _etherCatH->RecvBufCnt;
    _etherCatH->RecvBufCnt = 0;
#if 0
    /*读取串口接收到的数据，测试用*/
    LOG_info("read data : ");
    for (size_t i = 0; i < recCnt; i++)
    {
        LOG("%x ", _etherCatH->RecvBuf[i]);
    }
    LOG("\r\n");

#endif


    /*查找包头位置，找不到的话，就退出*/
    uint16_t FrameStart = word(FrameStart_1, FrameStart_2); // FrameStart_1 << 8 | FrameStart_2;
    int16_t fs_a = contains(_etherCatH->RecvBuf, recCnt, FrameStart);
    if (fs_a == -1)
    {
        LOG_error("FrameStart error!\r\n");
        return 0;
    }

    // 获取消息id
    uint16_t msgid = word(_etherCatH->RecvBuf[fs_a + 5], _etherCatH->RecvBuf[fs_a + 4]);

    // 消息类型
    uint8_t msgType = _etherCatH->RecvBuf[fs_a + 6]; // - MsgTypeBack; //0x81 - 0x80 = 0x01

    /*判断数据长度*/
    size_t inLen = word(_etherCatH->RecvBuf[fs_a + 3], _etherCatH->RecvBuf[fs_a + 2]) + FrameBaseLen; //_inBuffIdx;
    if ((recCnt < inLen) && (msgType != RestartLAN9255_MsgType))
    {
        LOG_error("read Data Len is too low! error!\r\n");
        return 0;
    }
    //	_etherCatH->RecvBufCnt = 0;

    /*校验crc*/
    uint16_t crc16 = CRC16_CCITT(_etherCatH->RecvBuf, inLen - 2);       //(_etherCatH->RecvBuf, inLen - 2);
    if (_etherCatH->RecvBuf[inLen - 1] != highByte(crc16) || _etherCatH->RecvBuf[inLen - 2] != lowByte(crc16))
    {
        LOG_error("crc error!\r\n");
        return 0;
    }

//    emsgH->MsgID = msgid;
//    emsgH->MsgType = msgType;
//    emsgH->len = inLen - 9; 
//    emsgH->data = _etherCatH->RecvBuf + 7;

//    _eMsgH = emsgH;
    _eMsgH.MsgID = msgid;
    _eMsgH.MsgType = msgType;
    _eMsgH.len = inLen - 9; 
    _eMsgH.data = _etherCatH->RecvBuf + 7;

    if ((_eMsgH.MsgType - MsgTypeBack) == RestartLAN9255_MsgType)  // 0x81 - 0x80 = 0x01
        eos_event_pub_topic(Event_ECAT_RestartLAN9255S_RecvRSP);        
    else if ((_eMsgH.MsgType - MsgTypeBack) == GetStatus_MsgType)       // 0x81 - 0x80 = 0x01
        eos_event_pub_topic(Event_ECAT_ReadLAN9255State_RecvRSP);
    else if ((_eMsgH.MsgType - MsgTypeBack) == AddParaList_MsgType)          // 0x81 - 0x80 = 0x01
        eos_event_pub_topic(Event_ECAT_AddParaList_RecvRSP);
    else if ((_eMsgH.MsgType - MsgTypeBack) == SetMsgTimeOut_MsgType)            // 0x81 - 0x80 = 0x01
        eos_event_pub_topic(Event_ECAT_SetMsgTimeout_RecvRSP);
    else if ((_eMsgH.MsgType - MsgTypeBack) == StartDevice_MsgType)      // 0x81 - 0x80 = 0x01
        eos_event_pub_topic(Event_ECAT_StartLAN9255_RecvRSP);
        
    return 1;
    // 消息处理
    //EtherCatPDO_ProtocolAnalyze(msgid, msgType, _etherCatH->RecvBuf + 7, inLen - 9);
}

/**
 * @brief 发送数据帧给9255
 *
 * @param msgid ： 消息ID
 * @param msgtype ：消息类型
 * @param data ：内容
 * @param len ：内容长度
 */
void EtherCatPDO_SendFrame(uint16_t msgid, uint16_t msgtype, byte *data, uint16_t len)
{
    //_etherCatH->TxEnd = 0; // 标记为 未完成发送
    //    byte buffer[1024];
    size_t idx = 0;
    if (len + FrameBaseLen > (uint16_t)sizeof(_etherCatH->SendBuf))
        len = sizeof(_etherCatH->SendBuf) - FrameBaseLen;

    // 帧头
    _etherCatH->SendBuf[idx++] = FrameStart_1;
    _etherCatH->SendBuf[idx++] = FrameStart_2;

    // 内容长度
    memcpy(_etherCatH->SendBuf + idx, &len, 2);
    idx += 2;

    // 信息id
    memcpy(_etherCatH->SendBuf + idx, &msgid, 2);
    idx += 2;

    // 消息类型
    _etherCatH->SendBuf[idx++] = msgtype;

    // 数据
    memcpy(_etherCatH->SendBuf + idx, data, len);
    idx += len;

    // 校验
    uint16_t crc = CRC16_CCITT(_etherCatH->SendBuf, idx);
    memcpy(_etherCatH->SendBuf + idx, &crc, 2);
    idx += 2;

#if 0
	LOG_info("write data to BaseBoard : ");
	for (size_t i = 0; i < idx; i++) {
		LOG("%x ", _etherCatH->SendBuf[i]);
	}
	LOG("\r\n");
//    LOG("txend : %d\r\n", _etherCatH->TxEnd);
#endif

    _etherCatH->SendBufCnt = idx;
    // transfer buffer to serial line IT
    
//    HAL_UART_Transmit(_etherCatH->port, _etherCatH->SendBuf, idx, 10);
    if (_etherCatH->TxEnd) {
        _etherCatH->TxEnd = 0;
//        LOG("transmit it\r\n");
        HAL_UART_Transmit_IT(_etherCatH->port, _etherCatH->SendBuf, idx);
    }
}

/**
 * @brief 解析读取到的数据帧
 *
 * @param msgid : 消息ID
 * @param msgType ： 消息类型
 * @param data ： 内容
 * @param len ： 内容长度
 */
void EtherCatPDO_ProtocolAnalyze(uint16_t msgid, uint8_t msgType, byte *data, uint16_t len)
{
    msgType = msgType - MsgTypeBack; // 0x81 - 0x80 = 0x01

    switch (msgType)
    {
        case RestartLAN9255_MsgType:
            LOG_info("MsgType is 0x00 : Restart Lan9255 OK ! \r\n");
            break;

        case GetStatus_MsgType:
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[data[0]]);
            if (FirstPowerUP == true)
            {
                FirstPowerUP = false;
                if (data[0] == s_WaitInit && ParaList_Num == 0)
                {
                    LOG_info("First Power Up, goto add ParaList!\r\n");
                    AddParaList(ParaList_Num);
                }
                else
                {
                    LOG_info("First Power Up, is Inited, reStart Lan9255!\r\n"); //
                    ParaList_Num = 0;
                    // 已初始化的话，就重启9255模块
                    RestartLAN9255();
                }
            }
            else
            {
                if (data[0] == s_WaitInit && ParaList_Num == 0)
                {
                    LOG_info("goto add ParaList!\r\n");
                    readySendData = false;
                    AddParaList(ParaList_Num);
                }
                else if (data[0] == s_Connected)
                {
                    readySendData = true;
                }
                else
                {
                    readySendData = false;
                }
            }
            break;

        case AddParaList_MsgType:
            Enable_ReadLAN9255State = false;        //先禁用轮询读取LAN9255的状态，等待属性表添加完成
            LOG_info("MsgType is 0x02 : Add ParaList!......result : Add [ %s ] to paraList %s\r\n", OBJ_H[ParaList_Num].ObjName, Result_Log[data[2]]);
            if (data[2] == 1)      
                ParaList_Num++;
            else
                LOG_error("AddParaList! \r\n");
            if (ParaList_Num < ObjList_Num)
            {
                AddParaList(ParaList_Num);
            }
            else if (ParaList_Num >= ObjList_Num)
            {
                ParaList_Num = 0;
                setMsgCheckTimeout(); // StartDevice();
            }
            break;

        case StartDevice_MsgType:
            LOG_info("MsgType is 0x03 : Start Device!......result : %s\r\n", Result_Log[data[0]]);
            //_inBuff[9]是启动通信的应答结果
            if (data[0] == 1) 
            {
                InitCompleted = true;
                Enable_ReadLAN9255State = true;     //启动通信成功后，重新恢复轮询读取LAN9255的状态
            } 
            else
            {
                InitCompleted = false;
                LOG_error("StartDevice Fault ! \r\n");
            }
            break;

        case ReadData_MsgType:
            LOG_info("MsgType is 0x04 : Read Data!......len : %d\r\n", len);
            ReadDataFromPLC(msgid, data, len);
            break;

        case WriteDataSDO_MsgType:
            LOG_info("MsgType is 0x05 : Write SDO Data!\r\n");
            WriteDataRsp(data, len);
            break;

        case SetMsgTimeOut_MsgType:
            LOG_info("MsgType is 0x0A : Set Msg Time Out!\r\n");
            if (KeyOUTData_TimeOut == word(data[1], data[0]))
            {
                LOG_info("result : Set Msg Time Out Success! \r\n"); // addMessage("result : Set Msg Time Out Success! ");          //
                StartDevice();
            }
            else
            {
                LOG_error("SetMsgTimeOut Fault ! \r\n");
            }
            break;

        default:
            LOG_error("MsgType Fault ! \r\n");
    }
}

/**
 * @brief 读取9255的状态
 * 消息类型： 0x01
 * 默认消息ID ： 1
 *
 */
void ReadLan9255State(void)
{
    //FrameMsgID = ReadStateMsgID;       //addMsgID(FrameMsgID);
    byte Data[0];
    uint16_t DataLen = 0;

    EtherCatPDO_SendFrame(ReadStateMsgID, GetStatus_MsgType, Data, DataLen);
}

/**
 * @brief 解析读取到的9255的状态
 * 
 */
void AnalyzeLan9255State(void)
{

}

/**
 * @brief 添加属性表, 采用Record类型
 * 消息类型： 0x02
 * @param paraidx ：指向OBJ_H中的第几个属性
 */
void AddParaList(uint8_t paraidx)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[300];

    uint16_t DataLen = fillParaListFormat(paraidx, Data);

    EtherCatPDO_SendFrame(FrameMsgID, AddParaList_MsgType, Data, DataLen);
}

/**
 * @brief 启动设备
 * 消息类型： 0x03
 *
 */
void StartDevice(void)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[200];
    uint16_t idx = 0;

    // 添加供应商ID
    memcpy(Data + idx, &venderID, 4);
    idx += 4;

    // 添加产品代码
    memcpy(Data + idx, &productCode, 4);
    idx += 4;

    // 添加修订号
    memcpy(Data + idx, &revisionNumber, 4);
    idx += 4;

    // 添加产品序列号
    memcpy(Data + idx, &productSN, 4);
    idx += 4;

    // 添加厂商名称
    uint16_t cnlen = strlen(CompanyName) + 1;
    memcpy(Data + idx, CompanyName, cnlen);
    idx += cnlen;

    // 添加设备名称
    uint16_t dnlen = strlen(_etherCatH->devName) + 1;
    memcpy(Data + idx, _etherCatH->devName, dnlen);
    idx += dnlen;

    EtherCatPDO_SendFrame(FrameMsgID, StartDevice_MsgType, Data, idx);
}

/**
 * @brief 发送 读取数据 的指令给9255
 * 消息类型： 0x04
 * 消息 id = 0 表示模块主动通知，当数据有变化时模块会主动发送通知
 * 数据位长度 = 0 :表示读取错误，没有这个属性
 * 需要确保返回帧长度不超过 1024
 */
void ReadDataCmd(void)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[200];
    uint16_t idx = 0;

    // 添加对象地址列表
    for (int i = 0; i < Obj_OUTDATA_Num; i++)
    {
        memcpy(Data + idx, &OBJ_H[i + Obj_INDATA_Num].ObjAddr, 2);
        idx += 2;
    }

    EtherCatPDO_SendFrame(FrameMsgID, ReadData_MsgType, Data, idx);
}

/**
 * @brief ReadDataFromPLC() 解析从9255发送过来的数据，并保存
 *
 * @param msgid
 * @param readdata
 * @param len
 */
void ReadDataFromPLC(uint16_t msgid, byte *readdata, uint16_t len)
{
    if (len < 4)
    {
        readDataSuccess = false;
        DeviceStateInfo = DeviceError_ReadDataFail;

        LOG_error("read Data from PLC Fault!\r\n");

        return;
    }
    uint16_t robjaddrlist[10];
    uint8_t listLen = 0;
    uint16_t idx = 0;
    while (idx < len)
    {
        uint16_t objaddr = word(readdata[idx + 1], readdata[idx]); //, data[0]);
        idx += 2;

        robjaddrlist[listLen] = objaddr;
        listLen++;

        uint16_t datalen = word(readdata[idx + 1], readdata[idx]);
        idx += 2;
        if (datalen == 0)
        {
            readDataSuccess = false;
            DeviceStateInfo = DeviceError_ReadDataFail;

            LOG_error("read Data from PLC Fault! \r\n");

            return;
        }
        uint16_t objvaluel, objvalueh;

#if 0
        if (datalen == 8 || datalen == 1) {

        } else if (datalen == 16) {
            objvaluel = word(readdata[idx + 1], readdata[idx]);
            idx += 2;
        } else if (datalen == 32) {
            objvaluel = word(readdata[idx + 1], readdata[idx]);
            idx += 2;
            objvalueh = word(readdata[idx + 1], readdata[idx]);
            idx += 2;
            uint32_t input = Dword(objvalueh, objvaluel);
            memcpy(&_etherCatH->OUTData_Para->OUTData_DCModule_SetCurr, &input, sizeof(_etherCatH->OUTData_Para->OUTData_DCModule_SetCurr));
        } else {
            LOG_error("DeviceError ReadData Fault!\r\n");
            readDataSuccess = false;
            DeviceStateInfo = DeviceError_ReadDataFail;
        }
#endif

        if (objaddr == ObjAddr_DevEnable)
        {
            if (datalen == 8)
                _etherCatH->OUTData_Para->OUTData_DevEnable[0] = readdata[idx];
            else
                DeviceStateInfo = DeviceError_ReadDataFail;
        }
        else if ((objaddr >= ObjAddr_DCModule1_SetCurr) && (objaddr <= ObjAddr_DCModule1_SetCurr + DCModuleNum))
        {
            if (datalen == 32)
            {
                objvaluel = word(readdata[idx + 1], readdata[idx]);
                idx += 2;

                objvalueh = word(readdata[idx + 1], readdata[idx]);
                idx += 2;

                uint32_t input = Dword(objvalueh, objvaluel);
                uint8_t i = objaddr - ObjAddr_DCModule1_SetCurr;
                memcpy(&_etherCatH->OUTData_Para->OUTData_DCModule_SetCurr[i], &input, sizeof(_etherCatH->OUTData_Para->OUTData_DCModule_SetCurr[i]));
            }
            else
                DeviceStateInfo = DeviceError_ReadDataFail;
        }
        else if ((objaddr >= ObjAddr_DCModule1_SetFreq) && (objaddr <= ObjAddr_DCModule1_SetFreq + DCModuleNum))
        {
            if (datalen == 16)
            {
                objvaluel = word(readdata[idx + 1], readdata[idx]);
                idx += 2;

                uint8_t i = objaddr - ObjAddr_DCModule1_SetFreq;
                memcpy(&_etherCatH->OUTData_Para->OUTData_DCModule_SetFreq[i], &objvaluel, sizeof(_etherCatH->OUTData_Para->OUTData_DCModule_SetFreq[i]));
            }
            else
                DeviceStateInfo = DeviceError_ReadDataFail;
        }
        else if ((objaddr >= ObjAddr_DCModule1_SetPWM) && (objaddr <= ObjAddr_DCModule1_SetPWM + DCModuleNum))
        {
            if (datalen == 16)
            {
                objvaluel = word(readdata[idx + 1], readdata[idx]);
                idx += 2;

                uint8_t i = objaddr - ObjAddr_DCModule1_SetPWM;
                memcpy(&_etherCatH->OUTData_Para->OUTData_DCModule_SetPWM[i], &objvaluel, sizeof(_etherCatH->OUTData_Para->OUTData_DCModule_SetPWM[i]));
            }
            else
                DeviceStateInfo = DeviceError_ReadDataFail;
        }
        else
        {
            LOG_error("ObjAddr Fault!\r\n");

            readDataSuccess = false;
            DeviceStateInfo = DeviceError_ReadDataFail;
        }
    }

    if (msgid == 0) // 发送通知确认给9255
        writeMsgCheck(robjaddrlist, listLen);
    readDataSuccess = true;
}

/**
 * @brief 往LAN9255写入数据, 无需应答和需要应答 2种类型
 *
 * @param msgtype : 消息类型 0x05(需要应答) 或者 0x06(无需应答)
 */
void WriteDataToLAN9255(uint8_t msgtype)
{
    FrameMsgID = addMsgID(FrameMsgID);

    fillINDataBodyFormat(_etherCatH->INData_Para->INData_DCModule_Volt, ObjAddr_DCModule_Volt, BodyDataBitLen_REAL32, DCModuleNum);
    fillINDataBodyFormat(_etherCatH->INData_Para->INData_DCModule_Curr, ObjAddr_DCModule_Curr, BodyDataBitLen_REAL32, DCModuleNum);
    fillINDataBodyFormat(_etherCatH->INData_Para->INData_DevState, ObjAddr_DevState, BodyDataBitLen_UINT8, 1);

    EtherCatPDO_SendFrame(FrameMsgID, msgtype, _wData, _wDataidx);
    _wDataidx = 0;
}

/**
 * @brief 解析 从9255发送过来的应答(消息类型0x05的应答)
 *
 * @param data
 * @param len
 */
void WriteDataRsp(byte *data, uint16_t len)
{
    if (len < 3 || len % 3 != 0)
    {
        LOG_error("Write Data Rsp Fault!\r\n");
        return;
    }
    uint8_t objn = len / 3;
    for (int i = 0; i < objn; i++)
    {
        uint16_t objaddr = word(data[1 + i * 3], data[i * 3]); //, data[0]);
        uint8_t result = data[2 + i * 3];
        if (result == 0)
        {
            LOG_error("Write SDO Data %d %s", objaddr, Result_Log[result]);
            writeDataSuccess = false;
            return;
        }
    }
    writeDataSuccess = true;
    return;
}

/**
 * @brief 设置通知确认的超时时间
 *        不频繁写入且关键的数据需要进行设置
 */
void setMsgCheckTimeout(void)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[200];
    uint16_t idx = 0;

    // 添加超时时间
    memcpy(Data + idx, &KeyOUTData_TimeOut, 2);
    idx += 2;

    // 添加对象地址列表
    for (uint8_t i = 0; i < sizeof(KeyOUTDataAddr_PLCToMCU) / sizeof(KeyOUTDataAddr_PLCToMCU[0]); i++)
    {
        memcpy(Data + idx, &KeyOUTDataAddr_PLCToMCU[i], 2);
        idx += 2;
    }

    EtherCatPDO_SendFrame(FrameMsgID, SetMsgTimeOut_MsgType, Data, idx);
}

/**
 * @brief 消息类型 ： 0x0B     通知确认
 *        当9255主动发送消息给MCU时(消息ID = 0)，需要发送此通知确认给9255
 * @param list
 * @param len
 */
void writeMsgCheck(uint16_t list[], uint8_t len)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[200];
    uint16_t idx = 0;

    // 添加对象地址列表
    for (int i = 0; i < len; i++)
    {
        memcpy(Data + idx, &list[i], 2);
        idx += 2;
    }

    EtherCatPDO_SendFrame(FrameMsgID, CheckMsg_MsgType, Data, idx);
}

/**
 * @brief 添加输出属性的对象地址
 *
 * @param objaddr
 */
#if 0
void AddreadDataAddr(uint16_t objaddr)
{
    _rDataAddr[_rDataAddridx] = objaddr;
    _rDataAddridx++;
}
#endif

/**
 * @brief 填充body格式
 *
 * @param bdatatype
 * @param baccess
 * @param bdatabitlen
 * @param bname
 * @param body
 * @return uint16_t
 */
uint16_t fillBodyFormat(const uint16_t bdatatype, const uint16_t baccess, const uint16_t bdatabitlen, const char bname[], byte body[])
{
    uint16_t bidx = 0;
    memcpy(body + bidx, &bdatatype, 2);
    bidx += 2;
    memcpy(body + bidx, &baccess, 2);
    bidx += 2;
    memcpy(body + bidx, &bdatabitlen, 2);
    bidx += 2;
    memcpy(body + bidx, bname, strlen(bname) + 1);
    bidx += strlen(bname) + 1;

    return bidx;
}

/**
 * @brief 填充属性表格式
 *
 * @param paraidx
 * @param paralist
 * @return uint16_t
 */
uint16_t fillParaListFormat(uint8_t paraidx, byte paralist[])
{
    byte bodycache[100];
    uint16_t bodycacheLen = 0;
    uint16_t idx = 0;

    // 添加对象地址
    memcpy(paralist + idx, &OBJ_H[paraidx].ObjAddr, 2);
    idx += 2;

    // 添加对象代码
    memcpy(paralist + idx, &OBJ_H[paraidx].ObjCode, 2);
    idx += 2;
        
    if (OBJ_H[paraidx].ObjCode == ObjCode_Record)
    {
        // 添加对象名称
        uint16_t objnameLen = strlen(OBJ_H[paraidx].ObjName) + 1;
        memcpy(paralist + idx, OBJ_H[paraidx].ObjName, objnameLen);
        idx += objnameLen;

        // 添加body
        uint8_t bodyNum = OBJ_H[paraidx].BodyNum;
        for (int i = 0; i < bodyNum; i++)
        {
            char new_bodyname[30];
            insertCTOStr("DCM", ntoc[i], OBJ_H[paraidx].BodyName, new_bodyname);
            //strcpy(bodyname, OBJ_H[paraidx].BodyName);
            //if (bodyNum > 1)
            //    strcat(bodyname, ntoc[i]); // ntoc[i]);
            bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, new_bodyname, bodycache);
            memcpy(paralist + idx, bodycache, bodycacheLen);
            idx += bodycacheLen;
        }
    }
    else if (OBJ_H[paraidx].ObjCode == ObjCode_Variable)
    {
        uint8_t objNum = OBJ_H[paraidx].ObjNum;
        // 添加对象名称
        uint16_t objnameLen = strlen(OBJ_H[paraidx].ObjName) + 1;
        memcpy(paralist + idx, OBJ_H[paraidx].ObjName, objnameLen);
        idx += objnameLen;

        
            //char bodyname[100];
            // uint8_t bodyNum = OBJ_H[paraidx].OBNum;
            //strcpy(bodyname, OBJ_H[paraidx].OBName);
            //if (objNum > 1)
            //    strcat(bodyname, ntoc[i]);
        // 添加body
        bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, OBJ_H[paraidx].BodyName, bodycache);
        memcpy(paralist + idx, bodycache, bodycacheLen);
        idx += bodycacheLen;
    }
    return idx;
}
#if 0
uint16_t fillParaListFormat(uint8_t paraidx, byte paralist[])
{
    byte bodycache[100];
    uint16_t bodycacheLen = 0;
    uint16_t idx = 0;


    if (OBJ_H[paraidx].ObjCode == ObjCode_Record)
    {
        // 添加对象地址
        memcpy(paralist + idx, &OBJ_H[paraidx].ObjAddr, 2);
        idx += 2;

        // 添加对象代码
        memcpy(paralist + idx, &OBJ_H[paraidx].ObjCode, 2);
        idx += 2;
        
        // 添加对象名称
        uint16_t objnameLen = strlen(OBJ_H[paraidx].OBName) + 1;
        memcpy(paralist + idx, OBJ_H[paraidx].OBName, objnameLen);
        idx += objnameLen;

        // 添加body
        uint8_t bodyNum = OBJ_H[paraidx].BodyNum;
        for (int i = 0; i < bodyNum; i++)
        {
            char bodyname[100];
            strcpy(bodyname, OBJ_H[paraidx].OBName);
            if (bodyNum > 1)
                strcat(bodyname, ntoc[i]); // ntoc[i]);
            bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, bodyname, bodycache);
            memcpy(paralist + idx, bodycache, bodycacheLen);
            idx += bodycacheLen;
        }
    }
    else if (OBJ_H[paraidx].ObjCode == ObjCode_Variable)
    {
        uint8_t objNum = OBJ_H[paraidx].ObjNum;
        for (int i = 0; i < objNum; i++)
        {
            // 添加对象地址
            memcpy(paralist + idx, &OBJ_H[paraidx].ObjAddr, 2);
            idx += 2;

            // 添加对象代码
            memcpy(paralist + idx, &OBJ_H[paraidx].ObjCode, 2);
            idx += 2;

            // 添加对象名称
            uint16_t objnameLen = strlen(OBJ_H[paraidx].OBName) + 1;
            memcpy(paralist + idx, OBJ_H[paraidx].OBName, objnameLen);
            idx += objnameLen;

            // 添加body
            char bodyname[100];
            // uint8_t bodyNum = OBJ_H[paraidx].OBNum;
            strcpy(bodyname, OBJ_H[paraidx].OBName);
            if (objNum > 1)
                strcat(bodyname, ntoc[i]);
            
            bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, bodyname, bodycache);
            memcpy(paralist + idx, bodycache, bodycacheLen);
            idx += bodycacheLen;
        }
    }
    return idx;
}
#endif

/**
 * @brief 重启模块: 重启模块有2个方法，一个指令重启，一个rst复位重启，最好用rst复位重启
 *
 */
void RestartLAN9255(void)
{
    LOG_info("reStart Lan9255 ! \r\n"); // addMessage("Restart LAN9255 ! \r\n");

    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[0];
    uint16_t DataLen = 0;
    EtherCatPDO_SendFrame(FrameMsgID, RestartLAN9255_MsgType, Data, DataLen);
}

/**
 * @brief 获取9255发送过来的应答 （消息类型0x05(写入数据，需要应答)）
 *
 * @return true
 */
bool getWriteDataRsp(void)
{
    bool r = writeDataSuccess;
    writeDataSuccess = false;
    return r;
}

/**
 * @brief 获取9255发送过来的应答 （消息类型0x04(读取数据)）
 * @return true
 * @return false
 */
bool getReadDataRsp(void)
{
    bool r = readDataSuccess;
    readDataSuccess = false;
    return r;
}

/**
 * @brief
 * @note 从PLC获取输出属性的数据，并保存到OUTDataList
 * @param OUTDataList
 * @return uint8_t 输出属性的字节数
 */
#if 0
uint8_t getOUTData(OUTDataValueHandle *OUTDataList)
{
    uint8_t dataLen = 0;
    
    if (memcmp(&_rData, OUTDataList, sizeof(OUTDataValueHandle)) == 0)
        dataLen = -1;
    else 
    {
        memcpy(OUTDataList, &_rData, sizeof(_rData));
        dataLen = sizeof(OUTDataValueHandle);      //return sizeof(OUTDataValueHandle);
    }
    return dataLen;
}
#endif

/**
 * @brief 获取EtherCat通信中的状态
 * 0: 9255初始化失败 ；
 * 1: 9255已经初始化完成，但是还未跟PLC连接上，暂时无法正常通信
 * 2: 已经跟PLC连接上，可以正常通信
 *
 * @return uint8_t 状态值
 */
uint8_t getCommunicateState(void)
{
    uint8_t status;
    if (InitCompleted == true)
    {
        status = LAN9255State_InitSuccess; // 1、 9255已经初始化完成，但是还未跟PLC连接上，暂时无法正常通信
        if (readySendData == true)
        {
            status = LAN9255State_ConnectedToPLC; // 2、 已经跟PLC连接上，可以正常通信
        }
    }
    else
    {
        status = LAN9255State_InitFailt; // 0、 初始化失败
    }

    return status;
}

/**
 * @brief addMsgID
 * @note 添加MessageID
 * @param current_mid
 * @return uint16_t 新的mid
 */
uint16_t addMsgID(uint16_t current_mid)
{
    if (current_mid > MaxMsgID)
    {
        current_mid = StartMsgID;
    }
    else
    {
        current_mid++;
    }
    return current_mid;
}

/**
 * @brief contains
 * @note 用于寻找数据帧当中指定的字节
 * @param arr
 * @param len
 * @param target
 * @return uint16_t
 */
uint16_t contains(uint8_t arr[], uint16_t len, uint16_t target)
{
    uint8_t highByte = target >> 8;
    uint8_t lowByte = target & 0xFF;
    for (uint16_t i = 0; i < len - 1; i++)
    {
        if (arr[i] == highByte && arr[i + 1] == lowByte)
        {
            return i;
        }
    }
    return -1;
}

/***/
void insertCTOStr(const char *keystr, const char *insertstr, const char *name, char *new_name) {
    char *insert_pos = strstr(name, keystr);        //"DCM");  // 找到"DCM"在字符串中的位置
//    LOG("insert_pos : %s\r\n", insert_pos);
    if (insert_pos != NULL) {
        // 计算"DCM"的长度和它在字符串中的位置
        int dcm_len = strlen(keystr);
//        LOG("dcm_len : %d\r\n", dcm_len);
        int offset = insert_pos - name + dcm_len;
//        LOG("offset : %d\r\n", offset);

        // 复制前半部分
        strncpy(new_name, name, offset);
        new_name[offset] = '\0';  // 添加空字符
//        LOG("new_name : %s\r\n", new_name);

        // 插入数字"1"
        strcat(new_name, insertstr);
//        LOG("...new_name : %s\r\n", new_name);
        // 复制后半部分
        strcat(new_name, name + offset);
//        LOG(".....new_name : %s\r\n", new_name);

        strcat(new_name, "\0");
//        LOG("........new_name : %s\r\n", new_name);
    }
}

/**
 * @brief
 *
 * @param wdata
 * @param objaddr
 * @param writeDataBitLen
 * @param N
 */
void fillINDataBodyFormat(void *indata, uint16_t objaddr, uint16_t writeDataBitLen, size_t N)
{
    union
    {
        void *p;
        uint8_t i8;
        uint16_t i16;
        uint32_t i32;
        float f;
        double d;
        char c;
    } datacvt;
    datacvt.p = indata;

    uint16_t BitOffset;

    for (uint8_t i = 0; i < N; i++)
    {
        memcpy(_wData + _wDataidx, &objaddr, 2); // 对象地址(对象索引)
        _wDataidx += 2;

        BitOffset = writeDataBitLen * i;
        memcpy(_wData + _wDataidx, &(BitOffset), 2); // 位偏移
        _wDataidx += 2;

        memcpy(_wData + _wDataidx, &writeDataBitLen, 2); // 位长度
        _wDataidx += 2;

        if (datacvt.i8)
        {
            memcpy(_wData + _wDataidx, &datacvt.i8, 1); // 内容
            _wDataidx += 1;
        }
        else if (datacvt.i16)
        {
            memcpy(_wData + _wDataidx, &datacvt.i16, 2); // 内容
            _wDataidx += 2;
        }
        else if (datacvt.i32)
        {
            memcpy(_wData + _wDataidx, &datacvt.i32, 4); // 内容
            _wDataidx += 4;
        }
        else if (datacvt.f)
        {
            memcpy(_wData + _wDataidx, &datacvt.f, 4); // 内容
            _wDataidx += 4;
        }
        else if (datacvt.d)
        {
            memcpy(_wData + _wDataidx, &datacvt.d, 8); // 内容
            _wDataidx += 8;
        }
        else if (datacvt.c)
        {
            memcpy(_wData + _wDataidx, &datacvt.c, 1); // 内容
            _wDataidx += 1;
        }
        else
        {
            LOG_error("datacvt.p is NULL");
        }
    }
}