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
#include <stdlib.h>

// extern uint16_t DeviceStateInfo;

/* 用于数据帧数据的填充 */
byte _wData[200];   
uint16_t _wDataidx = 0;

/* 关键的输出属性，需要有应答 */
uint16_t KeyOUTData_TimeOut = 1000;
uint16_t KeyOUTDataAddr_PLCToMCU[2] = {ObjAddr_DevEnable, ObjAddr_DCModule_Curr};


uint8_t ParaList_Num;    //            = 0;   //用于将属性表发送给PLC的计数
uint8_t ObjList_Num                    = 0;    //用于添加属性表的计数
static uint8_t Obj_INDATA_Num          = 0;    //用于统计输入属性的数量
static uint8_t Obj_OUTDATA_Num         = 0;    //用于统计输出属性的数量
uint8_t _OUTDATA_Change[32];                    //用于标记哪个“输出属性”的数值发生改变了


uint16_t FrameMsgID = StartMsgID;
static uint16_t MaxMsgID = 1023;


EPDOHandler_t *_etherCatH;

const char *ntoc[9] = {"_1", "_2", "_3", "_4", "_5", "_6", "_7", "_8", "_9"}; //"," 3"," 4"," 5"," 6"," 7"," 8"," 9"};

char* getDeviceState_Log[]         = {"Wait Device Init", "Device Connected", "Device Disconnected"};
char* Result_Log[]                = {"Error!", "Success!"};   

OBJHandle OBJ_H[30];

void EtherCatPDO_Init(EPDOHandler_t *ecatH)
{
    LAN9255_RST(0);
    HAL_Delay(100);
    LAN9255_RST(1);

    ecatH->TxEnd = 1;       //txend = 1表示发送完成
    ecatH->RxEnd = 1;       //rxend = 1表示接收完成

    /* 初始化接收缓冲区 和发送缓冲区*/
    ecatH->RecvBufCnt = 0;
    ecatH->SendBufCnt = 0;
    ecatH->RecvData = 0;
    memset(ecatH->RecvBuf, 0, MAX_BUFFER); // ecatH->RecvBufSize);
    memset(ecatH->SendBuf, 0, MAX_BUFFER); // ecatH->SendBufSize);

    /* 设置t35chars时间 */
    ecatH->t35chars = T35Chars;

    ecatH->RecvLastTime = 0;

    /**/
//    ecatH->read_LAN9255State_ts = 0;
//    if (ecatH->read_LAN9255State_interval == 0)
//        ecatH->read_LAN9255State_interval = 1000;

    _etherCatH = ecatH;

    /*初始化相关变量*/
    _wDataidx = 0;    // 用于发送数据帧时，计算发送数据的数量
    ParaList_Num = 0; // 用于将属性表发送给PLC的计数
    memset(_OUTDATA_Change, 0, Obj_OUTDATA_Num);


    // Receive data from serial port for Modbus using interrupt
    if (HAL_UART_Receive_IT(_etherCatH->port, &_etherCatH->RecvData, 1) != HAL_OK) {
        Error_Handler();
    }
    LOG_info("EtherCatPDOClass begin\r\n");
}

