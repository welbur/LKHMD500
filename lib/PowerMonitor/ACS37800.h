/*
 * acs37800.h
 *
 * Created: 2023-05-21
 *  Author: WU555
 */

#ifndef ACS37800_H_
#define ACS37800_H_

#include "main.h"
#include <inttypes.h>
#include <math.h> // for fabs()
// #include "MSP_SPI.h"

// The default I2C Address is 0x60 when DIO_0 and DIO_1 are 0V on start-up
// (There is a typo in the datasheet that suggests it is 0x61. It isn't...!)
// The address can be configured in EEPROM too using setI2Caddress
#define ACS37800_DEFAULT_I2C_ADDRESS 0x60

// Customer Access Code - stored in volatile register 0x2F
#define ACS37800_CUSTOMER_ACCESS_CODE 0x4F70656E

// Default sense resistance for voltage measurement (Ohms)
#define ACS37800_DEFAULT_SENSE_RES 8200

// Default voltage-divider resistance for voltage measurement (Ohms)
#define ACS37800_DEFAULT_DIVIDER_RES 2000000

// Default current-sensing range
// ACS37800KMACTR-030B3-I2C is a 30.0 Amp part - as used on the SparkFun Qwiic Power Meter
// ACS37800KMACTR-090B3-I2C is a 90.0 Amp part
#define ACS37800_DEFAULT_CURRENT_RANGE 30

// EEPROM Registers
#define ACS37800_REGISTER_EEPROM_0B 0x0B
#define ACS37800_REGISTER_EEPROM_0C 0x0C
#define ACS37800_REGISTER_EEPROM_0D 0x0D
#define ACS37800_REGISTER_EEPROM_0E 0x0E
#define ACS37800_REGISTER_EEPROM_0F 0x0F

// Shadow Registers
// At power up, all shadow registers are loaded from EEPROM, including all configuration parameters.
// The shadow registers can be written to in order to change the device behavior without having to
// perform an EEPROM write. Any changes made in shadow memory are volatile and do not persist through a reset event.
#define ACS37800_REGISTER_SHADOW_1B 0x1B
#define ACS37800_REGISTER_SHADOW_1C 0x1C
#define ACS37800_REGISTER_SHADOW_1D 0x1D
#define ACS37800_REGISTER_SHADOW_1E 0x1E
#define ACS37800_REGISTER_SHADOW_1F 0x1F

// Volatile Registers
#define ACS37800_REGISTER_VOLATILE_20 0x20
#define ACS37800_REGISTER_VOLATILE_21 0x21
#define ACS37800_REGISTER_VOLATILE_22 0x22
#define ACS37800_REGISTER_VOLATILE_25 0x25
#define ACS37800_REGISTER_VOLATILE_26 0x26
#define ACS37800_REGISTER_VOLATILE_27 0x27
#define ACS37800_REGISTER_VOLATILE_28 0x28
#define ACS37800_REGISTER_VOLATILE_29 0x29
#define ACS37800_REGISTER_VOLATILE_2A 0x2A
#define ACS37800_REGISTER_VOLATILE_2C 0x2C
#define ACS37800_REGISTER_VOLATILE_2D 0x2D
#define ACS37800_REGISTER_VOLATILE_2F 0x2F
#define ACS37800_REGISTER_VOLATILE_30 0x30

#define ACS37800_IC_Num 8

// Error result
typedef enum
{
	ACS37800_SUCCESS = 0,
	ACS37800_ERR_SPI_ERROR,
	ACS37800_ERR_REGISTER_READ_MODIFY_WRITE_FAILURE
} ACS37800ERR;

