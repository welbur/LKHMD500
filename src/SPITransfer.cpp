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
SPITransfer::SPITransfer(SlaveBoardHandler_t *slavebH, SPI_HandleTypeDef *theSPI, bool master) 
{
	_slavebH = slavebH;
  	_spi = theSPI;
	_master = master;
	_cs = _slavebH->BoardID;
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

/*!
 *    @brief  Write a buffer or two to the SPI device, with transaction
 * management.
 *    @param  buffer Pointer to buffer of data to write
 *    @param  len Number of bytes from buffer to write
 *    @param  prefix_buffer Pointer to optional array of data to write before
 * buffer.
 *    @param  prefix_len Number of bytes from prefix buffer to write
 *    @return Always returns true because there's no way to test success of SPI
 * writes
 */
bool SPITransfer::write(uint8_t *buffer, size_t len,
                        //const uint8_t *prefix_buffer, size_t prefix_len,
						uint8_t cs) 
{
	//uint8_t * prebuffer = (uint8_t *)prefix_buffer;
	if (cs != -1) _cs = cs;
	//if (!MSP_SPI_write(_spi, _cs, prebuffer, prefix_len)) return false;
	if (!MSP_SPI_write(_spi, _cs, buffer, len)) return false;

	return true;
}

/*!
 *    @brief  Read from SPI into a buffer from the SPI device, with transaction
 * management.
 *    @param  buffer Pointer to buffer of data to read into
 *    @param  len Number of bytes from buffer to read.
 *    @return Always returns true because there's no way to test success of SPI
 * writes
 */
bool SPITransfer::read(uint8_t *buffer, size_t len, uint8_t cs) {
	if (cs != -1) _cs = cs;
	if (!MSP_SPI_read(_spi, _cs, buffer, len)) return false;
	
  	return true;
}

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
			_slavebH->spiTransState = SpiTrans_Err;
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
				_slavebH->spiTransState = SpiTrans_S2M_RxData_Err;
				return;
			}   //{sTransState[cBoard] = SpiTrans_S2M_RxData_Err; reset();}
      		_slavebH->spiTransState = SpiTrans_S2M_RxData_End;
			//_slavebH->spiRx_uartTx_u8regs_size = rxi;
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
	_slavebH->spiTransState = SpiTrans_TimeOut;
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
	if (boardID == -1) { _slavebH->spiTransState = SpiTrans_Err; return; }

/*****如果没收到slave板发过来的ack信号， master板就重新发送一次ack信号给slave板，循环3次都失败的话，退出报错*****/  
	for (int i = 0; i < spiTxRx_reRunTimes; i++) {
		/* 1------主控板发送 ACK signal 给 slave板 */
		if (Master_writeACKto_Slave(boardID)) {
			printf("tx ack end\r\n");
			break;
  		} else { 
			printf("tx ack err\r\n");
			if (i >= (spiTxRx_reRunTimes - 1)) { _slavebH->spiTransState = SpiTrans_M2S_TxAck_Err; return; }   //{sTransState[cBoard] = SpiTrans_M2S_TxAck_Err; return;}
			else continue;
		}
	}
#if 0
  		/* 2------从slave板接收ACK信号------------ */
  		if (Master_readACKfrom_Slave(boardID)) {
			printf("rx ack end\r\n");
			break;
  		} else {
			printf("rx ack err\r\n");
			if (i >= (spiTxRx_reRunTimes - 1)) return SpiTrans_S2M_RxAck_Err; 
			else continue;
	  	}
	}
/***************************************************************************************************/

  	/* 3------Master板往slave板发送指令------------ */
  	uint8_t txbuf[4] = {0x01, 0x02, 0x03, 0xAA};
  	uint16_t sendSize = 0;
  	sendSize = packet.txObj(txbuf, sendSize);                     			//1-------封装数据
  	Master_writeCMDto_Slave_withPacket(sendSize, boardID);            		//2-------发送数据
#endif
  	/* 4------从slave板接收数据------------ */
  	//uint16_t recSize = 0;
  	//uint8_t rbuf[50];
	printf("start read data from slave \r\n");
	//osDelay(100);
  	Master_readDATAfrom_Slave_withPacket(boardID);            	//1-----接收数据
  	if (_slavebH->spiTransState != SpiTrans_S2M_RxData_End) { printf("read rxdata err\r\n"); return; }
	//recSize = packet.rxObj(rbuf, recSize);                                  				//2-------解析出可用数据
  	printf("rec data : ");
  	//for (int i = 0 ; i < _slavebH->spiRx_uartTx_u8regs_size ; i++) {
	for (int i = 0 ; i < (packet.bytesRead) ; i++) {
		_slavebH->spiRx_uartTx_u8regs[i] = packet.rxBuff[i];
    	//printf("%c", rbuf[i]);
		//printf("%d", _slavebH->spiRx_uartTx_u8regs[i]); packet
		printf("%2X ", packet.rxBuff[i]); 
  	}
	_slavebH->spiRx_uartTx_u8regs_size = packet.bytesRead;
  	printf(".......%d\r\n", packet.bytesRead);

	/* 5------接收完数据后，master板发送end信号给slave板------------ */
	if (!Master_writeENDto_Slave(boardID)) { printf("write end to slave err\r\n"); _slavebH->spiTransState = SpiTrans_Err; return; }
	_slavebH->spiTransState = SpiTrans_End;
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
	MSP_SPI_write(_spi, _cs, &txdata, 1);	//_spi->transfer(0xFF);
	packet.reset();
	status = packet.status;
	printf("spi transfer reset \r\n");
}

/*
 * 用于c调用
*/
extern void *SPITransfer_C_New(SlaveBoardHandler_t *slavebH, SPI_HandleTypeDef *theSPI, uint8_t master)
{
	return new SPITransfer(slavebH, theSPI, (bool)master);
}
extern void SPITransfer_C_Master_Spi1_Transfer(void *SpiTrans, BoardID_TypeDef boardID)
{
	SPITransfer *sTrans = (SPITransfer *)SpiTrans;
	sTrans->Master_Spi1_Transfer((uint8_t)boardID);
	delete sTrans;
	return;
}


//#endif // not (defined(MBED_H) || defined(__SAM3X8E__))

