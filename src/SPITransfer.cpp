//#include "Arduino.h"

//#if not(defined(MBED_H) || defined(__SAM3X8E__) || defined(DISABLE_SPI_SERIALTRANSFER)) // These boards are/will not be supported by SPITransfer.h

#include "SPITransfer.h"
//#include "SPITransfer_C.h"

/*!
 *    @brief  Create an SPI device with the given CS pin and settings
 *    @param  cspin The arduino pin number to use for chip select
 *    @param  freq The SPI clock frequency to use, defaults to 1MHz
 *    @param  dataOrder The SPI data order to use for bits within each byte,
 * defaults to SPI_BITORDER_MSBFIRST
 *    @param  dataMode The SPI mode to use, defaults to SPI_MODE0
 *    @param  theSPI The SPI bus to use, defaults to &theSPI
 */
#if 1
SPITransfer::SPITransfer(CHIPHandler_t *chipH, SPI_HandleTypeDef *theSPI, bool master) 
{
	_chipH = chipH;
  	_spi = theSPI;
	_master = master;
	_cs = _chipH->DChipID;
	begin(false);
	_begun = false;
}
#endif

/*
 void SPITransfer::begin(SPIClass &_port, configST configs, const uint8_t &_SS)
 Description:
 ------------
  * Advanced initializer for the SPITransfer Class
 Inputs:
 -------
  * const SPIClass &_port - SPI port to communicate over
  * const configST configs - Struct that holds config
  * const uint8_t &_SS - SPI buslave select pin used
  values for all possible initialization parameters
 Return:
 -------
  * void
*/
void SPITransfer::begin(const configST configs)
{
	packet.begin(configs);
	_begun = true;
}

/*
 void SPITransfer::begin(SPIClass &_port, const uint8_t &_SS, const bool _debug, Stream &_debugPort)
 Description:
 ------------
  * Simple initializer for the SPITransfer Class
 Inputs:
 -------
  * const Stream &_port - SPI port to communicate over
  * const uint8_t &_SS - SPI buslave select pin used
  * const bool _debug - Whether or not to print error messages
  * const Stream &_debugPort - Serial port to print error messages
 Return:
 -------
  * void
*/
void SPITransfer::begin(const bool _debug)
{
	//初始化spi//_spi->begin();
	//设置spi为主模式//_spi->setSlaveMode(_slave);
	packet.begin(_debug);
	_begun = true;
}

#if 0
/*!
 *    @brief  Manually begin a transaction (calls beginTransaction if hardware
 * SPI)
 */
void SPITransfer::beginTransaction(void) 
{
    //初始化spi//_spi->beginTransaction(*_spiSetting);
}

/*!
 *    @brief  Manually end a transaction (calls endTransaction if hardware SPI)
 */
void SPITransfer::endTransaction(void) 
{
    _spi->endTransaction();
}
#endif


/**
  * @brief  Slave_readACKfrom_Master
  * @param  cBoard: 指向Slave板的cs引脚
  * @note   slave板 读取Master板发送过来的ack信号.
  * @retval 发送成功 返回 1； 发送失败 返回 0
  */
bool SPITransfer::Slave_SyncWith_Master(uint8_t rxData)
{
  	uint8_t rxack;
  	uint32_t msTickstart = HAL_GetTick();
	do
  	{
//		if (!HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {
			//printf("cs2 = 0 \r\n");
    		if (!MSP_SPI_read(_spi, &rxack, 1)) {}//{ printf("spi read err \r\n"); }		//{printf("spi read err ...%d\r\n", rxack);}//{ printf("spi read err \r\n"); return false; }
			//printf("rxack ...%d\r\n", rxack);
			if (rxack == rxData) { 
//				printf("\r\n read rxack ok... %02X\r\n", rxack); 
				return true; 
			}
			//if (HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {
			//	printf("spi2 cs is 1. end read data......%d\r\n", rxack); 
			//	if (rxack == rxData) { printf("\r\n read rxack ok... %02X\r\n", rxack); return true; } 
			//	return false;
			//}
//		}
  	}while ((HAL_GetTick() - msTickstart) < 50);		//{0xAC, 0xA0, 0xB0, 0x30, 0xE1};
	
	printf("timeout ...msTickstart : %ld\r\n", (HAL_GetTick() - msTickstart));
  	return false;
}


