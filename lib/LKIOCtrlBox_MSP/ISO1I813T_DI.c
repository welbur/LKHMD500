/*  隔离数字输入芯片ISO1I813T的驱动
 *
*/
#include "ISO1I813T_DI.h"
#include "MSP_SPI.h"


/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void ISO1I813T_Write_Reg(uint8_t chipID, uint8_t reg, uint8_t dat)
{
    uint8_t wreg = reg | ISO1I813T_WRITE_REG;
	SPI1_CS_LOW(chipID);
    MSP_SPI_write(&hspi1, &wreg, 1);        //SPI1_WK2412S1_Read_Write(reg);
    MSP_SPI_write(&hspi1, &dat, 1);                      //SPI1_WK2412S1_Read_Write(dat);
    SPI1_CS_HIGH(chipID);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
uint8_t ISO1I813T_Read_Reg(uint8_t chipID, uint8_t reg)
{
	uint8_t rec_data = 0; 
    uint8_t txbuf[2] = {(reg | ISO1I813T_READ_REG), 0x00};
	SPI1_CS_LOW(chipID);
	//if (HAL_SPI_Transmit_IT(&hspi1, txbuf, 1) != HAL_OK) {LOGE("tran data error\r\n");}
	//	return 0;
    MSP_SPI_write(&hspi1, txbuf, 2);                //SPI1_WK2412S1_Read_Write(0x40 + reg);

	MSP_SPI_read(&hspi1, &rec_data, 1);             //rec_data = SPI1_WK2412S1_Read_Write(Dummy_Byte);
	SPI1_CS_HIGH(chipID);
	return rec_data;
}

#if 0
/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void EXHW_WK2412S1_Write_FIFO(uint8_t reg, uint8_t *buf, uint16_t len)
{
	uint16_t cnt = 0;
	
	SPI1_WK2412S1_CS_LOW();
	SPI1_WK2412S1_Read_Write(0x80 + reg);
	for(cnt = 0;cnt < len; cnt++){
		SPI1_WK2412S1_Read_Write(*(buf + cnt));
	} 
	SPI1_WK2412S1_CS_HIGH();
}

/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void EXHW_WK2412S1_Read_FIFO(uint8_t reg, uint8_t *buf, uint16_t len)
{
	uint16_t cnt = 0;
	
	SPI1_WK2412S1_CS_LOW();
	SPI1_WK2412S1_Read_Write(0xC0 + reg);
	for(cnt = 0;cnt < len; cnt++){
		*(buf + cnt) = SPI1_WK2412S1_Read_Write(Dummy_Byte);
	} 
	SPI1_WK2412S1_CS_HIGH();
}


/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void EXHW_WK2412S_Init(void)
{
	/*ʹ���Ӵ���1,2,3,4��ʱ��*/
	EXHW_WK2412S1_Write_Reg(GENA,0x0F);
	
	/*��λ�Ӵ���1,2,3,4*/
	EXHW_WK2412S1_Write_Reg(GRST,0x0F);
	
	/*ʹ���Ӵ���1,2,3,4��ȫ���ж� */
	EXHW_WK2412S1_Write_Reg(GIER,0x0F);
	
}

/*******************************************************************************
* Function Name  : 
* Description    : 
*  
*
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
uint8_t Wk2xxxTest(void)
{
	uint8_t rec_data = 0,rv = 0;
	//���ӿ�ΪSPI	
	rec_data = EXHW_WK2412S1_Read_Reg(GENA);
	if(rec_data == 0x30){
		rv = 0;
	}
	else{
		rv = 1;
	}
	return rv;
}
#endif
