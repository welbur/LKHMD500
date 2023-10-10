#ifndef FRAME_FORMAT_h
#define FRAME_FORMAT_h

#include "stm32g4xx_hal.h"
#include "BoardConfig.h"
#include "stdbool.h"

#define MaxReceiveDataLen 		100
#define MaxFrameDataLen		    500

#define ModuletoMCU_MsgID       0
#define ReadStateMsgID          1
#define StartMsgID  		    2

#define MsgTypeBack             0x80



//#define Enable_Lan9255RST          //0: 指令复位9255模块； 1: 硬件RST引脚复位9255模块

extern bool InitCompleted;
extern bool readySendData;
extern bool FirstPowerUP;
//extern uint8_t SendCMDStatus;    //gtdStatus_t SendCMDStatus = SendCMDState_startInit;

/**
 * 自定义协议的数据帧格式
 *  _____________________________________________________________________________________
 *  |     起始     |    内容长度    |    消息ID    |    消息类型    |    内容    |    CRC    |
 *  |  0x23 0x32  |     2Byte     |    2Byte    |     1Byte     |     -     |   2Byte   |
 *  |_____________|_______________|_____________|_______________|___________|___________|
 */
/*数据帧的格式*/
const uint8_t           FrameBaseLen = 9;
const uint16_t          MaxFrameFormatLen = MaxFrameDataLen + FrameBaseLen;
const uint8_t           FrameStart_1 = 0x23;
const uint8_t	        FrameStart_2 = 0x32;
extern uint16_t 		FrameActiveDataLen;
extern uint16_t 		FrameMsgID;// = DefaultMsgID;
//uint8_t 		FrameMsgType;
//uint8_t 		FrameData[MaxFrameDataLen];


/*消息类型*/
typedef enum 
{
	RestartLAN9255_MsgType	    = 0,
	GetStatus_MsgType		    = 1,
	AddParaList_MsgType		    = 2,
	StartDevice_MsgType		    = 3,
	ReadData_MsgType		    = 4,
	WriteDataSDO_MsgType		= 5,    //需要应答
    WriteDataPDO_MsgType		= 6,    //无需应答 (用于频繁写入数据)
    SetUpdatePWD_MsgType		= 7,    //设置升级密码
    ReadUUID_MsgType			= 8,    //读取UUID
    ReadRandom_MsgType          = 9,    //获取随机数
    SetMsgTimeOut_MsgType       = 10,   //设置通知超时时间
    CheckMsg_MsgType            = 11,   //通知确认
} FrameMsgType_t;

/**
 * 消息类型 ：0x01    获取状态
 * 发送（MCU-->LAN9255）：
 *  _______________________________
 *  |     消息类型     |    内容    |
 *  |      0x01       |    NULL   |    
 *  |_________________|___________|
 * 
 * 应答（LAN9255-->MCU）
 *  _____________________________________________________________
 *  |     消息类型     |    状态值    |    版本号    ｜    更新号    ｜
 *  |      0x81       |   uint8_t   |   uint8_t   |   uint8_t    |
 *  |_________________|_____________|_____________|______________|
 * 状态值有3种 ： 0x00  等待初始化      0x01  设备已连接      0x02  设备未连接  （已初始化的状态是 0x01 或者0x02）
*/    
const uint8_t s_WaitInit        = 0x00;
const uint8_t s_Connected       = 0x01;
const uint8_t s_DisConnected    = 0x02;
const char* getDeviceState_Log[]         = {"Wait Device Init", "Device Connected", "Device Disconnected"};

