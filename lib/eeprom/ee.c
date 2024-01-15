#include "ee.h"
#include "eeConfig.h"
#include <string.h>

#define DATA_32                 ((uint32_t)0x52345678)
#define DATA_64                 ((uint64_t)0x1122334455667788)


#define PAGE 0
#define SECTOR 1
#define PAGE_NUM 2

#if defined(STM32F103xB)
#define _EE_SIZE 1024
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error "Please Enter correct address, maximum is (127)"
#endif
#endif

#if defined(STM32F103x8)
#define _EE_SIZE 1024
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 63)
#error "Please Enter correct address, maximum is (63)"
#endif
#endif

#if defined(STM32F103xC)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error "Please Enter correct address, maximum is (127)"
#endif
#endif

#if defined(STM32F103xD)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 191)
#error "Please Enter correct address, maximum is (191)"
#endif
#endif

#if defined(STM32F103xE)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 255)
#error "Please Enter correct address, maximum is (255)"
#endif
#endif

#if defined(STM32F030x4) || defined(STM32F042x4) || defined(STM32F070x4)
#define _EE_SIZE 1024
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 15)
#error "Please Enter correct address, maximum is (15)"
#endif
#endif

#if defined(STM32F030x6) || defined(STM32F042x6) || defined(STM32F070x6)
#define _EE_SIZE 1024
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 31)
#error "Please Enter correct address, maximum is (31)"
#endif
#endif

#if defined(STM32F030x8) || defined(STM32F042x8)
#define _EE_SIZE 1024
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 63)
#error "Please Enter correct address, maximum is (63)"
#endif
#endif

#if defined(STM32F070xB)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 63)
#error "Please Enter correct address, maximum is (63)"
#endif
#endif

#if defined(STM32F070xC)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error "Please Enter correct address, maximum is (127)"
#endif
#endif

#if defined(STM32F405xx) || defined(STM32F407xx) || defined(STM32F415xx) || defined(STM32F417xx)
#define _EE_SIZE (1024 * 128)
#define _EE_ADDR_INUSE (((uint32_t)0x08020000) | (_EE_SIZE * (_EE_USE_FLASH_PAGE_OR_SECTOR - 5)))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_VOLTAGE_RANGE _EE_VOLTAGE
#define _EE_PAGE_OR_SECTOR SECTOR
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 11)
#error "Please Enter correct address, maximum is (11)"
#endif
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 5)
#error "Please Enter correct address, minimum is (5)"
#endif
#endif

#if defined(STM32F411xC)
#define _EE_SIZE (1024 * 128)
#define _EE_ADDR_INUSE (((uint32_t)0x08020000) | (_EE_SIZE * (_EE_USE_FLASH_PAGE_OR_SECTOR - 5)))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_VOLTAGE_RANGE _EE_VOLTAGE
#define _EE_PAGE_OR_SECTOR SECTOR
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 5)
#error "Please Enter correct address, maximum is (5)"
#endif
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 5)
#error "Please Enter correct address, minimum is (5)"
#endif
#endif

#if defined(STM32F411xE)
#define _EE_SIZE (1024 * 128)
#define _EE_ADDR_INUSE (((uint32_t)0x08020000) | (_EE_SIZE * (_EE_USE_FLASH_PAGE_OR_SECTOR - 5)))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_VOLTAGE_RANGE _EE_VOLTAGE
#define _EE_PAGE_OR_SECTOR SECTOR
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 7)
#error "Please Enter correct address, maximum is (7)"
#endif
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 5)
#error "Please Enter correct address, minimum is (5)"
#endif
#endif

#if defined(STM32G030xx) || defined(STM32G050xx)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 31)
#error "Please Enter correct address, maximum is (31)"
#endif
#endif

#if defined(STM32G070xx)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 63)
#error "Please Enter correct address, maximum is (63)"
#endif
#endif

#if defined(STM32G4xx)
#define _EE_SIZE 			2048 // FLASH_PAGE_SIZE
#define _EE_ADDR_INUSE 		(((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK 		FLASH_BANK_1	//FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR 	PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127) // FLASH_PAGE_NB
#error "Please Enter correct address, maximum is (127)"
#endif
#endif

