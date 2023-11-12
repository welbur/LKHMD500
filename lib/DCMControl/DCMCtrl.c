/******************************************************************
 * 功能：用于控制恒流模块，采用DAC方式控制恒流模块的输出电流
 * 作者：WU555
 * 时间：2023-10-19
 *******************************************************************/
#include "DCMCtrl.h"
#include "MSP_DAC.h"
#include "math.h"
#include <stdlib.h>

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

float DCM_SetOutCurr_Buffer[DCModuleNum];
uint8_t DCM_Fault_Flag[DCModuleNum];
uint8_t DCM_Pdet_Flag[DCModuleNum];
uint8_t DCM_Fault_PinValue_Cache[DCModuleNum];
uint8_t DCM_Pdet_PinValue_Cache[DCModuleNum];
DCModule_State_t DCM_State[DCModuleNum];

float DCM_OUTCurr_Max       = 20.0; // 输出最大电流
float DCM_OUTCurr_Min       = 0.8;  // 输出最小电流
float DCM_OUTCurr_Limit     = 0.3;  // 判断有无电流的最小值

float _KFP_Last_out = 0; // 用于卡尔曼滤波
KFP KFP_DCM1 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM2 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM3 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM4 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};
KFP KFP_DCM5 = {0.0, 1, 0, 0.0, 0.0, 0.01, 4.5};