/**
 * 每次添加新的参数，都在这里面添加；同步的需要把 ParaList_MaxNum 数量改下
 * 消息类型 ： 0x02     添加属性表
 * 属性表的数据格式
 *  __________________________________________________________________________
 *  |     对象地址     |    对象代码    |    对象名称    |    Body1 ... Bodyn    |
 *  |    uint16_t     |   uint16_t   |  以 ‘\0’ 结束  |                       |
 *  |_________________|______________|_______________|_______________________|
 * 对象地址 inputData : 0x6000-0x6FFF ;  outputData : 0x7000-0x7FFF
 * 对象代码  RECORD(0x00) 需要设置每个 Body 名称 ； VARIABLE(0x01) 只有一个 Body 且 Body 名称为空 ； ARRAY(0x02) 只有一个 Body 且 Body 名称为空
*/
//uint8_t ParaList_Num                   = 0;

//对象代码
const uint16_t ObjCode_Record           = 0x0000;
const uint16_t ObjCode_Variable         = 0x0001;
const uint16_t ObjCode_Array            = 0x0002;

/**
* Body的数据格式
*  __________________________________________________________________
*  |     数据类型     |    访问属性    |    数据位数    |    Body名称    |
*  |    uint16_t     |   uint16_t   |    uint16_t   |  以 ‘\0’ 结束  |
*  |_________________|______________|_______________|_______________|
*/
//body的数据类型
const uint16_t BodyDataType_DEFTYPE_BOOLEAN          = 0x0001;
const uint16_t BodyDataType_DEFTYPE_UINT8            = 0x0005;
const uint16_t BodyDataType_DEFTYPE_UINT16           = 0x0006;
const uint16_t BodyDataType_DEFTYPE_UINT32           = 0x0007;
const uint16_t BodyDataType_DEFTYPE_REAL32           = 0x0008;  //单精度浮点数

//body的访问属性 （Write + OBJACCESS_RXPDOMAPPING)   （Read + OBJACCESS_TXPDOMAPPING)
const uint16_t BodyAccess_WRITE                      = 0x0038;
const uint16_t BodyAccess_READ                       = 0x0007;
const uint16_t BodyAccess_READWRITE                  = 0x003F;
const uint16_t OBJACCESS_RXPDOMAPPING                = 0x0040;
const uint16_t OBJACCESS_TXPDOMAPPING                = 0x0080;
const uint16_t ACCESS_READ_IN                        = BodyAccess_READ + OBJACCESS_TXPDOMAPPING;
const uint16_t ACCESS_READWRITE_OUT                  = BodyAccess_READWRITE + OBJACCESS_RXPDOMAPPING;
//body的数据位数
const uint16_t BodyDataBitLen_BOOLEAN                = 1;
const uint16_t BodyDataBitLen_UINT8                  = 8;
const uint16_t BodyDataBitLen_UINT16                 = 16;
const uint16_t BodyDataBitLen_UINT32                 = 32;
const uint16_t BodyDataBitLen_REAL32                 = 32;

//对象(属性)的相关参数
typedef struct
{
    uint16_t        ObjAddr;            //同类型的对象的起始地址
    uint16_t        ObjNum;             //同类型的对象的数量 
    uint16_t        ObjCode;            //对象代码
    char            ObjName[30];        //对象名称
    uint16_t        BodyDataType;       //body的数据类型
    uint16_t        BodyAccess;         //body的访问属性
    uint16_t        BodyDataBitLen;     //body的数据位长
    uint16_t        BodyNum;            //body的数量
    char            BodyName[30];       //body名称
}OBJHandle;//对象的句柄


/**********************************************/
/*      所要添加的对象(属性) 都统一在这里定义       */
/**********************************************/
//#define ParaList_MaxNum         14   //与对象地址的数量对应

//对象地址
const uint16_t ObjAddr_DCModule_Volt      = 0x6000;     //直流模块当前的输出电压值，
const uint16_t ObjAddr_DCModule_Curr      = 0x6010;     //直流模块当前的输出电流值
const uint16_t ObjAddr_DevState           = 0x6020;     //设备的当前状态：包含当前的工作状态 和 错误状态

