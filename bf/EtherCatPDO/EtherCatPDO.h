#ifndef EtherCatPDO_h
#define EtherCatPDO_h


#include "BoardConfig.h"
#include "MSP_CRC.h"
#include "MSP_TIM.h"
#include "LOG.h"
#include "stdbool.h"


#define byte 	unsigned char

#define T35Chars		1750
#define MAX_BUFFER		200

#define word(a, b) (((uint16_t)(a) << 8) | (b))		//2个uint8_t合并为一个uint16_t
#define lowByte(x) ((uint8_t)(x))					//读取uint16_t中的低八位
#define highByte(x) ((uint8_t)((x) >> 8))		//读取uint16_t中的高八位
#define Dword(a, b) (((uint32_t)(a) << 16) | (b))	//((uint32_t)value1 << 16) | value2

typedef enum
{
	LAN9255State_DisconnectedToPLC 	= -1,
	LAN9255State_InitFailt 			= 0,
	LAN9255State_InitSuccess 		= 1,
	LAN9255State_ConnectedToPLC 	= 2,
} LAN9255State_t;

enum {
    INDIR,
    OUTDIR
};

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
//	uint8_t RecvBufSize;
//	uint8_t SendBufSize;
	uint8_t RecvBufCnt;
	uint8_t SendBufCnt;
	uint8_t TxEnd;

	uint32_t read_LAN9255State_ts;
	uint32_t read_LAN9255State_interval;

	/*需要发送给PLC的参数*/
	char *devName;

	INDATAParaHandle *INData_Para;
	OUTDATAParaHandle *OUTData_Para;
	
	LAN9255State_t Lan9255_state;
}
EPDOHandler_t;

extern EPDOHandler_t *_etherCatH;
extern uint8_t RecvBuffer[MAX_BUFFER];
extern uint8_t RecvBufferCnt;

void EtherCatPDO_Init(EPDOHandler_t *ecatH);
void EtherCatPDO_AddOBJ(uint16_t objaddr, uint8_t in_out, uint16_t bodydatatype, uint16_t bodydatabitlen, uint16_t N, const char* name);
void EtherCatPDO_process();
void EtherCatPDO_ReceiveFrame(void);
void EtherCatPDO_SendFrame(uint16_t msgid, uint16_t msgtype, byte *data, uint16_t len);
void EtherCatPDO_ProtocolAnalyze(uint16_t msgid, uint8_t msgType, byte *data, uint16_t len);
void ReadLan9255State(void);
void AddParaList(uint8_t paranum);
void StartDevice(void);
void ReadDataCmd(void);
void ReadDataFromPLC(uint16_t msgid, byte *readdata, uint16_t len);
void WriteDataToPLC(uint8_t msgtype);
void WriteDataRsp(byte *data, uint16_t len);
void setMsgCheckTimeout(void);
void writeMsgCheck(uint16_t list[], uint8_t len);
void AddreadDataAddr(uint16_t objaddr);
uint16_t fillBodyFormat(const uint16_t bdatatype, const uint16_t baccess, const uint16_t bdatabitlen, const char bname[], byte body[]);
uint16_t fillParaListFormat(uint8_t paranum, byte paralist[]);
void RestartLAN9255(void);
bool getWriteDataRsp(void);
bool getReadDataRsp(void);
//uint8_t getOUTData(OUTDataValueHandle *OUTDataList);
uint8_t getCommunicateState(void);
uint16_t addMsgID(uint16_t current_mid);
void insertCTOStr(const char *keystr, const char *insertstr, const char *name, char *new_name);
uint16_t contains(uint8_t arr[], uint16_t len, uint16_t target);
void fillINDataBodyFormat(void* indata, uint16_t objaddr, uint16_t writeDataBitLen, size_t N);


#endif // EtherCatPDO_h