void dcm_init(void)
{
    ADC_CONV_COMPLETE_FLAG = 0;

    memset(DCM_Fault_PinValue_Cache, 0, sizeof(DCM_Fault_PinValue_Cache));
    memset(DCM_Pdet_PinValue_Cache, 0, sizeof(DCM_Pdet_PinValue_Cache));
    memset(DCM_SetOutCurr_Buffer, 0, sizeof(DCM_SetOutCurr_Buffer));

    /*复位所有模拟输出通道*/
    for (int i = 0; i < DCModuleNum; i++)
        set_analogout(i, 0);

    /*启动所有模拟输出通道*/
    for (int i = 0; i < DCModuleNum; i++)
        start_analogout(i);

    /*启动所有ADC通道*/
    for (int i = 0; i < DCModuleNum; i++)
        start_analogin(i);
    // start_analogin(DCModule_2);
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
//    LOG("dcm_read_enable_value : %d\r\n", dcm_read_enable_value(ch));
//    LOG("dcm_set_outcurrent channel %d, curr : %f\r\n", ch, curr);

    float volt = curr * DCM_Gain_Multi / DCM_SetCurr_Gain;
//    LOG("volt : %f\r\n", volt);
    uint32_t data = (uint32_t)(volt * DCM_DAC_MAX * DCM_Vref_Multi / DCM_Vref);
    set_analogout(ch, data);
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
    float currvolt, currkfp;
    uint32_t c_u32;
    switch (ch)
    {
    case DCModule_1:
        c_u32 = DCM_AnalogIn_Buffer.DCM1_ANALOGIN[DCM1_OUTCurr];
        currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
        currkfp = kalmanFilter(&KFP_DCM1, currvolt);
        currkfp = currkfp * DCM_OUTCurr_Gain / (float)DCM_Gain_Multi;
        break;

    case DCModule_2:
        c_u32 = DCM_AnalogIn_Buffer.DCM2_ANALOGIN[DCM2_OUTCurr];
        currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
        currkfp = kalmanFilter(&KFP_DCM2, currvolt);
        currkfp = currkfp * DCM_OUTCurr_Gain / (float)DCM_Gain_Multi;
        break;

    case DCModule_3:
        c_u32 = DCM_AnalogIn_Buffer.DCM3_ANALOGIN[DCM3_OUTCurr];
        currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
        currkfp = kalmanFilter(&KFP_DCM3, currvolt);
        currkfp = currkfp * DCM_OUTCurr_Gain / (float)DCM_Gain_Multi;
        break;

    case DCModule_4:
        c_u32 = DCM_AnalogIn_Buffer.DCM4_ANALOGIN[DCM4_OUTCurr];
        currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
        currkfp = kalmanFilter(&KFP_DCM4, currvolt);
        currkfp = currkfp * DCM_OUTCurr_Gain / (float)DCM_Gain_Multi;
        break;

    case DCModule_5:
        c_u32 = DCM_AnalogIn_Buffer.DCM5_ANALOGIN[DCM5_OUTCurr];
        currvolt = (float)(c_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
        currkfp = kalmanFilter(&KFP_DCM5, currvolt);
        //    LOG("curr volt : %f\r\n", currkfp);
        currkfp = currkfp * DCM_OUTCurr_Gain / (float)DCM_Gain_Multi;
        //currkfp = kalmanFilter(&KFP_DCM5, curr);
        break;

    default:
        LOG_error("get outcurrent error!\r\n");
        return NAN;
    }

    return currkfp;
}

/**
 * @brief 获取某一恒流模块的输出电压
 *
 * @param ch 指向第几个恒流模块
 * @return float 返回当前恒流模块的输出电压
 */
float dcm_get_outvoltage(uint8_t ch)
{
    float volt;
    uint32_t v_u32;
    switch (ch)
    {
    case DCModule_1:
        v_u32 = DCM_AnalogIn_Buffer.DCM1_ANALOGIN[DCM1_OUTVolt];
        break;

    case DCModule_2:
        v_u32 = DCM_AnalogIn_Buffer.DCM2_ANALOGIN[DCM2_OUTVolt];
        break;

    case DCModule_3:
        v_u32 = DCM_AnalogIn_Buffer.DCM3_ANALOGIN[DCM3_OUTVolt];
        break;

    case DCModule_4:
        v_u32 = DCM_AnalogIn_Buffer.DCM4_ANALOGIN[DCM4_OUTVolt];
        break;

    case DCModule_5:
        v_u32 = DCM_AnalogIn_Buffer.DCM5_ANALOGIN[DCM5_OUTVolt];
        break;

    default:
        LOG_error("get OUTVoltage error!\r\n");
        return NAN;
    }

    volt = (float)(v_u32 * DCM_Vref / (float)DCM_Vref_Multi / DCM_DAC_MAX);
    //    LOG("volt volt : %f\r\n", volt);
    volt = volt * DCM_OUTVolt_Gain / (float)DCM_Gain_Multi;
    //    LOG("v_u32 : %lx, volt : %f\r\n", v_u32, volt);
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

    /* 2. 先判断是否有输入电压: 30201 */
    if (dcm_get_pdet(ch) == DCM_No_INVolt)
    {
        DCM_State[ch] = DCMState_IN_NoVolt;
        return DCM_State[ch];
    }

    /* 3. 判断输出是否过载: 30101 */
    if ((DCM_SetOutCurr_Buffer[ch] != 0) && (dcm_get_outcurrent(ch) - DCM_SetOutCurr_Buffer[ch] > 0.15)) //(DCM_SetOutCurr_Buffer[ch] + 1.0)))
    {
        DCM_State[ch] = DCMState_OUT_OverCurr;
        return DCM_State[ch];
    }

    /* 4. 判断输出是否断路：30301；断路有几种情况：未接负载、输出线断开、接触不良 */
    if ((dcm_read_enable_value(ch) == 1) && (dcm_get_outcurrent(ch) > DCM_OUTCurr_Limit) && (dcm_get_outcurrent(ch) < DCM_OUTCurr_Min))
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
    //    HAL_OPAMP_Start();
    switch (ch)
    {
    case DCModule_1:
        HAL_DAC_Start(&hdac3, DAC_CHANNEL_1);
        HAL_OPAMP_Start(&hopamp1);
        break;

    case DCModule_2:
        HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
        break;

    case DCModule_3:
        HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
        break;

    case DCModule_4:
        HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
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
        HAL_DAC_Stop(&hdac3, DAC_CHANNEL_1);
        break;

    case DCModule_2:
        HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
        break;

    case DCModule_3:
        HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
        break;

    case DCModule_4:
        HAL_DAC_Stop(&hdac2, DAC_CHANNEL_1);
        break;

    case DCModule_5:
        HAL_DAC_Stop(&hdac4, DAC_CHANNEL_1);
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
        HAL_DAC_SetValue(&hdac3, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_3:
        //LOG("value 3 : %ld\r\n", value);
        HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_4:
        HAL_DAC_SetValue(&hdac4, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    case DCModule_5:
        HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
        break;
    default:
        LOG_error("analog out channel error!\r\n");
        break;
    }
}

void start_analogin(uint8_t ch)
{
    LOG("start_analogin....%d\r\n", ch);
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

/**
 *卡尔曼滤波器
    float LastP;//上次估算协方差 初始化值为0.02
    float Now_P;//当前估算协方差 初始化值为1
    float out;//卡尔曼滤波器输出 初始化值为0
    float Kg;//卡尔曼增益 初始化值为0
    float Q;//过程噪声协方差 初始化值为0.0001
    float R;//观测噪声协方差 初始化值为5
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
#if 1
float kalmanFilter(KFP *kfp, float input)
{
    if (abs(_KFP_Last_out - input) >= 5)
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