const uint16_t ObjAddr_DevEnable          = 0x7000;     //整个设备的使能信号
const uint16_t ObjAddr_DCModule1_SetCurr  = 0x7001;     //设置直流模块1的输出电流值
const uint16_t ObjAddr_DCModule2_SetCurr  = 0x7002;     //设置直流模块2的输出电流值
const uint16_t ObjAddr_DCModule3_SetCurr  = 0x7003;     //设置直流模块3的输出电流值
const uint16_t ObjAddr_DCModule4_SetCurr  = 0x7004;     //设置直流模块4的输出电流值
const uint16_t ObjAddr_DCModule5_SetCurr  = 0x7005;     //设置直流模块5的输出电流值

const uint16_t ObjAddr_DCModule1_SetFreq  = 0x7011;     //设置直流模块1的输出频率
const uint16_t ObjAddr_DCModule2_SetFreq  = 0x7012;     //设置直流模块2的输出频率
const uint16_t ObjAddr_DCModule3_SetFreq  = 0x7013;     //设置直流模块3的输出频率
const uint16_t ObjAddr_DCModule4_SetFreq  = 0x7014;     //设置直流模块4的输出频率
const uint16_t ObjAddr_DCModule5_SetFreq  = 0x7015;     //设置直流模块5的输出频率

const uint16_t ObjAddr_DCModule1_SetPWM   = 0x7021;     //设置直流模块1的输出占空比
const uint16_t ObjAddr_DCModule2_SetPWM   = 0x7022;     //设置直流模块2的输出占空比
const uint16_t ObjAddr_DCModule3_SetPWM   = 0x7023;     //设置直流模块3的输出占空比
const uint16_t ObjAddr_DCModule4_SetPWM   = 0x7024;     //设置直流模块4的输出占空比
const uint16_t ObjAddr_DCModule5_SetPWM   = 0x7025;     //设置直流模块5的输出占空比


#if 0
OBJHandle OBJ_DCModule_Volt     = { 
    .ObjAddrStart           = ObjAddr_DCModule_Volt, 
    .OBNum                  = DCModuleNum, 
    .ObjCode                = ObjCode_Record, 
    .BodyDataType           = BodyDataType_DEFTYPE_REAL32, 
    .BodyAccess             = ACCESS_READ_IN, 
    .BodyDataBitLen         = BodyDataBitLen_REAL32, 
    .OBName                 = "DCModule Voltage\0" };
OBJHandle OBJ_DCModule_Curr     = { 
    .ObjAddrStart           = ObjAddr_DCModule_Curr, 
    .OBNum                   = DCModuleNum, 
    .ObjCode                = ObjCode_Record, 
    .BodyDataType           = BodyDataType_DEFTYPE_REAL32, 
    .BodyAccess             = ACCESS_READ_IN, 
    .BodyDataBitLen         = BodyDataBitLen_REAL32, 
    .OBName                 = "DCModule Current\0" }; 
OBJHandle OBJ_DevState     = { 
    .ObjAddrStart           = ObjAddr_DevState, 
    .OBNum                   = 1, 
    .ObjCode                = ObjCode_Record, 
    .BodyDataType           = BodyDataType_DEFTYPE_UINT16, 
    .BodyAccess             = ACCESS_READ_IN, 
    .BodyDataBitLen         = BodyDataBitLen_UINT16, 
    .OBName                = "Device State\0" }; 

OBJHandle OBJ_DevEnable     = { 
    .ObjAddrStart           = ObjAddr_DevEnable, 
    .OBNum                   = 1, 
    .ObjCode                = ObjCode_Variable, 
    .BodyDataType           = BodyDataType_DEFTYPE_UINT8, 
    .BodyAccess             = ACCESS_READWRITE_OUT, 
    .BodyDataBitLen         = BodyDataBitLen_UINT8, 
    .OBName                = "Enable Device\0" }; 
