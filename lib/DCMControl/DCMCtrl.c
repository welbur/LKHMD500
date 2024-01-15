/******************************************************************
 * 功能：用于控制恒流模块，采用DAC方式控制恒流模块的输出电流
 * 作者：WU555
 * 时间：2023-10-19
 *******************************************************************/
#include "DCMCtrl.h"
#include "MSP_DAC.h"
#include "math.h"
#include <stdlib.h>
#include "ee.h"
#include "eeConfig.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern ADC_HandleTypeDef hadc4;
extern ADC_HandleTypeDef hadc5;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;
extern DMA_HandleTypeDef hdma_adc4;
extern DMA_HandleTypeDef hdma_adc5;

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;
extern DAC_HandleTypeDef hdac3;
extern DAC_HandleTypeDef hdac4;
extern OPAMP_HandleTypeDef hopamp1;
extern OPAMP_HandleTypeDef hopamp4;

DCModule_AnalogIn_t DCM_AnalogIn_Buffer;

float DCM_SetOutCurr_Buffer[DCModuleNum];           //用于保存设置的输出电流值
uint8_t DCM_Adjust_OutCurr_Buffer[DCModuleNum] = {100, 100, 100, 100, 100};     //用于保存设置的调整电流的百分比值

uint8_t DCM_Fault_Flag[DCModuleNum];
uint8_t DCM_Pdet_Flag[DCModuleNum];
uint8_t DCM_Fault_PinValue_Cache[DCModuleNum];
uint8_t DCM_Pdet_PinValue_Cache[DCModuleNum];
float DCM_GetOutCurr_Cache[DCModuleNum];
DCModule_State_t DCM_State[DCModuleNum];

float DCM_OUTCurr_Max       = 20.0; // 输出最大电流
float DCM_OUTCurr_Min       = 0.8;  // 输出最小电流
float DCM_OUTCurr_Limit     = 0.3;  // 判断有无电流的最小值

/********************** 需要在其他文件中使用的参数，全局变量 ***************************/
float Get_OUTCurr_Volt[DCModuleNum];
float Get_SetCurr_Volt[DCModuleNum] = {0, 0, 0, 0, 0};

uint8_t DCM_SetCurr_Now_Cnt[DCModuleNum] = {0, 0, 0, 0, 0};


#ifdef USE_AdjustFunc
DCM_Adjust_FromPLC_t DCM_Adjust_Flag[DCModuleNum] = {0, 0, 0, 0, 0};
#endif

/*********************************************************************************/

float _KFP_Last_out = 0; // 用于卡尔曼滤波
KFP KFP_DCM1 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM2 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM3 = {0.0, 1, 0, 0.0, 0.0, 0.01, 2.5};
KFP KFP_DCM4 = {0.0, 1, 0, 0.0, 0.0, 0.01, 2.5};
KFP KFP_DCM5 = {0.0, 1, 0, 0.0, 0.0, 0.008, 8.5};
//KFP KFP_DCMV1 = {0.0, 1, 0, 0.0, 0.0, 0.01, 2.5};


#ifdef USE_EEPROMFunc
uint8_t have_para_inflash = 0;              //用于判断参数是否在flash中
#endif



/* 输出电流控制信号ctrl，分段比例系数 
 * 0.8A~1.5A,   1.5A~2.5A,    2.5A~3.5A,    3.5A~4.5A,    4.5A~5.5A,   5.5A~6.5A,   6.5A~7.5A,   7.5A~8.5A,   8.5A~9.5A,   9.5A~10.5A, 
 * 10.5A~11.5A, 11.5A~12.5A,  12.5A~13.5A,  13.5A~14.5A,  14.5A~15.5A, 15.5A~16.5A, 16.5A~17.5A, 17.5A~18.5A, 18.5A~19.5A, 19.5A~20.0A
*/
//调高，电流减小
uint16_t DCM_SetCurr_Gain_Default[DCModuleNum][DCM_Gain_Num] = {
    /* 1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    */
    {1344, 1340, 1354, 1352, 1351, 1351, 1348, 1345, 1344, 1337, 1327, 1322, 1320, 1318, 1315, 1312, 1310, 1308, 1308, 1308},   //DCM1
    {1336, 1336, 1356, 1354, 1354, 1353, 1349, 1345, 1341, 1333, 1326, 1323, 1321, 1320, 1317, 1315, 1311, 1313, 1313, 1313},   //DCM2 5
    {1334, 1370, 1363, 1362, 1364, 1362, 1358, 1352, 1341, 1331, 1327, 1324, 1322, 1320, 1318, 1317, 1317, 1316, 1317, 1317},   //DCM3 ok
    {1334, 1334, 1332, 1348, 1349, 1348, 1347, 1345, 1341, 1334, 1323, 1318, 1315, 1313, 1311, 1310, 1309, 1307, 1305, 1305},   //DCM4
    {1345, 1343, 1359, 1356, 1358, 1358, 1355, 1350, 1345, 1334, 1324, 1320, 1319, 1317, 1316, 1315, 1313, 1312, 1312, 1312},   //DCM5     
};
uint16_t DCM_SetCurr_Gain_Flash[DCModuleNum][DCM_Gain_Num];