//void EtherCatPDO_AddOBJ(uint16_t objaddr, uint8_t in_out, uint16_t bodydatatype, uint16_t bodydatabitlen, uint16_t N, const char *name)
void EtherCatPDO_AddOBJ(void *indata, DataTypeHandle dt, uint8_t in_out, uint16_t bodyn, const char *name)
{
    uint16_t objaddr = 0;
    uint16_t objcode = 0;
    uint16_t bodydatatype;
    uint16_t bodydatabitlen;
    uint16_t bodyaccess = 0;
    uint16_t bodynum = 0;
    char bodyname[30];

    OBJHandle OBJ_Cache;

    /* 设置 bodydatatype 和 bodydatabitlen */
    if (dt == TYPE_BOOL)
    {
        bodydatatype = BodyDataType_DEFTYPE_BOOLEAN;
        bodydatabitlen = BodyDataBitLen_BOOLEAN;
    }
    else if (dt == TYPE_UINT8)
    {
        bodydatatype = BodyDataType_DEFTYPE_UINT8;
        bodydatabitlen = BodyDataBitLen_UINT8;
    }
    else if (dt == TYPE_UINT16)
    {
        bodydatatype = BodyDataType_DEFTYPE_UINT16;
        bodydatabitlen = BodyDataBitLen_UINT16;
    } 
    else if (dt == TYPE_UINT32)
    {
        bodydatatype = BodyDataType_DEFTYPE_UINT32;
        bodydatabitlen = BodyDataBitLen_UINT32;
    }
    else if (dt == TYPE_FLOAT)
    {
        bodydatatype = BodyDataType_DEFTYPE_REAL32;
        bodydatabitlen = BodyDataBitLen_REAL32;
    }
    else if (dt == TYPE_DOUBLE)
    {
        bodydatatype = BodyDataType_DEFTYPE_REAL64;
        bodydatabitlen = BodyDataBitLen_REAL64;
    }
    else
    {
        bodydatatype = BodyDataType_DEFTYPE_UINT8;
        bodydatabitlen = BodyDataBitLen_UINT8;
    }
    
    /*设置body的读取方向*/
    if (in_out == INDIR)
    {
        objaddr = ObjAddr_INData_StartAddr + Obj_INDATA_Num;
        bodyaccess = ACCESS_READ_IN;
        objcode = ObjCode_Record;
        bodynum = bodyn;
        Obj_INDATA_Num++;
        strcpy(bodyname, name);
    }
    else if (in_out == OUTDIR)
    {
        objaddr = ObjAddr_OUTData_StartAddr + Obj_OUTDATA_Num;
        bodyaccess = ACCESS_READWRITE_OUT;
        objcode = ObjCode_Variable;
        bodynum = bodyn;
        Obj_OUTDATA_Num++;      //= objnum;
        strcpy(bodyname, "\0");
    }

//    _dataph->addr_offset = objaddr;
    #if 0
    OBJHandle OBJ_Cache = {
        .ObjAddr = objaddr,
        .ObjCode = objcode,
        .BodyDataType = bodydatatype,
        .BodyAccess = bodyaccess,
        .BodyDataBitLen = bodydatabitlen,
        .BodyNum = bodynum,
        //    .OBName = name
    };
    #endif
    OBJ_Cache.DataValue = indata;
    OBJ_Cache.DataType = dt;
    OBJ_Cache.ObjAddr = objaddr;
    OBJ_Cache.ObjCode = objcode;
    OBJ_Cache.BodyDataType = bodydatatype;
    OBJ_Cache.BodyAccess = bodyaccess;
    OBJ_Cache.BodyDataBitLen = bodydatabitlen;
    OBJ_Cache.BodyNum = bodynum;
    
    strncpy(OBJ_Cache.ObjName, name, strlen(name) + 1);      //sizeof(OBJ_Cache.OBName) - 1);
    strncpy(OBJ_Cache.BodyName, bodyname, strlen(bodyname) + 1); 
    OBJ_H[ObjList_Num] = OBJ_Cache;
    ObjList_Num++;
    
}


void EtherCatPDO_process()
{
//    EMessageHandler_t eMsgH;
//    uint8_t recvState;  // = EtherCatPDO_ReceiveFrame(&eMsgH);
}

/**
 * @brief 循环读取串口的数据帧(9255发送过来的)
 * 读取完数据帧，再解析,然后用eos状态机去执行相应的指令
 */