OBJHandle OBJ_DCModule_SetCurr     = { 
    .ObjAddrStart           = ObjAddr_DCModule_SetCurr, 
    .OBNum                   = DCModuleNum, 
    .ObjCode                = ObjCode_Variable, 
    .BodyDataType           = BodyDataType_DEFTYPE_REAL32, 
    .BodyAccess             = ACCESS_READWRITE_OUT, 
    .BodyDataBitLen         = BodyDataBitLen_REAL32, 
    .OBName                = "DCModule Current\0" }; 
OBJHandle OBJ_DCModule_SetFreq     = { 
    .ObjAddrStart           = ObjAddr_DCModule_SetFreq, 
    .OBNum                   = DCModuleNum, 
    .ObjCode                = ObjCode_Variable, 
    .BodyDataType           = BodyDataType_DEFTYPE_UINT16, 
    .BodyAccess             = ACCESS_READWRITE_OUT, 
    .BodyDataBitLen         = BodyDataBitLen_UINT16, 
    .OBName                = "DCModule Frequency\0" }; 
OBJHandle OBJ_DCModule_SetPWM     = { 
    .ObjAddrStart           = ObjAddr_DCModule_SetPWM, 
    .OBNum                   = DCModuleNum, 
    .ObjCode                = ObjCode_Variable, 
    .BodyDataType           = BodyDataType_DEFTYPE_UINT16, 
    .BodyAccess             = ACCESS_READWRITE_OUT, 
    .BodyDataBitLen         = BodyDataBitLen_UINT16, 
    .OBName                = "DCModule PWM\0" }; 
#endif

//const uint8_t ParaList_MaxNum           = 7;   //与对象地址的数量对应
//const uint8_t INDATA_MaxNum             = 3;
//const uint8_t OUTDATA_MaxNum            = 4;
//extern OBJHandle OBJ_H[ParaList_MaxNum];
//OBJHandle OBJ_H[ParaList_MaxNum] = { 
//    OBJ_LoadStatus, OBJ_INOW };


/**
 * 消息类型 ： 0x03     启动通信
 * 启动通信的数据格式
 *  _________________________________________________________________________________________________
 *  |     供应商ID     |    产品代码    |    修订号    |    产品序号    |    厂商名称    ｜    设备名称     ｜
 *  |    uint32_t     |   uint32_t   |    uint32_t  |   uint32_t   |  以 ‘\0’ 结束  |   以 ‘\0’ 结束  |
 *  |_________________|______________|______________|______________|_______________|________________|
*/
const uint32_t venderID                 = 0x00001234;   //正常是由ethercat联盟提供，暂时随机填一个
const uint32_t productCode              = 0x20231002;   //变更属性表需要更改产品代码
const uint32_t revisionNumber           = 0x00000001;   //修订号
const uint32_t productSN                = 0x20230200;   //产品序号 前四个表示年份，后4个表示型号 0x20230200表示2023年开发的型号为LKHM200
const char CompanyName[]                = "Butup\0";
const char* Result_Log[]                = {"Error!", "Success!"};          //const String Result_Log[]               = {"Error!", "Success!"};
//char *DeviceName;   //[6];
//const PROGMEM char DeviceName[]                       = "ESSR1\0";            


/**
 * 消息类型 ： 0x04     读取数据
 * 需要确保返回帧长度不超过 1024
 * 发送（MCU-->LAN9255）：
 *  ______________________________________
 *  |     消息类型     |    对象地址列表    |
 *  |      0x04       |   uint16_t * N   | 
 *  |_________________|__________________|
 * 
 * 应答（LAN9255-->MCU）
 *  _______________________________________________________________________________________________________________________
 *  |     消息类型     |    对象地址    |    数据位长度    ｜    内容    ｜ ..... |    对象地址    |    数据位长度    ｜    内容    ｜
 *  |      0x84       |   uint16_t   |     uint16_t    |     -      |       |    uint16_t   |    uint16_t     |     -      |
 *  |_________________|______________|_________________|____________|_______|_______________|_________________|____________|
 *
 *  其中 消息 id = 0 表示模块主动通知，当数据有变化时模块会主动发送通知 
 * 数据位长度 = 0 :表示读取错误，没有这个属性
 * 
*/


