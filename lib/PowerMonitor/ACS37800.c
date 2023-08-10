/*
 * acs37800.c
 *
 * Created: 18.02.2021 10:11:16
 *  Author: gfcwfzkm
 */

#include "ACS37800.h"

#define ACS_READ 0x80
#define ACS_WRITE 0x00

//uint8_t PowerMonitorData[8][64]; // 8颗ACS37800芯片，每颗芯片保留64byte空间保存数据
const float ACS37800_CRS_SNS_GAINS[8] = {1.0f, 2.0f, 3.0f, 3.5f, 4.0f, 4.5f, 5.5f, 8.0f};
uint8_t _acsprintDebug = 0;
float _currentCoarseGain;
float _currentSensingRange;
float _dividerResistance;
float _senseResistance;
SPI_HandleTypeDef *_spiPort;

ACS37800ERR ACS37800_Init(ACS37800_t *acs37800)
{
    ACS37800ERR error = ACS37800_getCurrentCoarseGain(&acs37800->CurrentCoarseGain); // Get the current gain from shadow memory
    if (_acsprintDebug == 1)
    {
        if (error != ACS37800_SUCCESS)
        {
            LOG("ACS37800 init: failed! getCurrentCoarseGain returned: %d\r\n", error);
            return error;
        }
        else
        {
            LOG("ACS37800 init: success! _currentCoarseGain is: %02f\r\n", acs37800->CurrentCoarseGain);
        }
    }
    _acsprintDebug = acs37800->printDebug;
    _currentCoarseGain = acs37800->CurrentCoarseGain;
    _currentSensingRange = acs37800->CurrentSensingRange;
    _dividerResistance = acs37800->DividerResistance;
    _senseResistance = acs37800->SenseResistance;
    _spiPort = acs37800->spiPort;
    return ACS37800_SUCCESS;
}

ACS37800ERR ACS37800_readRegister(uint32_t *data, uint8_t address)
{
    uint8_t spiWResult = MSP_SPI_write(_spiPort, &address, 1);

    if (spiWResult == 0)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readRegister: endTransmission returned: %d", spiWResult);
        }
        return (ACS37800_ERR_SPI_ERROR); // Bail
    }

    // Read 4 bytes (32 bits)
    uint8_t readBuf[4];
    uint8_t spiRResult = MSP_SPI_read(_spiPort, readBuf, 4);
    if (spiRResult == 0)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readRegister: requestFrom returned: %d", spiRResult);
        }
        return (ACS37800_ERR_SPI_ERROR); // Bail
    }
    *data = readBuf[0] + (readBuf[1] << 8) + (readBuf[2] << 16) + (readBuf[3] << 24);

    return (ACS37800_SUCCESS);
}

// Write data to the selected register
ACS37800ERR ACS37800_writeRegister(uint32_t data, uint8_t address)
{
    uint8_t dataBuf[5];
    dataBuf[0] = address;
    dataBuf[1] = data & 0xFF;
    dataBuf[2] = (data >> 8) & 0xFF;
    dataBuf[3] = (data >> 16) & 0xFF;
    dataBuf[4] = (data >> 24) & 0xFF;
    uint8_t spiWResult = MSP_SPI_write(_spiPort, dataBuf, 5);
    if (spiWResult == 0)
    {
        if (_acsprintDebug == 1)
        {
            LOG("writeRegister: endTransmission returned: %d", spiWResult);
        }
        return (ACS37800_ERR_SPI_ERROR); // Bail
    }
    return (ACS37800_SUCCESS);
}