/* EEPROM Registers : Bit Field definitions */
typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t qvo_fine : 9;
			uint32_t sns_fine : 10;
			uint32_t crs_sns : 3;
			uint32_t iavgselen : 1;
			uint32_t pavgselen : 1;
			uint32_t reserved : 2;
			uint32_t ECC : 6;
		} bits;
	} data;
} ACS37800_REGISTER_0B_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t rms_avg_1 : 7;
			uint32_t rms_avg_2 : 10;
			uint32_t vchan_offset_code : 8;
			uint32_t reserved : 1;
			uint32_t ECC : 6;
		} bits;
	} data;
} ACS37800_REGISTER_0C_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t reserved1 : 7;
			uint32_t ichan_del_en : 1;
			uint32_t reserved2 : 1;
			uint32_t chan_del_sel : 3;
			uint32_t reserved3 : 1;
			uint32_t fault : 8;
			uint32_t fltdly : 3;
			uint32_t reserved4 : 2;
			uint32_t ECC : 6;
		} bits;
	} data;
} ACS37800_REGISTER_0D_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vevent_cycs : 6;
			uint32_t reserved1 : 2;
			uint32_t overvreg : 6;
			uint32_t undervreg : 6;
			uint32_t delaycnt_sel : 1;
			uint32_t halfcycle_en : 1;
			uint32_t squarewave_en : 1;
			uint32_t zerocrosschansel : 1;
			uint32_t zerocrossedgesel : 1;
			uint32_t reserved2 : 1;
			uint32_t ECC : 6;
		} bits;
	} data;
} ACS37800_REGISTER_0E_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t reserved1 : 2;
			uint32_t i2c_slv_addr : 7;
			uint32_t i2c_dis_slv_addr : 1;
			uint32_t dio_0_sel : 2;
			uint32_t dio_1_sel : 2;
			uint32_t n : 10;
			uint32_t bypass_n_en : 1;
			uint32_t reserved2 : 1;
			uint32_t ECC : 6;
		} bits;
	} data;
} ACS37800_REGISTER_0F_t;
/* EEPROM Registers : Bit Field definitions end */

/* Volatile Registers : Bit Field definitions */

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vrms : 16;
			uint32_t irms : 16;
		} bits;
	} data;
} ACS37800_REGISTER_20_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t pactive : 16;
			uint32_t pimag : 16;
		} bits;
	} data;
} ACS37800_REGISTER_21_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t papparent : 16;
			uint32_t pfactor : 11;
			uint32_t posangle : 1;
			uint32_t pospf : 1;
		} bits;
	} data;
} ACS37800_REGISTER_22_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t numptsout : 10;
		} bits;
	} data;
} ACS37800_REGISTER_25_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vrmsavgonesec : 16;
			uint32_t irmsavgonesec : 16;
		} bits;
	} data;
} ACS37800_REGISTER_26_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vrmsavgonemin : 16;
			uint32_t irmsavgonemin : 16;
		} bits;
	} data;
} ACS37800_REGISTER_27_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t pactavgonesec : 16;
		} bits;
	} data;
} ACS37800_REGISTER_28_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t pactavgonemin : 16;
		} bits;
	} data;
} ACS37800_REGISTER_29_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vcodes : 16;
			uint32_t icodes : 16;
		} bits;
	} data;
} ACS37800_REGISTER_2A_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t pinstant : 16;
		} bits;
	} data;
} ACS37800_REGISTER_2C_t;

typedef struct
{
	union
	{
		uint32_t all;
		struct
		{
			uint32_t vzerocrossout : 1;
			uint32_t faultout : 1;
			uint32_t faultlatched : 1;
			uint32_t overvoltage : 1;
			uint32_t undervoltage : 1;
		} bits;
	} data;
} ACS37800_REGISTER_2D_t;
/* Volatile Registers : Bit Field definitions end */

// Register Field Enums
typedef enum
{
	ACS37800_CRS_SNS_1X = 0,
	ACS37800_CRS_SNS_2X,
	ACS37800_CRS_SNS_3X,
	ACS37800_CRS_SNS_3POINT5X,
	ACS37800_CRS_SNS_4X,
	ACS37800_CRS_SNS_4POINT5X, // This appears to be the default
	ACS37800_CRS_SNS_5POINT5X,
	ACS37800_CRS_SNS_8X
} ACS37800_CRS_SNS_e; // Coarse gain for the current channel

// const float ACS37800_CRS_SNS_GAINS[8] = { 1.0, 2.0, 3.0, 3.5, 4.0, 4.5, 5.5, 8.0 };