/*
 uint8_t SPITransfer::Master_writeCMDto_Slave(const uint16_t &messageLen, const uint8_t packetID)
 Description:
 ------------
  * Send a specified number of bytes in packetized form
 Inputs:
 -------
  * const uint16_t &messageLen - Number of values in txBuff
  to send as the payload in the next packet
  * const uint8_t packetID - The packet 8-bit identifier
 Return:
 -------
  * uint8_t numBytesIncl - Number of payload bytes included in packet
*/
uint8_t SPITransfer::Slave_writeDATAto_Master_withPacket(const uint16_t& messageLen, const uint8_t boardID) 
{
	uint8_t numBytesIncl = packet.constructPacket(messageLen, boardID);

	MSP_SPI_write(_spi, packet.preamble, sizeof(packet.preamble));
	MSP_SPI_write(_spi, packet.txBuff, numBytesIncl);
	MSP_SPI_write(_spi, packet.postamble, sizeof(packet.postamble));
	return numBytesIncl;
}

#if 0
bool SPITransfer::Slave_readACKfrom_Master(uint8_t cs)
{
  	uint8_t rxack, rxi = 0;
  	uint32_t msTickstart = HAL_GetTick();
	//printf(" read rxack  is ... ");
	uint8_t rxbuf[5];
	
  	//while (!HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS))
	do
  	{
		if (!HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {
    		if (!MSP_SPI_read(_spi, _cs, rxbuf, 5)) {printf("spi read err \r\n");}//{ printf("spi read err \r\n"); return false; }
			if (HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {printf("spi2 cs is 1. end read data\r\n"); break;}
		}
  	}while ((HAL_GetTick() - msTickstart) < 50);		//{0xAC, 0xA0, 0xB0, 0x30, 0xE1};
	printf("\r\nmsTickstart : %ld\r\n", (HAL_GetTick() - msTickstart));
	printf(" read is ... ");
	for (int i = 0 ; i < 5 ; i++) {
		printf("%02X, ", rxbuf[i]);
	}
	printf("\r\n read rxack ok... %d\r\n", rxi);
  	return true;
}
#endif

#if 0
/**
  * @brief  Master_writeACKto_Slave
  * @param  cs: 指向Slave板的cs引脚
  * @note   Master板 发送ack信号给Slave板.
  * @retval 发送成功 返回 1； 发送失败 返回 0
  */
bool SPITransfer::Master_writeACKto_Slave(uint8_t cs)
{
	if (cs != -1) _cs = cs;
  	return (bool)MSP_SPI_write(_spi, _cs, &SPI_MASTER_ACK, 1);
}

/**
  * @brief  Master_readACKfrom_Slave
  * @param  cBoard: 指向Slave板的cs引脚
  * @note   Master板 读取Slave板发送过来的ack信号.
  * @retval 发送成功 返回 1； 发送失败 返回 0
  */
bool SPITransfer::Master_readACKfrom_Slave(uint8_t cs)
{
  	uint8_t rxack;
  	uint32_t msTickstart = HAL_GetTick();
	
	if (cs != -1) _cs = cs;
  	do
  	{
    	if (!MSP_SPI_read(_spi, _cs, &rxack, 1)) return false;
		if ((HAL_GetTick() - msTickstart) > sTxRx_TimeOut) {
      		printf("spi rx timeout....\r\n");
      		return false;
    	}
  	} while (rxack != SPI_SLAVE_ACK);
  	return true;
}

/**
  * @brief  Master_writeENDto_Slave
  * @param  cs: 指向Slave板的cs引脚
  * @note   Master板 发送END信号给Slave板.
  * @retval 发送成功 返回 1； 发送失败 返回 0
  */
bool SPITransfer::Master_writeENDto_Slave(uint8_t cs)
{
	if (cs != -1) _cs = cs;
  	return (bool)MSP_SPI_write(_spi, _cs, &SPI_Trans_END, 1);
}

/*
 uint8_t SPITransfer::Master_writeCMDto_Slave(const uint16_t &messageLen, const uint8_t packetID)
 Description:
 ------------
  * Send a specified number of bytes in packetized form
 Inputs:
 -------
  * const uint16_t &messageLen - Number of values in txBuff
  to send as the payload in the next packet
  * const uint8_t packetID - The packet 8-bit identifier
 Return:
 -------
  * uint8_t numBytesIncl - Number of payload bytes included in packet
*/
uint8_t SPITransfer::Master_writeCMDto_Slave_withPacket(const uint16_t& messageLen, const uint8_t boardID) 
{
	uint8_t numBytesIncl = packet.constructPacket(messageLen, boardID);
	if (boardID != -1) _cs = boardID;
	MSP_SPI_write(_spi, _cs, packet.preamble, sizeof(packet.preamble));
	MSP_SPI_write(_spi, _cs, packet.txBuff, numBytesIncl);
	MSP_SPI_write(_spi, _cs, packet.postamble, sizeof(packet.postamble));
	
	return numBytesIncl;
}

void SPITransfer::Master_readDATAfrom_Slave_withPacket(uint8_t cs)
{
  	uint32_t msTickstart = xTaskGetTickCount();			//HAL_GetTick();
  	uint8_t recChar = 0xF0;
	bytesRead = 0;
	if (cs != -1) _cs = cs;
  	uint8_t rxi = 0;    //
	uint8_t spiRxData[100];
	do
  	{
		if (!MSP_SPI_read(_spi, _cs, &recChar, 1)) {
			_chipH->spiTransState = SpiTrans_Err;
			return;
		}
		//if (recChar != 126) continue;
		if (rxi > 100) rxi = 0;
		spiRxData[rxi++] 		= recChar;   //
		//printf("%d, ", recChar);
    	bytesRead                					= packet.parse(recChar);
		status                   					= packet.status;
		if (status != CONTINUE) {
      		printf("\r\nstatus : %d, bytesRead : %d\r\n", status, bytesRead);
      		if (status < 0) {
				reset(); 
				_chipH->spiTransState = SpiTrans_S2M_RxData_Err;
				return;
			}   //{sTransState[cBoard] = SpiTrans_S2M_RxData_Err; reset();}
      		_chipH->spiTransState = SpiTrans_S2M_RxData_End;
			//_chipH->spiRx_uartTx_u8regs_size = rxi;
			return;                   //sTransState[cBoard] = SpiTrans_S2M_RxData_End;
		}
	}while((xTaskGetTickCount() - msTickstart) < sTrans_TimeOut * 20);             //while(recChar != 129); //0xAA);

#if 1   //打印测试信息
  	printf("read spi data len : %d\r\n", rxi);
  	for (int i = 0; i < rxi; i++) {
    	printf("%d, ", spiRxData[i]);
  	}
  	printf("-------------%ld\r\n", xTaskGetTickCount());
  	printf("status : %d\r\n", status);
  	rxi = 0;
#endif
#if 1
  	/* 3------判断是否为超时退出？或者是完成读取数据------------ */
  	//if (sTransState[cBoard] == SpiTrans_S2M_RxData_End) break;
  	if ((HAL_GetTick() - msTickstart) > sTrans_TimeOut) 
  	{
    	printf("spi trans timeout\r\n");
  	}
#endif
	_chipH->spiTransState = SpiTrans_TimeOut;
  	return;
}

/**
  * @brief  Master Synchronization with Slave.
  * @param  currentBoard_TypeDef 当前触发的板子号
  * @retval None                                            如果同步成功，返回true
  */
void SPITransfer::Master_Spi1_Transfer(uint8_t boardID)
{
  	//printf("current board : --------------------------------------------------------------------%d\r\n", boardID);
	if (boardID == -1) { _chipH->spiTransState = SpiTrans_Err; return; }

/*****如果没收到slave板发过来的ack信号， master板就重新发送一次ack信号给slave板，循环3次都失败的话，退出报错*****/  
	for (int i = 0; i < spiTxRx_reRunTimes; i++) {
		/* 1------主控板发送 ACK signal 给 slave板 */
		if (Master_writeACKto_Slave(boardID)) {
			printf("tx ack end\r\n");
			break;
  		} else { 
			printf("tx ack err\r\n");
			if (i >= (spiTxRx_reRunTimes - 1)) { _chipH->spiTransState = SpiTrans_M2S_TxAck_Err; return; }   //{sTransState[cBoard] = SpiTrans_M2S_TxAck_Err; return;}
			else continue;
		}
	}
  	/* 4------从slave板接收数据------------ */
  	//uint16_t recSize = 0;
  	//uint8_t rbuf[50];
	printf("start read data from slave \r\n");
	//osDelay(100);
  	Master_readDATAfrom_Slave_withPacket(boardID);            	//1-----接收数据
  	if (_chipH->spiTransState != SpiTrans_S2M_RxData_End) { printf("read rxdata err\r\n"); return; }
	//recSize = packet.rxObj(rbuf, recSize);                                  				//2-------解析出可用数据
  	printf("rec data : ");
  	//for (int i = 0 ; i < _chipH->spiRx_uartTx_u8regs_size ; i++) {
	for (int i = 0 ; i < (packet.bytesRead) ; i++) {
		_chipH->spiRx_uartTx_u8regs[i] = packet.rxBuff[i];
    	//printf("%c", rbuf[i]);
		//printf("%d", _chipH->spiRx_uartTx_u8regs[i]); packet
		printf("%2X ", packet.rxBuff[i]); 
  	}
	_chipH->spiRx_uartTx_u8regs_size = packet.bytesRead;
  	printf(".......%d\r\n", packet.bytesRead);

	/* 5------接收完数据后，master板发送end信号给slave板------------ */
	if (!Master_writeENDto_Slave(boardID)) { printf("write end to slave err\r\n"); _chipH->spiTransState = SpiTrans_Err; return; }
	_chipH->spiTransState = SpiTrans_End;
	return;
}
#endif

/**
  * @brief  Slave trans data to master with spi2.
  * @param  currentBoard_TypeDef 当前触发的板子号
  * @retval None                                            如果同步成功，返回true
  */
void SPITransfer::Slave_Spi2_Transfer(uint8_t boardID)
{
//	
	uint8_t test_TxBuff[] = {11, 22, 33, 104, 101, 44, 55, 66, 77, 88, 99, 132, 126, 0, 255, 12};  
  	uint16_t sendSize = 0;
  	sendSize = packet.txObj(test_TxBuff, sendSize);  		//1-------封装数据
	//printf("current board : --------------------------------------------------------------------%d\r\n", boardID);
//	if (boardID == -1) { _chipH->spiTransState = SpiTrans_Err; return; }
#if 1
/*****读取master板发过来的ack信号，失败的话，退出报错*****/  
	//printf("start  Slave_Spi2_Transfer\r\n");
	while (HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {} 
	/* 1------主控板发送 ACK signal 给 slave板 */
	if (!Slave_SyncWith_Master(SPI_MASTER_ACK)) {
		printf("read ack from master error.................................\r\n");
		return;
	}
  	/* 2------往master板发送数据------------ */
//	printf("start write data to master \r\n");
  	                   			
  	Slave_writeDATAto_Master_withPacket(sendSize, boardID);            		//2-------发送数据
	while(!HAL_GPIO_ReadPin(SPI2_CS_Port, SPI2_CS)) {} 
//	MSP_SPI_write(_spi, _cs, test_TxBuff, sizeof(test_TxBuff));
//  	printf("write data  end  \r\n");
	/* 3------读取master板发送过来的end信号------------ */
//	if (!Slave_SyncWith_Master(SPI_Trans_END, boardID)) {
//		printf("read end signal from master error!!!!\r\n");
//		return;
//	}
	_chipH->spiTransState = SpiTrans_End;
#endif
	return;
}

/*
 uint8_t SPITransfer::currentPacketID()
 Description:
 ------------
  * Returns the ID of the last parsed packet
 Inputs:
 -------
  * void
 Return:
 -------
  * uint8_t - ID of the last parsed packet
*/
uint8_t SPITransfer::currentPacketID()
{
	return packet.currentPacketID();
}

/*
 void SerialTransfer::reset()
 Description:
 ------------
  * Clears out the tx, and rx buffers, plus resets
  the "bytes read" variable, finite state machine, etc
 Inputs:
 -------
  * void
 Return:
 -------
  * void
*/
void SPITransfer::reset()
{
	uint8_t txdata = 0xFF;
	MSP_SPI_write(_spi, &txdata, 1);	//_spi->transfer(0xFF);
	packet.reset();
	status = packet.status;
	printf("spi transfer reset \r\n");
}

/*
 * 用于c调用
*/
extern void *SPITransfer_C_New(CHIPHandler_t *chipH, SPI_HandleTypeDef *theSPI, uint8_t master)
{
	return new SPITransfer(chipH, theSPI, (bool)master);
}
#if 0
extern void SPITransfer_C_Master_Spi1_Transfer(void *SpiTrans, DChipID_TypeDef chipID)
{
	SPITransfer *sTrans = (SPITransfer *)SpiTrans;
	sTrans->Master_Spi1_Transfer((uint8_t)chipID);
	delete sTrans;
	return;
}
#endif
extern void SPITransfer_C_Slave_Spi2_Transfer(void *SpiTrans, DChipID_TypeDef chipID)
{
	SPITransfer *sTrans = (SPITransfer *)SpiTrans;
	sTrans->Slave_Spi2_Transfer((uint8_t)chipID);
	delete sTrans;
	return;
}


//#endif // not (defined(MBED_H) || defined(__SAM3X8E__))