// Set the number of samples for RMS calculations. Bypass_N_Enable must be set/true for this to have effect.
ACS37800ERR ACS37800_setNumberOfSamples(uint32_t numberOfSamples, uint8_t eeprom)
{
    ACS37800ERR error = ACS37800_writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F); // Set the customer access code

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setNumberOfSamples: writeRegister (2F) returned: %d", error);
        }
        return (error); // Bail
    }

    ACS37800_REGISTER_0F_t store;
    error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); // Read register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setNumberOfSamples: readRegister (1F) returned: %d", error);
        }
        return (error); // Bail
    }

    if (_acsprintDebug == 1)
    {
        LOG("setNumberOfSamples: number of samples is currently: %d", store.data.bits.n);
    }

    store.data.bits.n = numberOfSamples & 0x3FF; // Adjust the number of samples (limit to 10 bits)

    error = ACS37800_writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1F); // Write register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setNumberOfSamples: writeRegister (1F) returned: %d", error);
        }
        return (error); // Bail
    }

    if (eeprom) // Check if user wants to set eeprom too
    {
        error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0F); // Read register 0F

        if (error != ACS37800_SUCCESS)
        {
            if (_acsprintDebug == 1)
            {
                LOG("setNumberOfSamples: readRegister (0F) returned: %d", error);
            }
            return (error); // Bail
        }

        if (_acsprintDebug == 1)
        {
            LOG("setNumberOfSamples: eeprom number of samples is currently: %d", store.data.bits.n);
        }

        store.data.bits.n = numberOfSamples & 0x3FF; // Adjust the number of samples (limit to 10 bits)

        error = ACS37800_writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0F); // Write register 0F

        if (error != ACS37800_SUCCESS)
        {
            if (_acsprintDebug == 1)
            {
                LOG("setNumberOfSamples: writeRegister (0F) returned: %d", error);
            }
        }
    }

    error = ACS37800_writeRegister(0, ACS37800_REGISTER_VOLATILE_2F); // Clear the customer access code

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setNumberOfSamples: writeRegister (2F) returned: %d", error);
        }
        return (error); // Bail
    }

    osDelay(100); // Allow time for the shadow/eeprom memory to be updated - otherwise the next readRegister will return zero...

    return (error);
}

// Read and return the number of samples from shadow memory
ACS37800ERR ACS37800_getNumberOfSamples(uint32_t *numberOfSamples)
{
    ACS37800_REGISTER_0F_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); // Read register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("getNumberOfSamples: readRegister (1F) returned: %d", error);
        }
        return (error); // Bail
    }

    if (_acsprintDebug == 1)
    {
        LOG("getNumberOfSamples: number of samples is currently: %d", store.data.bits.n);
    }

    *numberOfSamples = store.data.bits.n; // Return the number of samples

    return (error);
}

// Set/Clear the Bypass_N_Enable flag
ACS37800ERR ACS37800_setBypassNenable(uint8_t bypass, uint8_t eeprom)
{
    ACS37800ERR error = ACS37800_writeRegister(ACS37800_CUSTOMER_ACCESS_CODE, ACS37800_REGISTER_VOLATILE_2F); // Set the customer access code

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setBypassNenable: writeRegister (2F) returned: %d\r\n", error);
        }
        return (error); // Bail
    }

    ACS37800_REGISTER_0F_t store;
    error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); // Read register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setBypassNenable: readRegister (1F) returned: %d\r\n", error);
        }
        return (error); // Bail
    }

    if (_acsprintDebug == 1)
    {
        LOG("setBypassNenable: bypass_n_en is currently: %d\r\n", store.data.bits.bypass_n_en);
    }

    if (bypass) // Adjust bypass_n_en
    {
        store.data.bits.bypass_n_en = 1;
    }
    else
    {
        store.data.bits.bypass_n_en = 0;
    }

    error = ACS37800_writeRegister(store.data.all, ACS37800_REGISTER_SHADOW_1F); // Write register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setBypassNenable: writeRegister (1F) returned: %d\r\n", error);
        }
        return (error); // Bail
    }

    if (eeprom) // Check if user wants to set eeprom too
    {
        error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_EEPROM_0F); // Read register 0F

        if (error != ACS37800_SUCCESS)
        {
            if (_acsprintDebug == 1)
            {
                LOG("setBypassNenable: readRegister (0F) returned: %d\r\n", error);
            }
            return (error); // Bail
        }

        if (_acsprintDebug == 1)
        {
            LOG("setBypassNenable: eeprom bypass_n_en is currently: %d\r\n", store.data.bits.bypass_n_en);
        }

        if (bypass) // Adjust bypass_n_en
        {
            store.data.bits.bypass_n_en = 1;
        }
        else
        {
            store.data.bits.bypass_n_en = 0;
        }

        error = ACS37800_writeRegister(store.data.all, ACS37800_REGISTER_EEPROM_0F); // Write register 0F

        if (error != ACS37800_SUCCESS)
        {
            if (_acsprintDebug == 1)
            {
                LOG("setBypassNenable: writeRegister (0F) returned: %d\r\n", error);
            }
        }
    }

    error = ACS37800_writeRegister(0, ACS37800_REGISTER_VOLATILE_2F); // Clear the customer access code

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("setBypassNenable: writeRegister (2F) returned: %d\r\n", error);
        }
        return (error); // Bail
    }

    osDelay(100); // Allow time for the shadow/eeprom memory to be updated - otherwise the next readRegister will return zero...

    return (error);
}