typedef enum
{
	ACS37800_FLTDLY_0000 = 0,
	ACS37800_FLTDLY_0475 = 2, // 4.75 microseconds
	ACS37800_FLTDLY_0925,	  // 9.25 microseconds
	ACS37800_FLTDLY_1375,
	ACS37800_FLTDLY_1850,
	ACS37800_FLTDLY_2325,
	ACS37800_FLTDLY_2775 // 27.75 microseconds
} ACS37800_FLTDLY_e;	 // Fault Delay

typedef enum
{
	ACS37800_DIO0_FUNC_ZERO_CROSSING = 0,
	ACS37800_DIO0_FUNC_OVERVOLTAGE,
	ACS37800_DIO0_FUNC_UNDERVOLTAGE,
	ACS37800_DIO0_FUNC_OV_OR_UV
} ACS37800_DIO0_FUNC_e; // DIO_0 Function

typedef enum
{
	ACS37800_DIO1_FUNC_OVERCURRENT = 0,
	ACS37800_DIO1_FUNC_UNDERVOLTAGE,
	ACS37800_DIO1_FUNC_OVERVOLTAGE,
	ACS37800_DIO1_FUNC_OV_OR_UV_OR_OCF_LAT
} ACS37800_DIO1_FUNC_e; // DIO_1 Function

typedef enum
{
	ACS37800_EEPROM_ECC_NO_ERROR = 0,
	ACS37800_EEPROM_ECC_ERROR_CORRECTED,
	ACS37800_EEPROM_ECC_ERROR_UNCORRECTABLE,
	ACS37800_EEPROM_ECC_NO_MEANING
} ACS37800_EEPROM_ECC_e; // EEPROM ECC Errors

typedef struct
{
	// ACS37800ERR         error;
	uint8_t printDebug;
	uint8_t ACS37800Address;
	// uint16_t maxVoltage;				// Needed to calculate V & P correctly
	// uint8_t maxCurrent;					// Needed to calculate I & P correctly
	SPI_HandleTypeDef *spiPort; // void                *ioInterface;					// Pointer to the IO/Peripheral Interface library

	float SenseResistance;	   // The value of the sense resistor for voltage measurement in Ohms
	float DividerResistance;   // The value of the divider resistance for voltage measurement in Ohms
	float CurrentSensingRange; // The ACS37800's current sensing range
	float CurrentCoarseGain;   // The ACS37800's coarse current gain - needed by the current calculations

} ACS37800_t;

typedef struct
{
	float Vrms;		  // 读取RMS电压
	float Irms;		  // 读取RMS电流
	float Pactive;	  // 有功功率
	float Preactive;  // 无功功率
	float Papparent;  // 视在功率
	float Pfactor;	  // 功率因数
	uint8_t PosAngle; // 角差      值：0和1
	uint8_t PosPF;	  // 功率角差   值：0和1
	// uint8_t             ZeroCross;          //过零检测   值：0和1
	uint8_t ACSstatus; // acs37800芯片的状态
} PowerMonitorData_t;
#define PMIntDataNum 3
#define PMFloatDataNum 6



PowerMonitorData_t PowerMonitorData[ACS37800_IC_Num];

ACS37800ERR ACS37800_Init(ACS37800_t *acs37800);

// Basic methods for accessing registers
ACS37800ERR ACS37800_readRegister(uint32_t *data, uint8_t address);
ACS37800ERR ACS37800_writeRegister(uint32_t data, uint8_t address);

// Configurable Settings
// By default, settings are written to the shadow registers only. Set _eeprom to true to write to EEPROM too.
// Set/Get the number of samples for RMS calculations. Bypass_N_Enable must be set/true for this to have effect.
ACS37800ERR ACS37800_setNumberOfSamples(uint32_t numberOfSamples, uint8_t eeprom);
ACS37800ERR ACS37800_getNumberOfSamples(uint32_t *numberOfSamples); // Read and return the number of samples (from _shadow_ memory)
// Set/Clear the Bypass_N_Enable flag
ACS37800ERR ACS37800_setBypassNenable(uint8_t bypass, uint8_t eeprom);
ACS37800ERR ACS37800_getBypassNenable(uint8_t *bypass); // Read and return the bypass_n_en flag (from _shadow_ memory)
// Read and return the gain (from _shadow_ memory)
ACS37800ERR ACS37800_getCurrentCoarseGain(float *currentCoarseGain);

