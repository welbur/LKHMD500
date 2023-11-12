#ifndef EtherCatPDO_h
#define EtherCatPDO_h


#include "BoardConfig.h"
#include "MSP_CRC.h"
#include "MSP_TIM.h"
#include "MSP_GPIO.h"
#include "LOG.h"
#include "stdbool.h"
//#include "FrameFormat.h"

#define MaxRepeatCnt 10		//100

#define byte 	unsigned char

#define T35Chars		1750
#define MAX_BUFFER		200

#define SDO_MsgType		5    //需要应答
#define PDO_MsgType		6    //无需应答 (用于频繁写入数据)

#define word(a, b) (((uint16_t)(a) << 8) | (b))		//2个uint8_t合并为一个uint16_t
#define lowByte(x) ((uint8_t)(x))					//读取uint16_t中的低八位
#define highByte(x) ((uint8_t)((x) >> 8))		//读取uint16_t中的高八位
#define Dword(a, b) (((uint32_t)(a) << 16) | (b))	//((uint32_t)value1 << 16) | value2


#define LAN9255_RST(n)			(n?HAL_GPIO_WritePin(D51_RSTn_Port,D51_RSTn_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(D51_RSTn_Port,D51_RSTn_Pin,GPIO_PIN_RESET))
#define Read_LAN9255_RST()		HAL_GPIO_ReadPin(D51_RSTn_Port,D51_RSTn_Pin)

typedef enum
{
	LAN9255State_DisconnectedToPLC 	= -1,
	LAN9255State_Default			= 0,
	LAN9255State_RestartLAN9255		= 1,
	LAN9255State_WaitInit			= 2,
	LAN9255State_Initting			= 3,
	LAN9255State_InitFailt 			= 4,
	LAN9255State_InitSuccess 		= 5,
	LAN9255State_ConnectedToPLC 	= 6,
	LAN9255State_WriteDataToPLC		= 7,
	LAN9255State_ReadDataFromPLC	= 8,
} LAN9255State_t;

enum {
    INDIR,
    OUTDIR
};

enum {
    s_WaitInit,
    s_Connected,
	s_DisConnected,
};

/**
 * @brief 用于记录msg的信息
 * 
 */
typedef struct
{
	uint16_t MsgID;
	uint8_t MsgType; 
	byte *data;
	uint16_t len;
}
EMessageHandler_t;

//对象(属性)的相关参数
typedef struct
{
    uint16_t        ObjAddr;            //同类型的对象的起始地址
    uint16_t        ObjCode;            //对象代码
    char            ObjName[30];        //对象名称
    void            *DataValue;          //对象的数值
	DataTypeHandle  DataType;          	//对象的数据类型
    uint16_t        BodyDataType;       //body的数据类型
    uint16_t        BodyAccess;         //body的访问属性
    uint16_t        BodyDataBitLen;     //body的数据位长
    uint16_t        BodyNum;            //body的数量
    char            BodyName[30];       //body名称
}OBJHandle;//对象的句柄
extern OBJHandle OBJ_H[30];

/**
 * @struct BackPanelHandler_t
 * @brief
 * BackPanel Trans handler structure
 * Contains all the variables required for Modbus daemon operation
 */
typedef struct
{
	UART_HandleTypeDef *port; //HAL Serial Port handler

	uint16_t t35chars;
	uint32_t RecvLastTime;

	uint8_t RecvData;
 	uint8_t RecvBuf[MAX_BUFFER];
 	uint8_t SendBuf[MAX_BUFFER];
	uint8_t RecvBufCnt;
	uint8_t SendBufCnt;
	uint8_t TxEnd;
	uint8_t RxEnd;

	uint32_t read_LAN9255State_ts;
	uint32_t read_LAN9255State_interval;

	/*需要发送给PLC的参数*/
	char *devName;
	
	LAN9255State_t Lan9255_state;
}
EPDOHandler_t;


extern uint16_t KeyOUTData_TimeOut;
extern uint16_t KeyOUTDataAddr_PLCToMCU[2];

extern uint8_t ParaList_Num;    //            = 0;   //用于将属性表发送给PLC的计数
extern uint8_t ObjList_Num;     //             = 0;    //用于添加属性表的计数


extern EPDOHandler_t *_etherCatH;

extern char* getDeviceState_Log[];	//         = {"Wait Device Init", "Device Connected", "Device Disconnected"};
extern char* Result_Log[];

void EtherCatPDO_Init(EPDOHandler_t *ecatH);
void EtherCatPDO_AddOBJ(void *indata, DataTypeHandle dt, uint8_t in_out, uint16_t N, const char *name);
//void EtherCatPDO_AddOBJ(uint16_t objaddr, uint8_t in_out, uint16_t bodydatatype, uint16_t bodydatabitlen, uint16_t N, const char* name);
void EtherCatPDO_process();
//uint8_t EtherCatPDO_ReceiveFrame(EMessageHandler_t * emsgH);
uint8_t EtherCatPDO_ReceiveFrame(void);
void EtherCatPDO_SendFrame(uint16_t msgid, uint16_t msgtype, byte *data, uint16_t len);
void ReadLAN9255State_SendCMD(void);
void AddParaList_SendCMD(uint8_t paranum);
void StartDevice_SendCMD(void);
void ReadDataFromLAN9255_SendCMD(void);
//uint8_t ReadDataFromLAN9255_RecvRSP(uint16_t msgid, byte *readdata, uint16_t len);
uint8_t ReadDataFromLAN9255_RecvRSP(EMessageHandler_t *emsgh);
void WriteDataToLAN9255_SendCMD(uint8_t msgtype);		
void WriteDatatoLAN9255_RecvRSP(byte *data, uint16_t len);
void setMsgCheckTimeout_SendCMD(void);
void WriteMsgCheck_SendCMD(uint16_t list[], uint8_t len);
void RestartLAN9255_SendCMD(void);
//void AddreadDataAddr(uint16_t objaddr);
uint16_t fillBodyFormat(const uint16_t bdatatype, const uint16_t baccess, const uint16_t bdatabitlen, const char bname[], byte body[]);
uint16_t fillParaListFormat(uint8_t paranum, byte paralist[]);
void fillINDataBodyFormat(OBJHandle *objH);
//void fillINDataBodyFormat(void* indata, uint16_t objaddr, uint16_t writeDataBitLen, size_t N);

//uint8_t getDataSize(void *indata);
uint16_t addMsgID(uint16_t current_mid);
uint16_t contains(uint8_t arr[], uint16_t len, uint16_t target);
void insertCTOStr(const char *keystr, const char *insertstr, const char *name, char *new_name);
//void setINDataBuffer(INDATAValueHandle inpara);

bool getWriteDataRsp(void);
bool getReadDataRsp(void);
//uint8_t getOUTData(OUTDataValueHandle *OUTDataList);
uint8_t getCommunicateState(void);

#endif // EtherCatPDO_h