/**
 * 消息类型 ： 0x05/0x06     写入数据
 * 0x05  需要应答
 * 0x06  无需应答(用于频繁写入数据)
 * 发送（MCU-->LAN9255）：
 *  ___________________________________________________________________________________________________________________________________________________
 *  |     消息类型     |    对象地址    |    位偏移    |    数据位长度    ｜    内容    ｜ ..... |    对象地址    |    位偏移    |    数据位长度    ｜    内容    ｜
 *  |    0x05/0x06    |   uint16_t   |   uint16_t  |     uint16_t    |     -      |       |    uint16_t   |   uint16_t  |    uint16_t     |     -      |
 *  |_________________|______________|_____________|_________________|____________|_______|_______________|_____________|_________________|____________|
 *
 * 应答（LAN9255-->MCU）
 *  ___________________________________________________________________________________
 *  |     消息类型     |    对象地址    |    结果    ｜ ..... |    对象地址    |    结果    ｜
 *  |      0x85       |   uint16_t   |   uint8_t  |       |    uint16_t   |   uint8_t  |
 *  |_________________|______________|____________|_______|_______________|____________|
 *
*/


/**
 * 消息类型 ： 0x07     设置升级密码
 * 发送（MCU-->LAN9255）：
 *  __________________________________
 *  |     消息类型     |    升级密码    |
 *  |      0x07       |   uint32_t   |
 *  |_________________|______________|
 *
 * 应答（LAN9255-->MCU）
 *  __________________________________
 *  |     消息类型     |    升级密码    |
 *  |      0x87       |   uint32_t   |
 *  |_________________|______________|
 *
*/


/**
 * 消息类型 ： 0x08     唯一识别码UUID
 * 发送（MCU-->LAN9255）：
 *  _______________________________
 *  |     消息类型     |    内容    |
 *  |      0x08       |    --     |
 *  |_________________|___________|
 *
 * 应答（LAN9255-->MCU）
 *  ________________________________
 *  |     消息类型     |    UUID     |
 *  |      0x88       |   16Byte   |
 *  |_________________|____________|
 *
*/


/**
 * 消息类型 ： 0x09     获取随机数
 * 发送（MCU-->LAN9255）：
 *  _______________________________
 *  |     消息类型     |    内容    |
 *  |      0x09       |    --     |
 *  |_________________|___________|
 *
 * 应答（LAN9255-->MCU）
 *  _______________________________
 *  |     消息类型     |    RNG     |
 *  |      0x89       | uint32_t  |
 *  |_________________|___________|
 *
*/


/**
 * 消息类型 ： 0x0A     设置确认超时
 * 不频繁写入且关键的数据需要进行设置
 * 
 * 发送（MCU-->LAN9255）：
 *  _______________________________________________________
 *  |     消息类型     |    超时时间ms    |    对象地址列表    |
 *  |      0x0A       |    uint16_t    |   uint16_t * N   | 
 *  |_________________|________________|__________________|
 *
 * 应答（LAN9255-->MCU）
 *  ____________________________________
 *  |     消息类型     |    超时时间ms    |
 *  |      0x8A       |    uint16_t    |
 *  |_________________|________________|
 *
*/


/**
 * 消息类型 ： 0x0B     通知确认
 * 当9255主动发送消息给MCU时(消息ID = 0)，需要发送此通知确认给9255
 * 
 * 发送（MCU-->LAN9255）：
 *  ______________________________________
 *  |     消息类型     |    对象地址列表    |
 *  |      0x0B       |   uint16_t * N   | 
 *  |_________________|__________________|
 *
*/

#endif