uint8_t EtherCatPDO_ReceiveFrame(void)
{
    if (_etherCatH->RecvBufCnt <= 0 || (_etherCatH->RxEnd == 0))            //if (_etherCatH->RecvBufCnt <= 0 || (t32c < _etherCatH->t35chars))
    {
        return 0;
    }
    uint8_t recCnt = _etherCatH->RecvBufCnt;
    _etherCatH->RecvBufCnt = 0;

//    LOG("current tick us : %ld\r\n", t32c);
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
    EMessageHandler_t eMsgH;
    eMsgH.MsgID = msgid;
    eMsgH.MsgType = msgType;
    eMsgH.len = inLen - 9; 
    eMsgH.data = _etherCatH->RecvBuf + 7;
    /* 复位LAN9255 */
    if ((eMsgH.MsgType - MsgTypeBack) == RestartLAN9255_MsgType)  // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_RestartLAN9255S_RecvRSP, (void *)eMsgH.data, eMsgH.len);        
    
    /* 获取LAN9255的状态 */
    else if ((eMsgH.MsgType - MsgTypeBack) == GetStatus_MsgType)       // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_ReadLAN9255State_RecvRSP, (void *)eMsgH.data, eMsgH.len);
    
    /* 添加属性表*/
    else if ((eMsgH.MsgType - MsgTypeBack) == AddParaList_MsgType)          // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_AddParaList_RecvRSP, (void *)eMsgH.data, eMsgH.len);
    
    /* 设置关键属性的超时时间 */
    else if ((eMsgH.MsgType - MsgTypeBack) == SetMsgTimeOut_MsgType)            // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_SetMsgTimeout_RecvRSP, (void *)eMsgH.data, eMsgH.len);
    
    /* 启动LAN9255 */
    else if ((eMsgH.MsgType - MsgTypeBack) == StartDevice_MsgType)      // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_StartLAN9255_RecvRSP, (void *)eMsgH.data, eMsgH.len);
    
    /* 写入数据到PLC，SDO模式，需要应答 */
    else if ((eMsgH.MsgType - MsgTypeBack) == WriteDataSDO_MsgType)       // 0x81 - 0x80 = 0x01
        eos_event_pub(Event_ECAT_WriteDataToLAN9255_RecvRSP, (void *)eMsgH.data, eMsgH.len);
    
    /* 从PLC读取数据 */
    else if ((eMsgH.MsgType - MsgTypeBack) == ReadData_MsgType)        // 0x81 - 0x80 = 0x01
    {
        ReadDataFromLAN9255_RecvRSP(&eMsgH);
        //eos_event_pub(Event_ECAT_ReadDataFromLAN9255_RecvRSP, (void *)_OUTDATA_Change, Obj_OUTDATA_Num);
    }
    return 1;
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
    if (_etherCatH->TxEnd) {
        _etherCatH->TxEnd = 0;
        HAL_UART_Transmit_IT(_etherCatH->port, _etherCatH->SendBuf, idx);
    }
}

/**
 * @brief 读取9255的状态
 * 消息类型： 0x01
 * 默认消息ID ： 1
 *
 */
void ReadLAN9255State_SendCMD(void)
{
    byte Data[0];
    uint16_t DataLen = 0;

    EtherCatPDO_SendFrame(ReadStateMsgID, GetStatus_MsgType, Data, DataLen);
}

/**
 * @brief 添加属性表, 采用Record类型
 * 消息类型： 0x02
 * @param paraidx ：指向OBJ_H中的第几个属性
 */