/* 输出电流反馈信号curr，分段比例系数 
 * 0.8A~1.5A,   1.5A~2.5A,    2.5A~3.5A,    3.5A~4.5A,    4.5A~5.5A,   5.5A~6.5A,   6.5A~7.5A,   7.5A~8.5A,   8.5A~9.5A,   9.5A~10.5A,
 * 10.5A~11.5A, 11.5A~12.5A,  12.5A~13.5A,  13.5A~14.5A,  14.5A~15.5A, 15.5A~16.5A, 16.5A~17.5A, 17.5A~18.5A, 18.5A~19.5A, 19.5A~20.0A
*/
uint16_t DCM_OUTCurr_Gain_Default[DCModuleNum][DCM_Gain_Num] = {
    /*1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20     */
    {736, 734, 739, 740, 743, 739, 737, 734, 732, 731, 727, 726, 725, 724, 726, 722, 722, 721, 724, 724},                   //DCM1
    {730, 730, 740, 740, 736, 734, 735, 733, 732, 730, 724, 723, 722, 720, 719, 719, 720, 719, 717, 717},                   //DCM2
    {735, 741, 743, 737, 735, 729, 729, 731, 729, 719, 723, 712, 721, 716, 718, 717, 708, 717, 717, 717},                   //DCM3
    {748, 741, 732, 739, 740, 738, 737, 736, 734, 732, 726, 726, 724, 723, 722, 723, 719, 717, 717, 717},                   //DCM4          
    {735, 736, 741, 742, 741, 738, 740, 736, 734, 733, 728, 727, 726, 725, 725, 722, 721, 720, 717, 717},                   //DCM5          
}; 
uint16_t DCM_OUTCurr_Gain_Flash[DCModuleNum][DCM_Gain_Num];




void dcm_init(void)
{
    ADC_CONV_COMPLETE_FLAG = 0;

    memset(DCM_Fault_PinValue_Cache, 0, sizeof(DCM_Fault_PinValue_Cache));
    memset(DCM_Pdet_PinValue_Cache, 0, sizeof(DCM_Pdet_PinValue_Cache));
    memset(DCM_SetOutCurr_Buffer, 0, sizeof(DCM_SetOutCurr_Buffer));

    /* 从FLASH读取参数：SetCurr_Gain比例系数 和 OutCurr_Gain比例系数 */
    dcm_read_data_fromFLASH();
    LOG("have_para_inflash : %d\r\n", have_para_inflash);

    /*启动所有模拟输出通道*/
    for (int i = 0; i < DCModuleNum; i++)
        start_analogout(i);

    /*复位所有模拟输出通道*/
    for (int i = 0; i < DCModuleNum; i++)
        set_analogout(i, 0);

    /*启动所有ADC通道*/
    for (int i = 0; i < DCModuleNum; i++)
        start_analogin(i);
}


void open_dcm_outcurr(uint8_t ch, float curr)
{
    dcm_set_outcurrent(ch, curr);
    dcm_enable_output(ch, 1);
}

void close_dcm_outcurr(uint8_t ch)
{
    dcm_enable_output(ch, 0);
    dcm_set_outcurrent(ch, 0);
}

