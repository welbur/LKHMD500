#ifndef __ISO1I813T_DI_H_
#define __ISO1I813T_DI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "LOG.h"

/*  Register Bit Type Definition
 *_______________________________________________________________________________________________________________________
 *          Type                    |   Symbol   |       Description                                                    |
 *          Read                    |      r     |   The bit can be read                                                |
 * Read only, updated by hardware   |      h     |   The bit is updated by the device itself (for instance: sticky bit) |
 *          Write                   |      w     |   The bit can be written                                             |
 * ----------------------------------------------------------------------------------------------------------------------
*/
#define DIAG_ADDR        0x00               // Collective Diagnostics Register (Wire-Break Detection)                       r/h						
#define INPDATA_ADDR     0x02   			// Input Data Register (Input Channel Data)                                     r/h
#define GLERR_ADDR       0x04           	// Global Error Register                                                        r/h
#define COEFIL0_ADDR     0x06   			// Filter Time for the Data and the Diagnostics of Channel 0                    r/w
#define COEFIL1_ADDR     0x08   			// Filter Time for the Data and the Diagnostics of Channel 1                    r/w
#define COEFIL2_ADDR     0x0A   			// Filter Time for the Data and the Diagnostics of Channel 2                    r/w
#define COEFIL3_ADDR     0x0C   			// Filter Time for the Data and the Diagnostics of Channel 3                    r/w
#define COEFIL4_ADDR     0x0E   			// Filter Time for the Data and the Diagnostics of Channel 4                    r/w
#define COEFIL5_ADDR     0x10   			// Filter Time for the Data and the Diagnostics of Channel 5                    r/w
#define COEFIL6_ADDR     0x12   			// Filter Time for the Data and the Diagnostics of Channel 6                    r/w
#define COEFIL7_ADDR     0x14   			// Filter Time for the Data and the Diagnostics of Channel 7                    r/w
#define INTERR_ADDR      0x16   			// Internal Error Register                                                      r/w
#define GLCFG_ADDR       0x18   			// Global Configuration Register                                                r/w

#define ISO1I813T_READ_REG         0x00
#define ISO1I813T_WRITE_REG        0x80

void ISO1I813T_Write_Reg(uint8_t chipID, uint8_t reg, uint8_t dat);
uint8_t ISO1I813T_Read_Reg(uint8_t chipID, uint8_t reg);


#ifdef __cplusplus
}
#endif

#endif

//__ISO1I813T_DI_H_