//// Read and return the bypass_n_en flag from shadow memory
ACS37800ERR ACS37800_getBypassNenable(uint8_t *bypass)
{
    ACS37800_REGISTER_0F_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1F); // Read register 1F

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("getBypassNenable: readRegister (1F) returned: %d", error);
        }
        return (error); // Bail
    }

    if (_acsprintDebug == 1)
    {
        LOG("getBypassNenable: bypass_n_en is currently: %d", store.data.bits.bypass_n_en);
    }

    *bypass = (uint8_t)store.data.bits.bypass_n_en; // Return bypass_n_en

    return (error);
}

// Get the coarse current gain from shadow memory
ACS37800ERR ACS37800_getCurrentCoarseGain(float *currentCoarseGain)
{
    ACS37800_REGISTER_0B_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_SHADOW_1B); // Read register 1B

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("getCurrentCoarseGain: readRegister (1B) returned: %d", error);
        }
        return (error); // Bail
    }

    float gain = ACS37800_CRS_SNS_GAINS[store.data.bits.crs_sns];

    if (_acsprintDebug == 1)
    {
        LOG("getCurrentCoarseGain: shadow gain setting is currently: %02f\r\n", gain);
    }

    *currentCoarseGain = gain; // Return the gain

    return (error);
}

// Read volatile register 0x20. Return the vInst (Volts) and iInst (Amps).
ACS37800ERR ACS37800_readRMS(float *vRMS, float *iRMS)
{
    ACS37800_REGISTER_20_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_20); // Read register 20

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readRMS: readRegister (20) returned: %d", error);
        }
        return (error); // Bail
    }

    // Extract vrms. Convert to voltage in Volts.
    //  Note: datasheet says "RMS voltage output. This field is an unsigned 16-bit fixed point number with 16 fractional bits"
    //  Datasheet also says "Voltage Channel ADC Sensitivity: 110 LSB/mV"
    float volts = (float)store.data.bits.vrms;
    if (_acsprintDebug == 1)
    {
        LOG("readRMS: vrms: 0x%02X", store.data.bits.vrms);
        LOG("readRMS: volts (LSB, before correction) is %02f\r\n", volts);
    }
    volts /= 55000.0; // Convert from codes to the fraction of ADC Full Scale (16-bit)
    volts *= 250;     // Convert to mV (Differential Input Range is +/- 250mV)
    volts /= 1000;    // Convert to Volts
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    float resistorMultiplier = (_dividerResistance + _senseResistance) / _senseResistance;
    volts *= resistorMultiplier;
    if (_acsprintDebug == 1)
    {
        LOG("readRMS: volts (V, after correction) is %02f", volts);
    }
    *vRMS = volts;

    // Extract the irms. Convert to current in Amps.
    // Datasheet says: "RMS current output. This field is a signed 16-bit fixed point number with 15 fractional bits"
    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int

    signedUnsigned.unSigned = store.data.bits.irms; // Extract irms as signed int
    float amps = (float)signedUnsigned.Signed;
    if (_acsprintDebug == 1)
    {
        LOG("readRMS: irms: 0x%02X", store.data.bits.irms);
        LOG("readRMS: amps (LSB, before correction) is %02f\r\n", amps);
    }
    amps /= 55000.0;              // Convert from codes to the fraction of ADC Full Scale (16-bit)
    amps *= _currentSensingRange; // Convert to Amps
    if (_acsprintDebug == 1)
    {
        LOG("readRMS: amps (A, after correction) is %02f\r\n", amps);
    }
    *iRMS = amps;

    return (error);
}