#if defined(STM32L433xx)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_FLASH_BANK FLASH_BANK_1
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error "Please Enter correct address, maximum is (127)"
#endif
#endif

#if defined(STM32L476xx)
#define _EE_SIZE 2048
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 256)
#define _EE_FLASH_BANK FLASH_BANK_1
#else
#define _EE_FLASH_BANK FLASH_BANK_2
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 511)
#error "Please Enter correct address, maximum is (511)"
#endif
#endif
#endif

// 2MB version
#if defined(STM32U575xx) || defined(STM32U585xx)
#define _EE_SIZE 8192
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_ICACHE_CTRL 1
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 128)
#define _EE_FLASH_BANK FLASH_BANK_1
#else
#define _EE_FLASH_BANK FLASH_BANK_2
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 255)
#error "Please Enter correct address, maximum is (255)"
#endif
#endif
#endif

// 4MB version
#if defined(STM32U599xx)
#define _EE_SIZE 8192
#define _EE_ADDR_INUSE (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define _EE_ICACHE_CTRL 1
#if (_EE_USE_FLASH_PAGE_OR_SECTOR < 256)
#define _EE_FLASH_BANK FLASH_BANK_1
#else
#define _EE_FLASH_BANK FLASH_BANK_2
#define _EE_PAGE_OR_SECTOR PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 511)
#error "Please Enter correct address, maximum is (511)"
#endif
#endif
#endif

#if (_EE_USE_RAM_BYTE > 0)
uint8_t ee_ram[_EE_USE_RAM_BYTE];
#endif

// ##########################################################################################################
bool ee_init(void)
{
#if (_EE_USE_RAM_BYTE > 0)
	return ee_read(0, _EE_USE_RAM_BYTE, NULL);
#else
	return true;
#endif
}
// ##########################################################################################################
bool ee_format(bool keepRamData)
{
	uint32_t error;
	HAL_FLASH_Unlock();

	/* Clear OPTVERR bit set on virgin samples */
  	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

#if _EE_ICACHE_CTRL == 1
	HAL_ICACHE_Disable();
#endif
	FLASH_EraseInitTypeDef flashErase;
#if _EE_PAGE_OR_SECTOR == PAGE
	flashErase.NbPages = 1;
	flashErase.PageAddress = _EE_ADDR_INUSE;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
#elif _EE_PAGE_OR_SECTOR == SECTOR
	flashErase.NbSectors = 1;
	flashErase.Sector = _EE_USE_FLASH_PAGE_OR_SECTOR;
	flashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
#elif _EE_PAGE_OR_SECTOR == PAGE_NUM
	flashErase.NbPages = 1;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	flashErase.Page = _EE_USE_FLASH_PAGE_OR_SECTOR;// - 64;
#endif
#ifdef _EE_FLASH_BANK
	flashErase.Banks = _EE_FLASH_BANK;
#endif
#ifdef _EE_VOLTAGE_RANGE
	flashErase.VoltageRange = _EE_VOLTAGE_RANGE;
#endif

	LOG("FLASH_USER_START_ADDR : %lx\r\n", _EE_ADDR_INUSE);
	LOG("FirstPage : %ld\r\n", flashErase.Page);
	LOG("NbOfPages : %ld\r\n", flashErase.NbPages);
	LOG("BankNumber : %ld\r\n", flashErase.Banks);

	if (HAL_FLASHEx_Erase(&flashErase, &error) == HAL_OK)
	{
		HAL_FLASH_Lock();
		if (error != 0xFFFFFFFF)
		{
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			//LOG("eeprom format faild\r\n");
			return false;
		}
		else
		{
#if (_EE_USE_RAM_BYTE > 0)
			if (keepRamData == false)
				memset(ee_ram, 0xFF, _EE_USE_RAM_BYTE);
#endif
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			return true;
		}
	}

	HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
	HAL_ICACHE_Enable();
#endif
	LOG("eeprom format faild\r\n");
	return false;
}
// ##########################################################################################################
bool ee_read(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{
	if ((startVirtualAddress + len) > _EE_SIZE)
		return false;
	for (uint32_t i = startVirtualAddress; i < len + startVirtualAddress; i++)
	{
//		LOG("read address : %lx", (i + _EE_ADDR_INUSE));
		if (data != NULL)
		{
			*data = (*(__IO uint8_t *)(i + _EE_ADDR_INUSE));
			data++;
//			LOG("---- %lx\r\n", *data);
		}
#if (_EE_USE_RAM_BYTE > 0)
		if (i < _EE_USE_RAM_BYTE)
			ee_ram[i] = (*(__IO uint8_t *)(i + _EE_ADDR_INUSE));
#endif
	}
	return true;
}
// ##########################################################################################################
bool ee_write(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{
	
	if ((startVirtualAddress + len) > _EE_SIZE)
		return false;
	if (data == NULL)
		return false;
	HAL_FLASH_Unlock();
//	LOG("write address : %lx\r\n", ((startVirtualAddress)) + _EE_ADDR_INUSE);
#if _EE_ICACHE_CTRL == 1
	HAL_ICACHE_Disable();
#endif
#ifdef FLASH_TYPEPROGRAM_BYTE
	for (uint32_t i = 0; i < len; i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint64_t)(data[i])) != HAL_OK)
		{
			HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			return false;
		}
	}