void AddParaList_SendCMD(uint8_t paraidx)
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
void StartDevice_SendCMD(void)
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
void ReadDataFromLAN9255_SendCMD(void)
{
    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[200];
    uint16_t idx = 0;

    // 添加对象地址列表
    for (int i = 0; i < Obj_OUTDATA_Num; i++)
    {
//        LOG("ReadDataFromLAN9255_SendCMD objaddr : %d\r\n", OBJ_H[i + Obj_INDATA_Num].ObjAddr);
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
//uint8_t ReadDataFromLAN9255_RecvRSP(uint16_t msgid, byte *readdata, uint16_t len)
uint8_t ReadDataFromLAN9255_RecvRSP(EMessageHandler_t *emsgh)
{
    if (emsgh->len < 4)
    {
        LOG_error("read Data from PLC Fault!\r\n");
        return 0;
    }
    uint16_t robjaddrlist[32];
    uint8_t outdata_change[5];
    uint8_t outdata_change_idx = 0;
    uint8_t listLen = 0;
    uint16_t idx = 0;
    while (idx < emsgh->len)
    {
        uint16_t objaddr = word(emsgh->data[idx + 1], emsgh->data[idx]);    //idx + 1], readdata[idx]); //, data[0]);
        idx += 2;

        robjaddrlist[listLen] = objaddr;

        uint16_t datalen = word(emsgh->data[idx + 1], emsgh->data[idx]) / 8;
        idx += 2;
        if (datalen == 0)
        {
            LOG_error("read Data from PLC Fault! \r\n");
            return 0;
        }
        uint8_t obj_outdata_i = objaddr - ObjAddr_OUTData_StartAddr;
        
        /* 判断是哪个通道的参数发生变化 */
        uint8_t outdata_change_num = obj_outdata_i;
        if (obj_outdata_i < DCModuleNum)
            outdata_change_num = obj_outdata_i;
        else if ((obj_outdata_i >= DCModuleNum) && (obj_outdata_i < DCModuleNum * 2))
            outdata_change_num = obj_outdata_i - DCModuleNum;
#ifdef USE_AdjustFunc
        else if (obj_outdata_i >= DCModuleNum * 2)
            outdata_change_num = obj_outdata_i - DCModuleNum * 2;
#endif
        else
        {
            LOG_error("read Data from PLC Fault! ......outdata_change_i error...%d!\r\n", obj_outdata_i);
            return 0;
        }
        outdata_change[outdata_change_idx] = outdata_change_num;    //将发生变化的通道保存到数组中，
        outdata_change_idx++;

        obj_outdata_i = Obj_INDATA_Num + obj_outdata_i;

//        OBJ_H[obj_outdata_i].DataValue = (void *)malloc(datalen);
        memcpy(OBJ_H[obj_outdata_i].DataValue, emsgh->data + idx, datalen);
        
        listLen++;
        idx += datalen;
    }

    if (emsgh->MsgID == 0) // 发送通知确认给9255
        WriteMsgCheck_SendCMD(robjaddrlist, listLen);

    eos_event_pub(Event_ECAT_ReadDataFromLAN9255_RecvRSP, (void *)outdata_change, outdata_change_idx);

    return 1;
}

/**
 * @brief 往LAN9255写入数据, 无需应答和需要应答 2种类型
 *
 * @param msgtype : 消息类型 0x05(需要应答) 或者 0x06(无需应答)
 */
void WriteDataToLAN9255_SendCMD(uint8_t msgtype)   
{
    FrameMsgID = addMsgID(FrameMsgID);
    for(int i = 0; i < Obj_INDATA_Num; i++)
    {

#if 0
        LOG("indata num : %d....", Obj_INDATA_Num);
        float *fv = (float *)OBJ_H[i].DataValue;
        for (int j = 0; j < OBJ_H[i].BodyNum; j++)
        {
            LOG(" %.2f", fv[j]);
        }
        LOG("\r\n");
#endif

        fillINDataBodyFormat(&OBJ_H[i]);         //(OBJ_H[i].DataValue, OBJ_H[i].ObjAddr, OBJ_H[i].BodyDataBitLen, OBJ_H[i].BodyNum);
    }
    
    EtherCatPDO_SendFrame(FrameMsgID, msgtype, _wData, _wDataidx);
    _wDataidx = 0;
}

/**
 * @brief 解析 从9255发送过来的应答(消息类型0x05的应答)
 *
 * @param data
 * @param len
 */
void WriteDatatoLAN9255_RecvRSP(byte *data, uint16_t len)
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
 //           writeDataSuccess = false;
            return;
        }
    }
//    writeDataSuccess = true;
    return;
}

/**
 * @brief 设置通知确认的超时时间
 *        不频繁写入且关键的数据需要进行设置
 */