// Read volatile register 0x21. Return the pactive and pimag.
ACS37800ERR ACS37800_readPowerActiveReactive(float *pActive, float *pReactive)
{
    ACS37800_REGISTER_21_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_21); // Read register 21

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readPowerActiveReactive: readRegister (21) returned: %d", error);
        }
        return (error); // Bail
    }

    // Extract pactive. Convert to Watts
    // Note: datasheet says:
    // "Active power output. This field is a signed 16-bit fixed point
    //  number with 15 fractional bits, where positive MaxPow = 0.704,
    //  and negative MaxPow = –0.704. To convert the value (input
    //  power) to line power, divide the input power by the RSENSE and
    //  RISO voltage divider ratio using actual resistor values."
    // Datasheet also says:
    //  "3.08 LSB/mW for the 30A version and 1.03 LSB/mW for the 90A version"

    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int

    signedUnsigned.unSigned = store.data.bits.pactive;

    float power = (float)signedUnsigned.Signed;
    if (_acsprintDebug == 1)
    {
        LOG("readPowerActiveReactive: pactive: 0x%02X", signedUnsigned.unSigned);
        LOG("readPowerActiveReactive: pactive (LSB, before correction) is %02f", power);
    }
    float LSBpermW = 3.08;                   // LSB per mW
    LSBpermW *= 30.0 / _currentSensingRange; // Correct for sensor version
    power /= LSBpermW;                       // Convert from codes to mW
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    float resistorMultiplier = (_dividerResistance + _senseResistance) / _senseResistance;
    power *= resistorMultiplier;
    power /= 1000; // Convert from mW to W
    if (_acsprintDebug == 1)
    {
        LOG("readPowerActiveReactive: pactive (W, after correction) is %02f", power);
    }
    *pActive = power;

    // Extract pimag. Convert to VAR
    // Note: datasheet says:
    // "Reactive power output. This field is an unsigned 16-bit fixed
    //  point number with 16 fractional bits, where MaxPow = 0.704. To
    //  convert the value (input power) to line power, divide the input
    //  power by the RSENSE and RISO voltage divider ratio using actual
    //  resistor values."
    // Datasheet also says:
    //  "6.15 LSB/mVAR for the 30A version and 2.05 LSB/mVAR for the 90A version"

    power = (float)store.data.bits.pimag;
    if (_acsprintDebug == 1)
    {
        LOG("readPowerActiveReactive: pimag: 0x%02X", store.data.bits.pimag);
        LOG("readPowerActiveReactive: pimag (LSB, before correction) is %02f", power);
    }
    float LSBpermVAR = 6.15;                   // LSB per mVAR
    LSBpermVAR *= 30.0 / _currentSensingRange; // Correct for sensor version
    power /= LSBpermVAR;                       // Convert from codes to mVAR
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    power *= resistorMultiplier;
    power /= 1000; // Convert from mVAR to VAR
    if (_acsprintDebug == 1)
    {
        LOG("readPowerActiveReactive: pimag (VAR, after correction) is %02f", power);
    }
    *pReactive = power;

    return (error);
}