uint8_t dcm_enable_output(uint8_t ch, uint8_t en)
{
    if (en == 1)
        if ((DCM_SetOutCurr_Buffer[ch] > DCM_OUTCurr_Max) || (DCM_SetOutCurr_Buffer[ch] < DCM_OUTCurr_Min))
            en = 0;         //return 0;

    if (ch == DCModule_1)
        DCM1_EN(en);
    else if (ch == DCModule_2)
        DCM2_EN(en);
    else if (ch == DCModule_3)
        DCM3_EN(en);
    else if (ch == DCModule_4)
        DCM4_EN(en);
    else if (ch == DCModule_5)
    {
        DCM5_EN(en);
    }
    else
        return 0;

    /* 关闭恒流模块的同时，需要把DAC引脚设为0 */
    if (en == 0)
        set_analogout(ch, 0);

    return 1;
}

uint8_t dcm_read_enable_value(uint8_t ch)
{
    uint8_t en_value = 0;
    switch (ch)
    {
    case DCModule_1:
        en_value = HAL_GPIO_ReadPin(DCM1_EN_Port, DCM1_EN_Pin);
        break;

    case DCModule_2:
        en_value = HAL_GPIO_ReadPin(DCM2_EN_Port, DCM2_EN_Pin);
        break;

    case DCModule_3:
        en_value = HAL_GPIO_ReadPin(DCM3_EN_Port, DCM3_EN_Pin);
        break;

    case DCModule_4:
        en_value = HAL_GPIO_ReadPin(DCM4_EN_Port, DCM4_EN_Pin);
        break;

    case DCModule_5:
        en_value = HAL_GPIO_ReadPin(DCM5_EN_Port, DCM5_EN_Pin);
        break;

    default:
        LOG_error("dcm_read_enable_value channel error!\r\n");
    }
    return en_value;
}

/**
 * @brief 设置某一恒流模块的输出电流值
 * 
 * @param ch 指向第几个恒流模块
 * @param value 电流值 0～20A
 * @return uint8_t 1:表示设置成功
 */
uint8_t dcm_set_outcurrent(uint8_t ch, float curr)
{
    float volt;
    /* 保存设置的输出电流值 */
    DCM_SetOutCurr_Buffer[ch] = curr;

    /* 输出电流设置超过阀值的话，报错 */
    if ((curr > DCM_OUTCurr_Max) || (curr < DCM_OUTCurr_Min))
    {
        if (curr != 0)
        {
            LOG_error("dcm_set_outcurrent error !!! channel %d, curr : %f\r\n", ch, curr);
            return 0;
        }
    }

    /* 加上百分比值 */
    curr = curr * DCM_Adjust_OutCurr_Buffer[ch] / 100;
    if ((curr > DCM_OUTCurr_Max) || (curr < DCM_OUTCurr_Min))
    {
        if (curr != 0)
        {
            LOG_error("in dcm_set_outcurrent......dcm_set adjust out curr error !!! channel %d, curr : %f\r\n", ch, curr);
            DCM_Adjust_Flag[ch] = DCM_Adjust_FromPLC_SetData_Err;
            return 0;
        }
    }

    DCM_SetCurr_Now_Cnt[ch] = round(curr) - 1;
    
    if (have_para_inflash == 1)
    {
        uint16_t scgdata = DCM_SetCurr_Gain_Flash[ch][DCM_SetCurr_Now_Cnt[ch]];
        volt = curr * DCM_Gain_Multi / scgdata;     //DCM_SetCurr_Gain_Flash[ch][DCM_SetCurr_Now_Cnt[ch]];
        //LOG("in'dcm_set_outcurrent', DCM_SetCurr_Gain_Flash[][%d] = %d\r\n", DCM_SetCurr_Now_Cnt[ch], scgdata);
    }
    else
        volt = curr * DCM_Gain_Multi / DCM_SetCurr_Gain_Default[ch][DCM_SetCurr_Now_Cnt[ch]];

    Get_SetCurr_Volt[ch] = volt;
    
    uint32_t data = (uint32_t)(volt * DCM_DAC_MAX * DCM_Vref_Multi / DCM_Vref);
    set_analogout(ch, data);
    return 1;
}

/**
 * @brief 设置某一恒流模块的调整电流值
 * 
 * @param ch 指向第几个恒流模块
 * @param value 调整电流的百分比 50～150， 50 = 50%
 * @return uint8_t 1:表示设置成功
 */
