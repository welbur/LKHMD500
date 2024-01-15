#ifndef __EECONFIG_H
#define __EECONFIG_H

#define _EE_USE_SetCurrGain_START_ADDR              2
#define _EE_USE_OUTCurrGain_START_ADDR              250
#define _EE_USE_AdjustCurr_START_ADDR               500         //保存PLC发送过来的调整输出电流的数值
#define   _EE_USE_FLASH_PAGE_OR_SECTOR              (127)   //127
#define   _EE_USE_RAM_BYTE                          (1024)
#define   _EE_VOLTAGE                               VOLTAGE_RANGE_3       //FLASH_VOLTAGE_RANGE_3 //  use in some devices
#endif