void setMsgCheckTimeout_SendCMD(void)
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
void WriteMsgCheck_SendCMD(uint16_t list[], uint8_t len)
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
 * @brief 重启模块: 重启模块有2个方法，一个指令重启，一个rst复位重启，最好用rst复位重启
 *
 */
void RestartLAN9255_SendCMD(void)
{
    LOG_info("reStart Lan9255 ! \r\n"); // addMessage("Restart LAN9255 ! \r\n");

    FrameMsgID = addMsgID(FrameMsgID);
    byte Data[0];
    uint16_t DataLen = 0;
    EtherCatPDO_SendFrame(FrameMsgID, RestartLAN9255_MsgType, Data, DataLen);
}

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

            bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, new_bodyname, bodycache);
            memcpy(paralist + idx, bodycache, bodycacheLen);
            idx += bodycacheLen;
        }
    }
    else if (OBJ_H[paraidx].ObjCode == ObjCode_Variable)
    {
//        uint8_t objNum = OBJ_H[paraidx].ObjNum;
        // 添加对象名称
        uint16_t objnameLen = strlen(OBJ_H[paraidx].ObjName) + 1;
        memcpy(paralist + idx, OBJ_H[paraidx].ObjName, objnameLen);
        idx += objnameLen;

        // 添加body
        bodycacheLen = fillBodyFormat(OBJ_H[paraidx].BodyDataType, OBJ_H[paraidx].BodyAccess, OBJ_H[paraidx].BodyDataBitLen, OBJ_H[paraidx].BodyName, bodycache);
        memcpy(paralist + idx, bodycache, bodycacheLen);
        idx += bodycacheLen;
    }
    return idx;
}


/**
 * @brief
 *
 * @param wdata
 * @param objaddr
 * @param writeDataBitLen
 * @param N
 */
//void fillINDataBodyFormat(void *indata, uint16_t objaddr, uint16_t writeDataBitLen, size_t N)
void fillINDataBodyFormat(OBJHandle *objH)
{
    uint16_t BitOffset;
    for (uint8_t i = 0; i < objH->BodyNum; i++)
    {
        memcpy(_wData + _wDataidx, &objH->ObjAddr, 2); // 对象地址(对象索引)
        _wDataidx += 2;

        BitOffset = objH->BodyDataBitLen * i;
        memcpy(_wData + _wDataidx, &(BitOffset), 2); // 位偏移
        _wDataidx += 2;

        memcpy(_wData + _wDataidx, &objH->BodyDataBitLen, 2); // 位长度
        _wDataidx += 2;

        uint8_t bytenum = DIVIDE_AND_ROUND(objH->BodyDataBitLen, 8);  
        
        if (objH->DataType == TYPE_UINT8)
            memcpy(_wData + _wDataidx, &((uint8_t *)objH->DataValue)[i], bytenum); // 内容
        else if (objH->DataType == TYPE_UINT16)
            memcpy(_wData + _wDataidx, &((uint16_t *)objH->DataValue)[i], bytenum); // 内容
        else if (objH->DataType == TYPE_UINT32)
            memcpy(_wData + _wDataidx, &((uint32_t *)objH->DataValue)[i], bytenum); // 内容
        else if (objH->DataType == TYPE_FLOAT)
            memcpy(_wData + _wDataidx, &((float *)objH->DataValue)[i], bytenum); // 内容
        //memcpy(_wData + _wDataidx, &fv[i], bytenum); // 内容
        
        _wDataidx += bytenum;
    }
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
    char *insert_pos = strstr(name, keystr);     // 找到"DCM"在字符串中的位置
    if (insert_pos != NULL) {
        // 计算"DCM"的长度和它在字符串中的位置
        int dcm_len = strlen(keystr);
        int offset = insert_pos - name + dcm_len;

        // 复制前半部分
        strncpy(new_name, name, offset);
        new_name[offset] = '\0';  // 添加空字符

        // 插入数字"1"
        strcat(new_name, insertstr);
        // 复制后半部分
        strcat(new_name, name + offset);

        strcat(new_name, "\0");
    }
}