uint8_t dcm_set_adjustcurr(uint8_t ch, uint8_t value)
{
    if (value == 0)
        return 1;
        
    /* 输出电流设置超过阀值的话，报错 */
    if ((value > 150) || (value < 50) || (value < 0))
    {
        LOG_error("dcm_set adjust out curr error !!! channel %d, curr : %d\r\n", ch, value);
        DCM_Adjust_Flag[ch] = DCM_Adjust_FromPLC_SetData_Err;
        return 0;
    }

    /* 保存设置的输出电流百分比值 */
    DCM_Adjust_OutCurr_Buffer[ch] = value;
    return 1;
}

/**
 * @brief 获取某一恒流模块的输出电流值
 *
 * @param ch 指向第几个恒流模块
 * @return float 返回当前恒流模块的输出电流值
 */
float dcm_get_outcurrent(uint8_t ch)
{
    float curr, currvolt, currvoltkfp;
    uint32_t c_u32;
    switch (ch)
    {
    case DCModule_1:    
        c_u32 = DCM_AnalogIn_Buffer.DCM1_ANALOGIN[DCM1_OUTCurr_Index];
        break;

    case DCModule_2:
        c_u32 = DCM_AnalogIn_Buffer.DCM2_ANALOGIN[DCM2_OUTCurr_Index];
        break;

    case DCModule_3:
        c_u32 = DCM_AnalogIn_Buffer.DCM3_ANALOGIN[DCM3_OUTCurr_Index];
        break;

    case DCModule_4:
        c_u32 = DCM_AnalogIn_Buffer.DCM4_ANALOGIN[DCM4_OUTCurr_Index];
        break;

    case DCModule_5:
        c_u32 = DCM_AnalogIn_Buffer.DCM5_ANALOGIN[DCM5_OUTCurr_Index];
        break;

    default:
        LOG_error("get outcurrent error!\r\n");
        c_u32 = 0;
    }
    
    currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
//    LOG("%0.3f ", currvolt);
#ifdef USE_KFP
    currvoltkfp = kalmanFilter(KFP_DCM_Curr_Index(ch), currvolt);
#else
    currvoltkfp = currvolt;
#endif
//    LOG("%0.3f ", currvoltkfp);
    Get_OUTCurr_Volt[ch] = currvoltkfp;
    if (have_para_inflash == 1)
        curr = currvoltkfp * DCM_OUTCurr_Gain_Flash[ch][DCM_SetCurr_Now_Cnt[ch]] / (float)DCM_Gain_Multi;   //从flash读取的参数
    else
        curr = currvoltkfp * DCM_OUTCurr_Gain_Default[ch][DCM_SetCurr_Now_Cnt[ch]] / (float)DCM_Gain_Multi;
//    LOG("%0.3f ", curr);
    /* 扣除百分比，再输出给PLC */
    curr = curr * 100 / DCM_Adjust_OutCurr_Buffer[ch];
//    LOG("%0.3f \r\n", curr);
    return curr;
}

/**
 * @brief 获取某一恒流模块的输出电压
 *
 * @param ch 指向第几个恒流模块
 * @return float 返回当前恒流模块的输出电压
 */