#endif
#ifdef FLASH_TYPEPROGRAM_HALFWORD
	for (uint32_t i = 0; i < len; i += 2)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint64_t)(data[i] | (data[i + 1] << 8))) != HAL_OK)
		{
			HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			return false;
		}
	}
#endif
#ifdef FLASH_TYPEPROGRAM_DOUBLEWORD
//	LOG("double word : %llu \r\n", DATA_32);
	for (uint32_t i = 0; i < len; i += 8)
	{
		//uint8_t DoubleWord[8] ={
		//	data[i + 0], data[i + 1], data[i + 2], data[i + 3], data[i + 4],
		//	data[i + 5], data[i + 6], data[i + 7]};

		uint64_t DoubleWord = ee_byteArraytoDoubleWord(data + i);
		
		//LOG("address : %lx\r\n", ((i + startVirtualAddress)) + _EE_ADDR_INUSE);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, DoubleWord) != HAL_OK)
		{
			LOG("failt..............\r\n");
			HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			return false;
		}
	}
#endif
#ifdef FLASH_TYPEPROGRAM_QUADWORD
	for (uint32_t i = 0; i < len; i += 16)
	{
		uint8_t QuadWord[16] =
			{
				data[i + 0], data[i + 1], data[i + 2], data[i + 3], data[i + 4],
				data[i + 5], data[i + 6], data[i + 7], data[i + 8], data[i + 9],
				data[i + 10], data[i + 11], data[i + 12], data[i + 13], data[i + 14],
				data[i + 15]};
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint32_t)QuadWord) != HAL_OK)
		{
			HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
			HAL_ICACHE_Enable();
#endif
			return false;
		}
	}
#endif
	HAL_FLASH_Lock();
#if _EE_ICACHE_CTRL == 1
	HAL_ICACHE_Enable();