// Read volatile register 0x22. Return the apparent power, power factor, leading / lagging, generated / consumed
ACS37800ERR ACS37800_readPowerFactor(float *pApparent, float *pFactor, uint8_t *posangle, uint8_t *pospf)
{
    ACS37800_REGISTER_22_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_22); // Read register 22

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readPowerFactor: readRegister (22) returned: %d", error);
        }
        return (error); // Bail
    }

    // Extract papparent. Convert to VA
    // Note: datasheet says:
    // "Apparent power output magnitude. This field is an unsigned
    //  16-bit fixed point number with 16 fractional bits, where MaxPow
    //  = 0.704. To convert the value (input power) to line power, divide
    //  the input power by the RSENSE and RISO voltage divider ratio
    //  using actual resistor values."
    // Datasheet also says:
    //  "6.15 LSB/mVA for the 30A version and 2.05 LSB/mVA for the 90A version"

    float power = (float)store.data.bits.papparent;
    if (_acsprintDebug == 1)
    {
        LOG("readPowerFactor: papparent: 0x%02X", store.data.bits.papparent);
        LOG("readPowerFactor: papparent (LSB, before correction) is %02f\r\n", power);
    }
    float LSBpermVA = 6.15;                   // LSB per mVA
    LSBpermVA *= 30.0 / _currentSensingRange; // Correct for sensor version
    power /= LSBpermVA;                       // Convert from codes to mVA
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    float resistorMultiplier = (_dividerResistance + _senseResistance) / _senseResistance;
    power *= resistorMultiplier;
    power /= 1000; // Convert from mVAR to VAR
    if (_acsprintDebug == 1)
    {
        LOG("readPowerFactor: papparent (VA, after correction) is %02f\r\n", power);
    }
    *pApparent = power;

    // Extract power factor
    // Datasheet says:
    // "Power factor output. This field is a signed 11-bit fixed point number
    //  with 10 fractional bits. It ranges from –1 to ~1 with a step
    //  size of 2^-10."

    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int

    signedUnsigned.unSigned = store.data.bits.pfactor << 5; // Move 11-bit number into 16-bits (signed)

    float pfactor = (float)signedUnsigned.Signed / 32768.0; // Convert to +/- 1
    if (_acsprintDebug == 1)
    {
        LOG("readPowerFactor: pfactor: 0x%02X", store.data.bits.pfactor);
        LOG("readPowerFactor: pfactor is %02f\r\n", pfactor);
    }
    *pFactor = pfactor;

    // Extract posangle and pospf
    *posangle = store.data.bits.posangle & 0x1;
    *pospf = store.data.bits.pospf & 0x1;

    return (error);
}