float dcm_get_outvoltage(uint8_t ch)
{
    float volt, volt_v;
    uint32_t v_u32;
    switch (ch)
    {
    case DCModule_1:
        v_u32 = DCM_AnalogIn_Buffer.DCM1_ANALOGIN[DCM1_OUTVolt_Index];
        break;

    case DCModule_2:
        v_u32 = DCM_AnalogIn_Buffer.DCM2_ANALOGIN[DCM2_OUTVolt_Index];
        break;

    case DCModule_3:
        v_u32 = DCM_AnalogIn_Buffer.DCM3_ANALOGIN[DCM3_OUTVolt_Index];
        break;

    case DCModule_4:
        v_u32 = DCM_AnalogIn_Buffer.DCM4_ANALOGIN[DCM4_OUTVolt_Index];
        break;

    case DCModule_5:
        v_u32 = DCM_AnalogIn_Buffer.DCM5_ANALOGIN[DCM5_OUTVolt_Index];
        break;

    default:
        LOG_error("get OUTVoltage error!\r\n");
        return NAN;
    }
    //LOG("%ld ", v_u32);
    volt_v = (float)(v_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
    
    volt = volt_v * DCM_OUTVolt_Gain / (float)DCM_Gain_Multi;

    return volt;
}

/**
 * @brief 用于判断某一恒流模块的状态
 *
 *
 * @param ch
 * @return int
 */
DCModule_State_t dcm_get_dcmstate(uint8_t ch)
{
    /*1. 输出电流设置超过阀值的话，报错: 40101 */
    if (((DCM_SetOutCurr_Buffer[ch] > DCM_OUTCurr_Max) || (DCM_SetOutCurr_Buffer[ch] < DCM_OUTCurr_Min)))
    {
        if (DCM_SetOutCurr_Buffer[ch] != 0)
        {
            DCM_State[ch] = DCMState_Set_OutCurr_Err;
            return DCM_State[ch];
        }
    }

#ifdef USE_AdjustFunc
    /* 7. 判断是否有校准状态变化 */
    if (DCM_Adjust_Flag[ch] != 0)
    {
        DCM_State[ch] = 10500 + DCM_Adjust_Flag[ch];
        return DCM_State[ch];
    }
#endif

    /* 2. 先判断是否有输入电压: 30201 */
    if (dcm_get_pdet(ch) == DCM_No_INVolt)
    {
        DCM_State[ch] = DCMState_IN_NoVolt;
        return DCM_State[ch];
    }

    /* 3. 判断输出是否过载: 30101 */
    if ((DCM_SetOutCurr_Buffer[ch] != 0) && (dcm_get_outcurrent(ch) - DCM_SetOutCurr_Buffer[ch] > 0.3)) //(DCM_SetOutCurr_Buffer[ch] + 1.0)))
    {
        DCM_State[ch] = DCMState_OUT_OverCurr;
        return DCM_State[ch];
    }

    /* 4. 判断输出是否断路：30301；断路有几种情况：未接负载、输出线断开、接触不良 */
    if ((dcm_read_enable_value(ch) == Enable_DCM) && (dcm_get_outcurrent(ch) < DCM_OUTCurr_Limit))
    {
        DCM_State[ch] = DCMState_OUT_Break;
        return DCM_State[ch];
    }

    /* 5. 判断输出电压是否过高：  */

    /* 6. 判断输出电压是否过低：  */

#if 1    
    /* 0. 判断恒流模块是否报Fault错误: 30401 */
    if (dcm_get_dcmfault(ch) == DCM_Fault)
    {
        DCM_State[ch] = DCMState_Fault;
        return DCM_State[ch];
    }
#endif



    DCM_State[ch] = DCMState_Work;
    return DCM_State[ch];
}

#ifdef USE_AdjustFunc
void dcm_write_adjustflag(uint8_t ch, DCM_Adjust_FromPLC_t dcmadj)
{
    DCM_Adjust_Flag[ch] = 10500 + dcmadj;
}
#endif

/**
 * @brief 获取某一恒流模块的fault信号
 * 恒流模块有异常，会报一个fault信号 1:错误； 0:正常
 * @param ch
 * @return uint8_t
 */
uint8_t dcm_get_dcmfault(uint8_t ch)
{
    uint8_t faultpinvalue = dcm_read_fault_value(ch);
    if (DCM_Fault_PinValue_Cache[ch] == faultpinvalue)
        DCM_Fault_Flag[ch] = faultpinvalue;
    else
        DCM_Fault_PinValue_Cache[ch] = faultpinvalue;

    return DCM_Fault_Flag[ch];
}

#if 1
uint8_t dcm_read_fault_value(uint8_t ch)
{
    uint8_t fault = 0;
    switch (ch)
    {
    case DCModule_1:
        fault = HAL_GPIO_ReadPin(DCM1_Fault_Port, DCM1_Fault_Pin);
        break;

    case DCModule_2:
        fault = HAL_GPIO_ReadPin(DCM2_Fault_Port, DCM2_Fault_Pin);
        break;

    case DCModule_3:
        fault = HAL_GPIO_ReadPin(DCM3_Fault_Port, DCM3_Fault_Pin);
        break;

    case DCModule_4:
        fault = HAL_GPIO_ReadPin(DCM4_Fault_Port, DCM4_Fault_Pin);
        break;

    case DCModule_5:
#ifndef DEVBoardG4
        fault = HAL_GPIO_ReadPin(DCM5_Fault_Port, DCM5_Fault_Pin);
#else
        // LOG("is in DEVBoardG4, no DCM5_fault\r\n");
#endif
        break;

    default:
        LOG_error("dcm_get_dcmfault channel error!\r\n");
    }
    return fault;
}
#endif

/**
 * @brief 获取某一恒流模块的输入电压状态
 * 1:有电压输入； 0:无输入电压
 * @param ch
 * @return uint8_t
 */
uint8_t dcm_get_pdet(uint8_t ch)
{
    uint8_t pdetpinvalue = dcm_read_pdet_value(ch);
    if (DCM_Pdet_PinValue_Cache[ch] == pdetpinvalue)
        DCM_Pdet_Flag[ch] = pdetpinvalue;
    else
        DCM_Pdet_PinValue_Cache[ch] = pdetpinvalue;

    return DCM_Pdet_Flag[ch];
}

#if 1
uint8_t dcm_read_pdet_value(uint8_t ch)
{
    uint8_t pdet = 0;
    switch (ch)
    {
    case DCModule_1:
        pdet = HAL_GPIO_ReadPin(DCM1_Pdet_Port, DCM1_Pdet_Pin);
        break;

    case DCModule_2:
        pdet = HAL_GPIO_ReadPin(DCM2_Pdet_Port, DCM2_Pdet_Pin);
        break;

    case DCModule_3:
        pdet = HAL_GPIO_ReadPin(DCM3_Pdet_Port, DCM3_Pdet_Pin);
        break;

    case DCModule_4:
        pdet = HAL_GPIO_ReadPin(DCM4_Pdet_Port, DCM4_Pdet_Pin);
        break;

    case DCModule_5:
        pdet = HAL_GPIO_ReadPin(DCM5_Pdet_Port, DCM5_Pdet_Pin);
        break;

    default:
        LOG_error("dcm_get_pdet channel error!\r\n");
    }
    return pdet;
}
#endif

/**
 * @brief 启动指定通道的DAC模拟输出
 *
 * @param ch 通道号
 */
void start_analogout(uint8_t ch)
{
    switch (ch)
    {
    case DCModule_1:
        HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
        break;

    case DCModule_2:
        HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
        break;

    case DCModule_3:
        HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
        break;

    case DCModule_4:
        HAL_DAC_Start(&hdac3, DAC_CHANNEL_1);
        HAL_OPAMP_Start(&hopamp1);
        break;

    case DCModule_5:
        HAL_DAC_Start(&hdac4, DAC_CHANNEL_1);
        HAL_OPAMP_Start(&hopamp4);
        break;

    default:
        LOG_error("start analogout channel error!\r\n");
        break;
    }
}

/**
 * @brief 停止指定通道的DAC模拟输出
 *
 * @param ch 通道号
 */
void stop_analogout(uint8_t ch)
{
    switch (ch)
    {
    case DCModule_1:
        HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
        break;

    case DCModule_2:
        HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
        break;

    case DCModule_3:
        HAL_DAC_Stop(&hdac2, DAC_CHANNEL_1);
        break;

    case DCModule_4:
        HAL_DAC_Stop(&hdac3, DAC_CHANNEL_1);
        HAL_OPAMP_Stop(&hopamp1);
        break;

    case DCModule_5:
        HAL_DAC_Stop(&hdac4, DAC_CHANNEL_1);
        HAL_OPAMP_Stop(&hopamp4);
        break;

    default:
        LOG_error("stop analogout channel error!\r\n");
        break;
    }
}

/**
 * @brief 设置DAC模拟输出通道的值
 *
 * @param ch 通道号
 * @param value 输出值
 */
void set_analogout(uint8_t ch, uint32_t value)
{
    //LOG("set_analogout....%d, ......value : %ld \r\n", ch, value);
    switch (ch)
    {
    case DCModule_1:
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_2:
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_3:
        //LOG("value 3 : %ld\r\n", value);
        HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_4:
        HAL_DAC_SetValue(&hdac3, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_5:
        HAL_DAC_SetValue(&hdac4, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    default:
        LOG_error("analog out channel error!\r\n");
        break;
    }
}

void start_analogin(uint8_t ch)
{
    switch (ch)
    {
    case DCModule_1:
#ifndef DEVBoardG4
        /* Run the ADC calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK)
            Error_Handler();
        
        /* Start ADC conversion used dma*/
        // if (HAL_ADC_Start_DMA(&hadc2) != HAL_OK)    //, DCM_ANALOGIN_Size) != HAL_OK)
        if (HAL_ADC_Start_DMA(&hadc2, (uint32_t *)DCM_AnalogIn_Buffer.DCM1_ANALOGIN, DCM_ANALOGIN_Size) != HAL_OK)
            Error_Handler();
#endif
        break;

    case DCModule_2:
        /* Run the ADC calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
            Error_Handler();
    
        /* Start ADC conversion used dma*/
        // if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&tbuffer, 1) != HAL_OK)
        if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)DCM_AnalogIn_Buffer.DCM2_ANALOGIN, DCM_ANALOGIN_Size) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_3:
        /* Run the ADC calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED) != HAL_OK)
            Error_Handler();

        /* Start ADC conversion used dma*/
        if (HAL_ADC_Start_DMA(&hadc3, (uint32_t *)DCM_AnalogIn_Buffer.DCM3_ANALOGIN, DCM_ANALOGIN_Size) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_4:
        /* Run the ADC calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED) != HAL_OK)
            Error_Handler();

        /* Start ADC conversion used dma*/
        if (HAL_ADC_Start_DMA(&hadc4, (uint32_t *)DCM_AnalogIn_Buffer.DCM4_ANALOGIN, DCM_ANALOGIN_Size) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_5:
        /* Run the ADC calibration in single-ended mode */
        if (HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED) != HAL_OK)
            Error_Handler();

        /* Start ADC conversion used dma*/
        if (HAL_ADC_Start_DMA(&hadc5, (uint32_t *)DCM_AnalogIn_Buffer.DCM5_ANALOGIN, DCM_ANALOGIN_Size) != HAL_OK)
            Error_Handler();
        break;

    default:
        LOG_error("calibrate analogin channel error!\r\n");
        break;
    }
}

void stop_analogin(uint8_t ch)
{
    switch (ch)
    {
    case DCModule_1:
        /* Stop ADC2 conversion */
        if (HAL_ADC_Stop_DMA(&hadc2) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_2:
        /* Stop ADC1 conversion */
        if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_3:
        /* Stop ADC3 conversion */
        if (HAL_ADC_Stop_DMA(&hadc3) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_4:
        /* Stop ADC4 conversion */
        if (HAL_ADC_Stop_DMA(&hadc4) != HAL_OK)
            Error_Handler();
        break;

    case DCModule_5:
        /* Stop ADC5 conversion */
        if (HAL_ADC_Stop_DMA(&hadc5) != HAL_OK)
            Error_Handler();
        break;

    default:
        LOG_error("calibrate analogin channel error!\r\n");
        break;
    }
}

#ifdef USE_AdjustFunc
void dcm_read_data_fromFLASH(void)
{
    uint8_t ee_use_sf[2];
    uint16_t ee_use_sf_16;

    /* 读取保存参数的页面中的第1、2字节 */
    if (ee_read(0, sizeof(ee_use_sf), ee_use_sf) == true)
    {
        /* 如果有校准数据在FLASH中，则采用FLASH中的参数 */
        ee_use_sf_16 = ee_use_sf[0] + (ee_use_sf[1] << 8);
        if (ee_use_sf_16 == ee_use_StartFrame)
        {
            /* 读取校准数据 */
            uint8_t ee_read_data[500];
            uint32_t baseaddr = _EE_USE_OUTCurrGain_START_ADDR - _EE_USE_SetCurrGain_START_ADDR;
            if (ee_read(_EE_USE_SetCurrGain_START_ADDR, sizeof(ee_read_data), ee_read_data) == true)
            {
                LOG("in 'dcm_read_data_fromFLASH', [DCM_SetCurr_Gain_Flash , DCM_OUTCurr_Gain_Flash] : ");
                for (int i = 0; i < DCModuleNum; i++)
                {
                    for (int j = 0; j < DCM_Gain_Num; j++)
                    {
                        DCM_SetCurr_Gain_Flash[i][j] = word(ee_read_data[i * DCM_Gain_Num * 2 + j * 2 + 1], ee_read_data[i * DCM_Gain_Num * 2 + j * 2]);
                        if (DCM_SetCurr_Gain_Flash[i][j] >= 1500 || DCM_SetCurr_Gain_Flash[i][j] <= 1000)
                            DCM_SetCurr_Gain_Flash[i][j] = DCM_SetCurr_Gain_Default[i][j];
                        
                        DCM_OUTCurr_Gain_Flash[i][j] = word(ee_read_data[i * DCM_Gain_Num * 2 + j * 2 + 1 + baseaddr], ee_read_data[i * DCM_Gain_Num * 2 + j * 2 + baseaddr]);
                        if (DCM_OUTCurr_Gain_Flash[i][j] >= 1000 || DCM_OUTCurr_Gain_Flash[i][j] <= 500)
                            DCM_OUTCurr_Gain_Flash[i][j] = DCM_OUTCurr_Gain_Default[i][j];
                        
                        LOG(" [%d,%d] ", DCM_SetCurr_Gain_Flash[i][j], DCM_OUTCurr_Gain_Flash[i][j]);
                    }
                }
                LOG(".......\r\n");
                have_para_inflash = 1;
                LOG_info("read data from FLASH success! \r\n");
                return;
            }  
        } 
    } 

    have_para_inflash = 0;
}
#endif


/**
 *卡尔曼滤波器
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为1
    float Last_out;//上次卡尔曼滤波器输出 初始化值为0
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.0001  Q值越大，比较相信测量值；Q值越小，比较相信预测值，Q越小，滤波越平滑，但滤波结果越滞后
    float R;//观测噪声协方差 初始化值为5
    示例参数：KFP KFP_DCM5 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
#if 1
float kalmanFilter(KFP *kfp, float input)
{
    if (abs(_KFP_Last_out - input) >= 2)
        kfp->Last_out = input * 0.382 + kfp->Last_out * 0.618;

    // 预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    kfp->Now_P = kfp->LastP + kfp->Q;
    // 卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
    // 更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    kfp->out = kfp->Last_out + kfp->Kg * (input - kfp->Last_out);        //kfp->out = kfp->out + kfp->Kg * (input - kfp->out); // 因为这一次的预测值就是上一次的输出值
    // 更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
    kfp->LastP = (1 - kfp->Kg) * kfp->Now_P;
    kfp->Last_out = kfp->out;
    return kfp->out;
}
#endif

KFP* KFP_DCM_Curr_Index(uint8_t ch)
{
    KFP* dcm_kfp;
    switch(ch)
    {
        case DCModule_1:
            dcm_kfp = &KFP_DCM1;
            break;

        case DCModule_2:
            dcm_kfp = &KFP_DCM2;
            break;
        
        case DCModule_3:
            dcm_kfp = &KFP_DCM3;
            break;

        case DCModule_4:
            dcm_kfp = &KFP_DCM4;
            break;

        case DCModule_5:
            dcm_kfp = &KFP_DCM5;
            break;
        
        default:
            dcm_kfp = &KFP_DCM1;
            LOG_error("KFP_DCM_Curr_Index !\r\n");
    }
    return dcm_kfp;
}


/**
 * @brief 用于打印LOG
 * 
 */
char *DCModule_State_Log(DCModule_State_t dcmstate)
{
    char * dcmsl = "No LOG!";
    switch(dcmstate)
    {
        case DCMState_Work:
            dcmsl = "Working Fine!";
            break;

        case DCMState_OUT_OverCurr:
            dcmsl = "OUTPUT Over Current!";
            break;
        
        case DCMState_IN_NoVolt:
            dcmsl = "INPUT No Voltage!";
            break;

        case DCMState_OUT_Break:
            dcmsl = "OUTPUT Break!";
            break;

        case DCMState_Fault:
            dcmsl = "DC Module Fault!";
            break;

        case DCMState_Set_OutCurr_Err:
            dcmsl = "Set Output Current Error!";
            break;
        
        default:
            //LOG_error("DCModule_State_Log !\r\n");
            break;
    }
    return dcmsl;
}