#endif
	return true;
}
// ##########################################################################################################
bool ee_writeToRam(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{
#if (_EE_USE_RAM_BYTE > 0)
	if ((startVirtualAddress + len) > _EE_USE_RAM_BYTE)
		return false;
	if (data == NULL)
		return false;
	memcpy(&ee_ram[startVirtualAddress], data, len);
	return true;
#else
	return false;
#endif
}
// ##########################################################################################################
bool ee_commit(void)
{
#if (_EE_USE_RAM_BYTE > 0)
	if (ee_format(true) == false)
	{
		return false;
		LOG("ee commit error!\r\n");
	}
	return ee_write(0, _EE_USE_RAM_BYTE, ee_ram);
#else
	return false;
#endif
}
// ##########################################################################################################
uint32_t ee_maxVirtualAddress(void)
{
	return (_EE_SIZE);
}

// ##########################################################################################################
bool ee_ClearFLASHPage(void)
{
	uint8_t ee_clear_ram[_EE_USE_RAM_BYTE];
	memset(ee_clear_ram, 0, _EE_USE_RAM_BYTE);
	return ee_SavetoFLASH(0, _EE_USE_RAM_BYTE, ee_clear_ram);
}

// ##########################################################################################################
bool ee_SavetoFLASH(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{
	/* 写数据到FLASH */
	ee_writeToRam(startVirtualAddress, len, data);
    if (ee_commit() == false)
	{
        LOG_error("EEPROM Commit faild\r\n");
		return false;
	}
//	LOG("write data : 0x%X, 0x%X%X ", data[0], data[1], data[2]);
//	LOG(" 0x%X%X \r\n", data[3], data[4]);
	uint8_t ereaddata[len];
	/* 读出刚刚写入的数据 */
	if (ee_read(startVirtualAddress, len, ereaddata) == false)
	{
        LOG_error("read eeprom data faild!\r\n");
		return false;
	}
	LOG("ee read data : 0x%X%X ", ereaddata[0], ereaddata[1]);
	LOG(" 0x%X%X ", ereaddata[2], ereaddata[3]);
	LOG(" 0x%X%X ", ereaddata[4], ereaddata[5]);
	LOG(" 0x%X%X\r\n", ereaddata[6], ereaddata[7]);
	/*校验写入的数据是否正确*/
	if (memcmp(data, ereaddata, len) != 0)
	{
		LOG_error("eeprom data write error!\r\n");
		return false;
	}

	return true;
}

// ##########################################################################################################
#if 0
uint64_t ee_byteArraytoDoubleWord(uint8_t *data)
{
	uint16_t halfword1 = word(data[1], data[0]);
	uint16_t halfword2 = word(data[3], data[2]);
	uint16_t halfword3 = word(data[5], data[4]);
	uint16_t halfword4 = word(data[7], data[6]);
//	LOG("half word : %x, %x, ", halfword1, halfword2);
//	LOG("%x, %x\r\n", halfword3, halfword4);
	uint32_t doubword1 = Dword(halfword2, halfword1);
	uint32_t doubword2 = Dword(halfword4, halfword3);
//	LOG("doub word : %x, %x\r\n", doubword1, doubword2);
	uint64_t ddword = DFword(doubword2, doubword1);		//((uint64_t)(doubword2) << 32) | doubword1;
//	LOG("ddoub word : %llX\r\n", ddword);
	return ddword;
}
#endif

uint64_t ee_byteArraytoDoubleWord(uint8_t *data)
{
  uint64_t result = 0;
  for (int i = 0; i < 8; i++) {
    result |= (uint64_t)data[i] << (8 * i);
  }
  return result;
}

void ee_testWriteandRead(void)
{
	float DCM_AdjustOUTCurr_Value[5] = {1.0, 1.1, 1.2, 1.3, 1.4};
	uint8_t adjdata[30];
    uint8_t adj_whole_curr = 0xF0;
    memcpy(adjdata, &adj_whole_curr, 1);        //先保存标志
	LOG("adjdata : ");
    for (int i = 0; i < DCModuleNum; i++)
    {
        memcpy(adjdata + 1 + i * 4, &(DCM_AdjustOUTCurr_Value[i]), 4);
        LOG("%X%X%X%X,  ", adjdata[1 + i*4], adjdata[1 + i*4 + 1], adjdata[1 + i*4 + 2], adjdata[1 + i*4 + 3]);
    }
    LOG("\r\n");
	if (ee_SavetoFLASH(_EE_USE_AdjustCurr_START_ADDR, DCModuleNum * 4 + 1, adjdata) == true)        //保存参数到flash
    {
        LOG_info("Save Adjust data to FLASH Success! \r\n");
    }
    else
        LOG_error("save adjust-from-plc error\r\n");
}

#if 0
void ee_testWriteandRead(void)
{
	if (ee_init() == false)
		LOG_error("EEPROM Init faild!"); 

	uint8_t eett1[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA};
	//uint8_t eett1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
	uint8_t eereaddata[40];
//	HAL_Delay(5000);
	ee_writeToRam(0, sizeof(eett1), eett1);
    if (ee_commit() == false)
        LOG_error("EEPROM Commit faild\r\n");

	if (ee_read(0, sizeof(eereaddata), eereaddata) == false)
        LOG_error("read eeprom data faild!\r\n");
	
    LOG("read data : ");
    for (uint16_t i = 0; i < 40; i++)
    {
        LOG(" %x", eereaddata[i]);
    }
    LOG("\r\n");
}
#endif