// Basic methods for accessing the volatile registers
ACS37800ERR ACS37800_readRMS(float *vRMS, float *iRMS);													   // Read volatile register 0x20. Return the vRMS and iRMS.
ACS37800ERR ACS37800_readPowerActiveReactive(float *pActive, float *pReactive);							   // Read volatile register 0x21. Return the pactive and pimag (reactive)
ACS37800ERR ACS37800_readPowerFactor(float *pApparent, float *pFactor, uint8_t *posangle, uint8_t *pospf); // Read volatile register 0x22. Return the apparent power, power factor, leading / lagging, generated / consumed
ACS37800ERR ACS37800_readInstantaneous(float *vInst, float *iInst, float *pInst);						   // Read volatile registers 0x2A and 0x2C. Return the vInst, iInst and pInst.
ACS37800ERR ACS37800_readErrorFlags(ACS37800_REGISTER_2D_t *errorFlags);								   // Read volatile register 0x2D. Return its contents in errorFlags.

#if 0
//Change the parameters
void setSenseRes(float newRes); // Change the value of _senseResistance (Ohms)
void setDividerRes(float newRes); // Change the value of _dividerResistance (Ohms)
void setCurrentRange(float newCurrent); // Change the value of _currentSensingRange (Amps)
#endif

#if 0
void acs_setupI2C(acs37800_t* acs, uint8_t i2c_address,
		void *ioInterface, uint8_t (*startTransaction)(void*),
		uint8_t (*sendBytes)(void*,uint8_t,uint8_t*,uint16_t),
		uint8_t (*getBytes)(void*,uint8_t,uint8_t*,uint16_t),
		uint8_t (*endTransaction)(void*));

void acs_setupSPI(acs37800_t *acs,
		void *ioInterface, uint8_t (*startTransaction)(void*),
		uint8_t (*transceiveBytes)(void*, uint8_t, uint8_t*, uint16_t),
		uint8_t (*endTransaction)(void*));
#endif

#if 0
enum ACS_ERROR acs_init(acs37800_t *acs, uint16_t maxVoltage, uint8_t maxCurrent);

/* Basic Chip Access and Value Conversion */
void acs_writeReg(acs37800_t *acs_sensor, uint8_t regAddr, uint32_t regVal);

uint32_t acs_readReg(acs37800_t *acs_sensor, uint8_t regAddr);

float acs_ConvertUnsignedFixedPoint(uint32_t inVal, uint8_t binaryPoint, uint8_t width);

float acs_ConvertSignedFixedPoint(uint32_t inVal, uint8_t binaryPoint, uint8_t width);

int32_t acs_SignExtendBitfield(uint32_t data, uint16_t width);

/* Basic Read Functions - values calculated with voltage and current scale */
void acs_getVIRMS(acs37800_t *acs_sensor, float *vrms, float *irms);

void acs_getPACTIMAG(acs37800_t *acs_sensor, float *pActive, float *pImag);

void acs_getPAPPFACT(acs37800_t *acs_sensor, float *pApparent, float *pFactor, enum ACS_PFACT_DETAILS *pfDetails);

void acs_getVIAVGSEC(acs37800_t *acs_sensor, float *vrmsAvgSec, float *irmsAvgSec);

void acs_getVIAVGMIN(acs37800_t *acs_sensor, float *vrmsAvgMin, float *irmsAvgMin);

void acs_getPACTAVGSEC(acs37800_t *acs_sensor, float *pActiveAvgSec);

void acs_getPACTAVGMIN(acs37800_t *acs_sensor, float *pActiveAvgMin);

void acs_getInstantVI(acs37800_t *acs_sensor, float *vcodes, float *icodes);

void acs_getInstantP(acs37800_t *acs_sensor, float *pinstant);
#endif

#endif /* ACS37800_H_ */