// Read volatile registers 0x2A and 0x2C. Return the vInst (Volts), iInst (Amps) and pInst (VAR).
ACS37800ERR ACS37800_readInstantaneous(float *vInst, float *iInst, float *pInst)
{
    ACS37800_REGISTER_2A_t store;
    ACS37800ERR error = ACS37800_readRegister(&store.data.all, ACS37800_REGISTER_VOLATILE_2A); // Read register 2A

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readInstantaneous: readRegister (2A) returned: %d", error);
        }
        return (error); // Bail
    }

    // Extract the vcodes. Convert to voltage in Volts.
    union
    {
        int16_t Signed;
        uint16_t unSigned;
    } signedUnsigned; // Avoid any ambiguity when casting to signed int

    // Extract vcodes as signed int
    // vcodes as actually int16_t but is stored in a uint32_t as a 16-bit bitfield
    signedUnsigned.unSigned = store.data.bits.vcodes;
    float volts = (float)signedUnsigned.Signed;
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: vcodes: 0x%02X", signedUnsigned.unSigned);
        LOG("readInstantaneous: volts (LSB, before correction) is %02f\r\n", volts);
    }
    // Datasheet says "Voltage Channel ADC Sensitivity: 110 LSB/mV"
    volts /= 27500.0; // Convert from codes to the fraction of ADC Full Scale
    volts *= 250;     // Convert to mV (Differential Input Range is +/- 250mV)
    volts /= 1000;    // Convert to Volts
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    float resistorMultiplier = (_dividerResistance + _senseResistance) / _senseResistance;
    volts *= resistorMultiplier;
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: volts (V, after correction) is %02f\r\n", volts);
    }
    *vInst = volts;

    // Extract the icodes. Convert to current in Amps.
    signedUnsigned.unSigned = store.data.bits.icodes; // Extract icodes as signed int
    float amps = (float)signedUnsigned.Signed;
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: icodes: 0x%02X", signedUnsigned.unSigned);
        LOG("readInstantaneous: amps (LSB, before correction) is %02f\r\n", amps);
    }
    amps /= 27500.0;              // Convert from codes to the fraction of ADC Full Scale
    amps *= _currentSensingRange; // Convert to Amps
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: amps (A, after correction) is %02f\r\n", amps);
    }
    *iInst = amps;

    ACS37800_REGISTER_2C_t pstore;
    error = ACS37800_readRegister(&pstore.data.all, ACS37800_REGISTER_VOLATILE_2C); // Read register 2C

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readInstantaneous: readRegister (2C) returned: %d", error);
        }
        return (error); // Bail
    }

    // Extract pinstant as signed int. Convert to W
    // pinstant as actually int16_t but is stored in a uint32_t as a 16-bit bitfield
    signedUnsigned.unSigned = pstore.data.bits.pinstant;
    float power = (float)signedUnsigned.Signed;
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: pinstant: 0x%02X", signedUnsigned.unSigned);
        LOG("readInstantaneous: power (LSB, before correction) is %02f\r\n", power);
    }
    // Datasheet says: 3.08 LSB/mW for the 30A version and 1.03 LSB/mW for the 90A version
    float LSBpermW = 3.08;                   // LSB per mW
    LSBpermW *= 30.0 / _currentSensingRange; // Correct for sensor version
    power /= LSBpermW;                       // Convert from codes to mW
    // Correct for the voltage divider: (RISO1 + RISO2 + RSENSE) / RSENSE
    // Or:  (RISO1 + RISO2 + RISO3 + RISO4 + RSENSE) / RSENSE
    power *= resistorMultiplier;
    power /= 1000; // Convert from mW to W
    if (_acsprintDebug == 1)
    {
        LOG("readInstantaneous: power (W, after correction) is %02f\r\n", power);
    }
    *pInst = power;

    return (error);
}

// Read volatile register 0x2D. Return the error flags.
ACS37800ERR ACS37800_readErrorFlags(ACS37800_REGISTER_2D_t *errorFlags)
{
    ACS37800ERR error = ACS37800_readRegister(&errorFlags->data.all, ACS37800_REGISTER_VOLATILE_2D); // Read register 2D

    if (error != ACS37800_SUCCESS)
    {
        if (_acsprintDebug == 1)
        {
            LOG("readErrorFlags: readRegister (2D) returned: %d\r\n", error);
        }
    }

    return (error);
}

#if 0
//Change the value of the sense resistor (Ohms)
void ACS37800_setSenseRes(float newRes)
{
  _senseResistance = newRes;
}

//Change the value of the voltage divider resistance (Ohms)
void ACS37800_setDividerRes(float newRes)
{
  _dividerResistance = newRes;
}

//Change the current-sensing range (Amps)
//ACS37800KMACTR-030B3-I2C is a 30.0 Amp part - as used on the SparkFun Qwiic Power Meter
//ACS37800KMACTR-090B3-I2C is a 90.0 Amp part
void ACS37800_setCurrentRange(float newCurrent)
{
  _currentSensingRange = newCurrent;
}
#endif
