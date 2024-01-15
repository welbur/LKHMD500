/* include ------------------------------------------------------------------ */
#include "eos_event.h"
#include "eventos.h"
#include "event_def.h"
#include "EtherCatPDO.h"
#include "DCMCtrl.h"
#include "ee.h"
#include "eeConfig.h"
#include "math.h"

//#define USE_Event_UserTest

extern UART_HandleTypeDef huart3;

/* 来自DCMCtrl的参数 */
extern float Get_OUTCurr_Volt[DCModuleNum];
extern float Get_SetCurr_Volt[DCModuleNum]; 

extern uint8_t DCM_SetCurr_Now_Cnt[DCModuleNum];


#ifdef USE_AdjustFunc
extern DCM_Adjust_FromPLC_t DCM_Adjust_Flag[DCModuleNum];        //PLC发送过来的输出电流调整标记 0:默认状态； 1:接收到校准的数值完成； 2:保存校准的数值到FLASH完成
#endif

/* 用于校准的参数 */
const uint16_t ee_StartFrame = ee_use_StartFrame;
uint8_t DCM_AdjustOUTCurr_Value[DCModuleNum] = {0, 0, 0, 0, 0};         //OUTData_DCModule_AdjustOUTCurr
uint16_t DCM_SetCurr_Gain_Cache[DCModuleNum][DCM_Gain_Num];
uint16_t DCM_OutCurr_Gain_Cache[DCModuleNum][DCM_Gain_Num];


/*用于跟PC通讯的参数*/
uint8_t rDataFromPC[5];
uint8_t wDataToPC[5];

OUTDATAValueHandle OUTDataValue_Cache;
uint16_t INDataValue_DCMState_Cache[DCModuleNum];
float INDataValue_DCModule_Curr_Cache[DCModuleNum];

uint16_t DCMOUTCurr_Cnt = 0;

float _DCM_OUT_Volt[DCModuleNum];           //每个恒流模块的输出电压
uint8_t _DCM_INVolt_EN[DCModuleNum];        //检测是否有输入电压
uint8_t _DCM_Fault[DCModuleNum];            //每个恒流模块的错误状态

DCM_CalibrateState_t _DCM_Calibrate_Done = DCM_CalibrateState_Wait;            //校准完成标志


const char *HMD500_FirmWare = "V0.3.0";
const char* Enable_DCM_Log[]                = {"Disable!", "Enable!"};   
//char* DCModule_State_Log[]            = {"Working Fine!", "OUTPUT Over Current!", "No INPUT Voltage!", };

/* 用在主状态机的参数 */
typedef struct eos_sm_hmd500_tag {
    eos_sm_t super;
    eos_u8_t status;
    DCModule_State_t DCMstatus;
    LAN9255State_t ECATstatus;
    uint8_t TransData_Status;
    uint8_t reTry_Idx;
} eos_sm_hmd500_t;

eos_sm_hmd500_t sm_HMD500;

/* 用在LAN9255初始化的状态机中 */
typedef struct eos_sm_ecat_tag {
    eos_sm_t super;
    eos_u8_t status;
    uint8_t repeatCnt;
    uint8_t timeoout_idx;
    uint8_t cmd_state;
    uint16_t previewEvent;
    uint8_t LAN9255_InitDone;
} eos_sm_ecat_t;

eos_sm_ecat_t sm_EtherCatPDO;

enum {
    cmd_first_send,
    cmd_rsp_wait,
    cmd_rsp_ok,
    cmd_rsp_err
};

//EMessageHandler_t _eMsgH;
LAN9255State_t L9255State = LAN9255State_WaitInit; 


#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */
typedef struct eos_sm_led_tag {
    eos_sm_t super;
    eos_u8_t status;
    uint8_t ee_use_sf[2];  //用于检测保存参数的flash页面 是否已经保存过参数
} eos_sm_led_t;

eos_sm_led_t sm_led;

uint8_t status_button = 0;  // = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);

/* static state function ---------------------------------------------------- */
static eos_ret_t workled_state_init(eos_sm_led_t * const me, eos_event_t const * const e);
static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e);
static eos_ret_t dcm_calibrate(eos_sm_led_t * const me, eos_event_t const * const e);


/* api ---------------------------------------------------- */
void eos_sm_led_init(void)
{
    LOG("eos sm led init \r\n");
    eos_sm_init(&sm_led.super, 3, EOS_NULL);
    eos_sm_start(&sm_led.super, EOS_STATE_CAST(workled_state_init));
    
    sm_led.status = 0;
}



/* static state function ---------------------------------------------------- */
static eos_ret_t workled_state_init(eos_sm_led_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_WorkLed_Blink);
//    EOS_EVENT_SUB(Event_Button);
    EOS_EVENT_SUB(Event_PrintLOG);
    EOS_EVENT_SUB(Event_UserTest);
    EOS_EVENT_SUB(Event_DCM_Calibrate);
    EOS_EVENT_SUB(Event_DCM_ReadDataFromPC_Recv);
    EOS_EVENT_SUB(Event_DCM_SendDataToPC_Send);
#endif
    eos_event_pub_period(Event_DCM_ReadDataFromPC_Recv, PeriodTime_5ms);
    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_50ms);    
//    eos_event_pub_topic(Event_DCM_Calibrate);
#ifdef USE_Event_UserTest
    //eos_event_pub_period(Event_UserTest, 20);
//    eos_event_pub_topic(Event_UserTest);
#endif
    LOG("workled state init \r\n");
    return EOS_TRAN(dcm_calibrate);    //return EOS_TRAN(workled_state_on);
}


/********************************************************************************
 * @brief 1、工作状态指示灯：led event sm
 * 
 ********************************************************************************/
static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            LOG("eos workled state -----Event_Enter \r\n");
//            eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("eos state on-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_PrintLOG:
            LOG("............................      HMD500 Key Para      ......................\r\n");
            LOG("Firmware : %s\r\n", HMD500_FirmWare);
            
            LOG("###   Data Read From PLC   ###\r\n");
            //LOG("Set [DCM1~DCM5] OUT Current : ");
            for (int i = 0; i < DCModuleNum; i++)
            {
                LOG(">>>   DCM[%d] is %s, ", i+1, Enable_DCM_Log[OUTDataValue_Cache.OUTData_DCModule_Enable[i]]);
                LOG("and Set OUT Current : %f", OUTDataValue_Cache.OUTData_DCModule_SetCurr[i]);
                LOG("( Adjust_Curr = %d ; Ictrl_v = %f )\r\n", DCM_AdjustOUTCurr_Value[i], Get_SetCurr_Volt[i]);
            }

            LOG("###   HMD500 INFO   ###\r\n");
            for (int i = 0; i < DCModuleNum; i++)
            {   
                LOG("#####   DCM[%d]   #####\r\n", i+1);
                LOG("               <<<   Status : %d ------ %s\r\n", INDataValue_DCMState_Cache[i], DCModule_State_Log(INDataValue_DCMState_Cache[i]));
                LOG("               <<<   Actual OUTPUT Current : %f ( OUTCurr_v = %f )\r\n", INDataValue.INData_DCModule_Curr[i], Get_OUTCurr_Volt[i]);     //INDataValue_DCModule_Curr_Cache[i]);
//                LOG("                                                  %d  %d\r\n", DCM_SetCurr_Now_Cnt[i], DCM_OUTCurr_Gain[DCM_SetCurr_Now_Cnt[i]]);
                LOG("               ^^^   Actual OUTPUT Voltage : %f\r\n", dcm_get_outvoltage(i));
                LOG("               ^^^   Fault Signal : %d\r\n", dcm_get_dcmfault(i));
            }

            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        case Event_UserTest:
            ee_testWriteandRead();
            return EOS_Ret_Handled;

        case Event_WorkLed_Blink:
            //LOG("eos state on-----Event_Time_500ms");    
            WorkLed_TogglePin;

#ifndef USE_Event_UserTest
            me->status += 1;
            if (me->status >= 3) 
            {
                me->status = 0;
                eos_event_pub_topic(Event_PrintLOG);
            }
#endif
            return EOS_Ret_Handled;     //EOS_TRAN(state_off);

        case Event_Button:
            //eos_event_time_cancel(Event_Button);
            LOG("eos state on-----Event_Button %d\r\n", *(uint8_t *)e->data);
            return EOS_Ret_Handled; 

        default:
            //LOG("eos state on-----default \r\n");
            return EOS_SUPER(eos_state_top);
    }
}


/********************************************************************************
 * @brief  2、通过PC端工具校准5个恒流模块的参数  
 * 
 ********************************************************************************/
static eos_ret_t dcm_calibrate(eos_sm_led_t * const me, eos_event_t const * const e)
{
    uint16_t dcm_get_oc;
    uint16_t eeuseSF = ee_use_StartFrame;
    //uint8_t *edata = (uint8_t *)e->data; 
    float get_outcurr[DCModuleNum] = {0, 0, 0, 0, 0};
    switch (e->topic) {
        case Event_Enter:
            LOG("dcm calibrate -----Event_Enter \r\n");
            me->status = 0;
            
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("dcm calibrate -----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_DCM_ReadDataFromPC_Recv:
            /* 循环读取每个通道的状态和输出电流值 */
            for (int i = 0; i < DCModuleNum; i++)
                get_outcurr[i] = dcm_get_outcurrent(i);

            /* 等待接收PC发送过来的指令 */
	        if (HAL_UART_Receive_IT(&huart3, rDataFromPC, 5) == HAL_OK)
            {
                uint8_t dcm_num;
                uint8_t dcm_calibra_num;
                float dcm_value = 0;
                uint16_t dcm_outcurr_volt = 0;
                uint8_t rDataRSPToPC[5] = {0xAC, 0xED, 0x00, 0xED, 0xAC};
                uint8_t TransPC_SetCurrVolt_Flag = 0xAB;
                uint8_t TransPC_OutCurrVolt_Flag = 0xCD;
                uint8_t TransPC_OutCurr_Flag     = 0xCF;
                /* {包头2字节， 预留1字节， 数据2字节} */
                /* 1、先检测是否接收到进入校准模式的指令 */
                LOG("rdata : %X, %X, ", rDataFromPC[0], rDataFromPC[1]);
                LOG(" %X, %X, %X\r\n", rDataFromPC[2], rDataFromPC[3], rDataFromPC[4]);
                if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == 0x00)
                {
                    _DCM_Calibrate_Done = DCM_CalibrateState_Doing;
                    LOG("DCM_CalibrateState_Doing\r\n");
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);
                    return EOS_Ret_Handled;
                }
                /* 2、等待设置输出电流指令 rDataFromPC[1]:设置输出电流标志位, rDataFromPC[0]:输出电流值 */
                else if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == TransPC_SetCurrVolt_Flag && _DCM_Calibrate_Done == DCM_CalibrateState_Doing)
                {
                    dcm_num = rDataFromPC[3] - 1;
                    if (rDataFromPC[4] == 0)
                        close_dcm_outcurr(dcm_num);
                    else
                    {
                        dcm_value = rDataFromPC[4] / 10.0;
                        open_dcm_outcurr(dcm_num, dcm_value);
                        dcm_outcurr_volt = (uint16_t)(round(Get_SetCurr_Volt[dcm_num] * 10000));
                    }
                    
                    LOG("dcm_num : %d\r\n", dcm_num);
                    LOG("outcurr value : %f\r\n", dcm_value);
                    LOG("outcurr volt : %d\r\n", dcm_outcurr_volt);
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    wDataToPC[2] = wDataToPC[2] + 0xE1;   //E1
                    memcpy(wDataToPC + 3, &dcm_outcurr_volt, 2);
                    LOG("wDataToPC : %X%X, ", wDataToPC[0], wDataToPC[1]);
                    LOG("%X, ", wDataToPC[2]);
                    LOG("%X%X\r\n", wDataToPC[3], wDataToPC[4]);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);
                    return EOS_Ret_Handled;
                }
                /* 3、获取当前某一通道的输出电流值对应的电压值 */
                else if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == TransPC_OutCurrVolt_Flag)
                {
                    dcm_num = rDataFromPC[3] - 1;
                    /* 发送"输出电流反馈对应的电压值"给PC */
                    LOG("dcm_channel : %d\r\n", dcm_num);
                    LOG("dcm get outcurr_volt : %f\r\n", Get_OUTCurr_Volt[dcm_num]);
                    dcm_get_oc = (uint16_t)(round(Get_OUTCurr_Volt[dcm_num] * 10000));
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    wDataToPC[2] = wDataToPC[2] + 0xE2;   //E2
                    memcpy(wDataToPC + 3, &dcm_get_oc, 2);
                    
                    LOG("wDataToPC : %X%X, ", wDataToPC[0], wDataToPC[1]);
                    LOG("%X, ", wDataToPC[2]);
                    LOG("%X%X\r\n", wDataToPC[3], wDataToPC[4]);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);

                    _DCM_Calibrate_Done = DCM_CalibrateState_SavePara;
                    close_dcm_outcurr(dcm_num);
                    return EOS_Ret_Handled;
                }
                /* 4、获取当前某一通道的输出电流值 用于检验参数是否正确 */
                else if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == TransPC_OutCurr_Flag)    //0xCF
                {
                    dcm_num = rDataFromPC[3] - 1;
                    /* 发送输出电流反馈给PC */
                    LOG("dcm_channel : %d\r\n", dcm_num);
                    LOG("dcm get outcurr_volt : %f\r\n", Get_OUTCurr_Volt[dcm_num]);
                    dcm_get_oc = (uint16_t)(round(get_outcurr[dcm_num] * 1000));
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    wDataToPC[2] = wDataToPC[2] + 0xE3;   //E3
                    memcpy(wDataToPC + 3, &dcm_get_oc, 2);
                    
                    LOG("wDataToPC : %X%X, ", wDataToPC[0], wDataToPC[1]);
                    LOG("%X, ", wDataToPC[2]);
                    LOG("%X%X\r\n", wDataToPC[3], wDataToPC[4]);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);

                    close_dcm_outcurr(dcm_num);
                    return EOS_Ret_Handled;
                }
                /* 5、保存校准参数 SetCurr_Gain */
                else if (rDataFromPC[0] == TransPC_SetCurrVolt_Flag && _DCM_Calibrate_Done == DCM_CalibrateState_SavePara)    //0xAB
                {
                    /* 保险起见，把对应通道关闭 */
                    dcm_num = rDataFromPC[1] - 1;
                    dcm_calibra_num = rDataFromPC[2] - 1;
                    //close_dcm_outcurr(dcm_num);
                    
                    /* 发送应答给PC */
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);

                    /* 保存参数 */
                    DCM_SetCurr_Gain_Cache[dcm_num][dcm_calibra_num] = rDataFromPC[3] << 8 | rDataFromPC[4];
                    LOG("5...DCM_SetCurr_Gain_Cache[%d][%d] : %d\r\n", dcm_num, dcm_calibra_num, DCM_SetCurr_Gain_Cache[dcm_num][dcm_calibra_num]);
                }
                /* 6、保存校准参数 OutCurr_Gain */              //0xCD
                else if (rDataFromPC[0] == TransPC_OutCurrVolt_Flag && _DCM_Calibrate_Done == DCM_CalibrateState_SavePara)
                {
                    /* 保险起见，把对应通道关闭 */
                    dcm_num = rDataFromPC[1] - 1;
                    dcm_calibra_num = rDataFromPC[2] - 1;
                    //close_dcm_outcurr(dcm_num);

                    /* 发送应答给PC */
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);
                    
                    /* 保存参数 */
                    DCM_OutCurr_Gain_Cache[dcm_num][dcm_calibra_num] = rDataFromPC[3] << 8 | rDataFromPC[4];
                    LOG("~~~~DCM_OutCurr_Gain_Cache[%d][%d] : %d\r\n", dcm_num, dcm_calibra_num, DCM_OutCurr_Gain_Cache[dcm_num][dcm_calibra_num]);
                }
                /* 7、结束校准，去保存参数到FLASH的127页面 */
                else if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == 0xEE)
                {
                    LOG("goto Save para to FLASH\r\n");
                    uint8_t savetoFlash_buf[500];
                    //uint8_t outcurrgain_savetoFlash_buf[DCModuleNum * DCM_Gain_Num];
                    memcpy(savetoFlash_buf, &eeuseSF, 2);   //先添加一个头部标记
                    for (int i = 0; i < DCModuleNum; i++)
                    {
                        for (int j = 0; j < DCM_Gain_Num; j++)
                        {
                            memcpy(savetoFlash_buf + _EE_USE_SetCurrGain_START_ADDR + i * DCM_Gain_Num * 2 + j * 2, &DCM_SetCurr_Gain_Cache[i][j], 2);
                            memcpy(savetoFlash_buf + _EE_USE_OUTCurrGain_START_ADDR + i * DCM_Gain_Num * 2 + j * 2, &DCM_OutCurr_Gain_Cache[i][j], 2);
                        }
                    }
                    /* 去保存"DCM_SetCurr_Gain"到flash的127页面 */
                    if (ee_SavetoFLASH(0, sizeof(savetoFlash_buf), savetoFlash_buf) == false)
                        return EOS_Ret_Handled;

                    //if (ee_SavetoFLASH(_EE_USE_OUTCurrGain_START_ADDR, DCModuleNum * DCM_Gain_Num, outcurrgain_savetoFlash_buf) == false)
                    //    return EOS_Ret_Handled;
                    //ee_write(0, sizeof(me->ee_use_sf), me->ee_use_sf);
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    wDataToPC[2] = 0xEE;
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);
                    dcm_read_data_fromFLASH();
                    //_DCM_Calibrate_Done = DCM_CalibrateState_Doing;
                    //LOG("_DCM_Calibrate_Done : %d\r\n", _DCM_Calibrate_Done);
                }
                /* 8、 退出校准dcm的状态机 */
                else if (rDataFromPC[0] == 0xCA && rDataFromPC[1] == 0xDE && rDataFromPC[2] == 0xFF)
                {
                    LOG("end calibration dcm\r\n");
                    /* 保险起见，把所有通道都关闭一遍 */
                    for (int i = 0; i < DCModuleNum; i++)
                        close_dcm_outcurr(i);
                    
                    /* 退出校准dcm的状态机 */
                    eos_event_time_cancel(Event_WorkLed_Blink);
                    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_1000ms);
                    _DCM_Calibrate_Done = DCM_CalibrateState_Done;
                    sm_EtherCatPDO.status = 1;
                    memcpy(wDataToPC, rDataRSPToPC, 5);
                    wDataToPC[2] = 0xFF;
                    HAL_UART_Transmit_IT(&huart3, wDataToPC, 5);
                    return EOS_TRAN(workled_state_on);
                }
            }

            return EOS_Ret_Handled;
#if 0
        case Event_DCM_SendDataToPC_Send:
            /* 3、发送输出电流反馈给PC */
            LOG("dcm_channel : %d\r\n", edata[0]);
            LOG("dcm get outcurr : %f\r\n", dcm_get_outcurrent(edata[0]));
            dcm_get_oc = (uint16_t)(dcm_get_outcurrent(edata[0]) * 100) + (0x1000 * (edata[0] + 1));
            LOG(" dcm_get_oc = %x\r\n", dcm_get_oc);
            memcpy(wDataToPC, &dcm_get_oc, 2);
            LOG("wDataToPC : %x, %x\r\n", wDataToPC[0], wDataToPC[1]);
            HAL_UART_Transmit_IT(&huart3, wDataToPC, 2);
            return EOS_Ret_Handled;
#endif
        case Event_WorkLed_Blink:
            //LOG("dcm calibrate workled blink -----Event_Time_100ms");    
            WorkLed_TogglePin;

            /* 如果正在校准中，就退出 */
            if (_DCM_Calibrate_Done == DCM_CalibrateState_Doing || _DCM_Calibrate_Done == DCM_CalibrateState_SavePara)
                return EOS_Ret_Handled;

            me->status++;
            if (me->status >= 20 * 6)      //设置等待6s，然后判断是否有参数保存到flash 
            {
                LOG("time out me->state = 100\r\n");
                /* 读取保存参数的页面中的第1、2字节 */
                if (ee_read(0, sizeof(me->ee_use_sf), me->ee_use_sf) == false)
                {
                    LOG_error("read eeprom data faild!\r\n");
                    return EOS_Ret_Handled;
                }
                LOG("read eeprom data 1&2 0x%X%X\r\n", me->ee_use_sf[0], me->ee_use_sf[1]);
                /* 如果校准完成，标记校准完成，并进入正常闪灯模式 */
                if (memcmp(me->ee_use_sf, &eeuseSF, sizeof(me->ee_use_sf)) == 0)
                {
                    eos_event_time_cancel(Event_DCM_ReadDataFromPC_Recv);
                    eos_event_time_cancel(Event_WorkLed_Blink);
                    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_1000ms);
                    _DCM_Calibrate_Done = DCM_CalibrateState_Done;
                    sm_EtherCatPDO.status = 1;
                    return EOS_TRAN(workled_state_on);
                }
                //if (me->status >= 100)          
                me->status = 0;
            }
            return EOS_Ret_Handled;     

        default:
            //LOG("dcm calibration-----default \r\n");
            return EOS_SUPER(eos_state_top);

    }
}

#endif


/********************************************************************************
 * @brief  3、跟LAN9255通讯用的相关状态机  
 * 
 ********************************************************************************/
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */


/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e);
static eos_ret_t EtherCatPDO_state_WaitInit(eos_sm_hmd500_t * const me, eos_event_t const * const e);
static eos_ret_t EtherCatPDO_state_InitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e);
//static eos_ret_t HMD500_state_Process(eos_sm_hmd500_t * const me, eos_event_t const * const e);

//static eos_ret_t EtherCatPDO_state_DataTransferWithPLC(eos_sm_ecat_t * const me, eos_event_t const * const e);


/* api ---------------------------------------------------- */
void eos_sm_EtherCatPDO_init(void)
{
    LOG("eos sm EtherCatPDO init \r\n");
    eos_sm_init(&sm_EtherCatPDO.super, 1, EOS_NULL);
    eos_sm_start(&sm_EtherCatPDO.super, EOS_STATE_CAST(EtherCatPDO_state_init));
   
    sm_EtherCatPDO.status = 0;  //cmd_first_send; //cmd_rsp_ok;
    sm_EtherCatPDO.cmd_state = cmd_first_send;
    
}

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    /* 用于LAN9255初始化的事件 */
    EOS_EVENT_SUB(Event_ECAT_WaitInit);
    EOS_EVENT_SUB(Event_ECAT_Start_InitLAN9255);
    EOS_EVENT_SUB(Event_ECAT_END_InitLAN9255);
    EOS_EVENT_SUB(Event_ECAT_RestartLAN9255S_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_RestartLAN9255S_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_AddParaList_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_AddParaList_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_SetMsgTimeout_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_SetMsgTimeout_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_StartLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_StartLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_SendCMD_TIMEOUT);

    /* 用于主状态机的事件：1、读取恒流模块DCM的状态和电流值；2、控制恒流模块的输出；3、读取PLC发送过来的指令；4、发送数据给PLC */
 //   EOS_EVENT_SUB(Event_HMD_Start_MainFSM);
#endif

    //eos_event_pub_period(Event_ECAT_SendCMD_TIMEOUT, PeriodTime_2000ms);
    //eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);
    eos_event_pub_period(Event_ECAT_WaitInit, PeriodTime_1000ms);

    LOG("Event_EtherCatPDO state init \r\n");
    return EOS_TRAN(EtherCatPDO_state_WaitInit);
}

static eos_ret_t EtherCatPDO_state_WaitInit(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            //LOG_info("EtherCatPDO_state_WaitInit ----------Event Enter\r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("EtherCatPDO_state_WaitInit ---------Event exit\r\n");
            return EOS_Ret_Handled;

        case Event_ECAT_WaitInit:
            //LOG("EtherCatPDO_state_WaitInit ---------Event Event_ECAT_WaitInit.....%d\r\n", me->status);
            if (me->status == 1)
            {
                eos_event_time_cancel(Event_ECAT_WaitInit);
                eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);
                return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            }
            return EOS_Ret_Handled;

        default:
            return EOS_SUPER(eos_state_top);
    }            
}

/**
 * @brief 
 * 
 * @param me 
 * @param e 
 * @return eos_ret_t 
 */
static eos_ret_t EtherCatPDO_state_InitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
    byte *edata = (byte *)e->data; 
    switch (e->topic) {
        case Event_Enter:
            //LOG_info("EtherCatPDO_state_InitLAN9255\r\n");
            me->repeatCnt = 0;
            me->timeoout_idx = 0;
            me->LAN9255_InitDone = 0;    //未初始化完成
            
            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("EtherCatPDO_state_InitLAN9255-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_ECAT_Start_InitLAN9255:
            LOG("Event_ECAT_Start_InitLAN9255 \r\n");
            eos_event_pub_period(Event_ECAT_SendCMD_TIMEOUT, PeriodTime_2000ms);
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_1000ms);
            return EOS_Ret_Handled;
        
        case Event_ECAT_END_InitLAN9255:
            LOG("Event_ECAT_END_InitLAN9255 \r\n");
            me->status = 0;     //关闭初始化lan9255的状态机，转到waitinit状态机
            eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
            eos_event_time_cancel(Event_ECAT_SendCMD_TIMEOUT);
    
            sm_HMD500.status = 1;      //启动主状态机

            eos_event_pub_period(Event_ECAT_WaitInit, PeriodTime_1000ms);   //启动等待事件
            return EOS_TRAN(EtherCatPDO_state_WaitInit);  //转到waitinit状态机

        /*********************      刚上电的时候，先读取LAN9255状态      *********************/
        case Event_ECAT_ReadLAN9255State_SendCMD:
            //LOG("Event_ECAT_ReadLAN9255State_SendCMD \r\n");
            me->cmd_state = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_ReadLAN9255State_SendCMD;
            ReadLAN9255State_SendCMD();

            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_RecvRSP:
            //me->cmd_state = cmd_rsp_ok;
            //byte *edata = (byte *)e->data;    //(byte *)&e->data[0];
            //LOG("e->data : %d\r\n", edata[0]);
            LOG_info("ECAT-----MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[edata[0]]);
            //if ((byte *)&e->data[0] == s_WaitInit)
            if (edata[0] == s_WaitInit)
            {
                me->cmd_state = cmd_rsp_ok;
                //me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                //eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, PeriodTime_500ms);
                eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
            }
            else
            {
                me->cmd_state = cmd_rsp_err;
                me->repeatCnt = MaxRepeatCnt;
            }
            return EOS_Ret_Handled;        

        /*********************      添加属性表      *********************/
        case Event_ECAT_AddParaList_SendCMD:
            //me->repeatCnt += 1;
            me->cmd_state = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_AddParaList_SendCMD;
            AddParaList_SendCMD(ParaList_Num);
            return EOS_Ret_Handled;       

        case Event_ECAT_AddParaList_RecvRSP:
            LOG_info("MsgType is 0x02 : Add ParaList!......result : Add [ %s ] to paraList %s\r\n", OBJ_H[ParaList_Num].ObjName, Result_Log[edata[2]]);
            if (edata[2] == 1)
            {
                me->cmd_state = cmd_rsp_ok;
                ParaList_Num++;
                if (ParaList_Num >= ObjList_Num)
                {
                    ParaList_Num = 0;
                    eos_event_pub_topic(Event_ECAT_SetMsgTimeout_SendCMD);        
                }
                else 
                {
                    eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
                }
            }
            else
            {
                me->cmd_state = cmd_rsp_err;
            }
            return EOS_Ret_Handled;
        
        /*********************      设置关键数据的超时时间      *********************/
        case Event_ECAT_SetMsgTimeout_SendCMD:
            me->cmd_state = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_SetMsgTimeout_SendCMD;
            setMsgCheckTimeout_SendCMD();

            return EOS_Ret_Handled;         

        case Event_ECAT_SetMsgTimeout_RecvRSP:
            LOG_info("MsgType is 0x0A : Set Msg Time Out!......result : ");
            if (KeyOUTData_TimeOut == word(edata[1], edata[0]))
            {
                me->cmd_state = cmd_rsp_ok;
                LOG("Success!\r\n");
                eos_event_pub_topic(Event_ECAT_StartLAN9255_SendCMD);
            }
            else
            {
                me->cmd_state = cmd_rsp_err;
                LOG("Fail!\r\n");
            }
            return EOS_Ret_Handled;

        /*********************      启动LAN9255      *********************/
        case Event_ECAT_StartLAN9255_SendCMD:
            me->cmd_state = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_StartLAN9255_SendCMD;
            StartDevice_SendCMD();     
            return EOS_Ret_Handled;

        case Event_ECAT_StartLAN9255_RecvRSP:
            LOG_info("MsgType is 0x03 : Start Device!......result : %s\r\n", Result_Log[edata[0]]);
            if (edata[0] == 1) 
            {
                me->LAN9255_InitDone = 1;   //LAN9255初始化完成
                me->cmd_state = cmd_rsp_ok;
                eos_event_pub_topic(Event_ECAT_END_InitLAN9255);    //LAN9255初始化完成，结束InitLAN9255状态机
            }
            else 
                me->cmd_state = cmd_rsp_err;
            return EOS_Ret_Handled;

        /*********************      用于处理通讯过程中，未接收到9255发送过来的响应，重新发送一条指令给9255      *********************/
        /*                          多次发送指令都失败的话，就重启9255                                                        */
        /*****************************************************************************************************************/
        case Event_ECAT_SendCMD_TIMEOUT:
            //LOG_info("Event_ECAT_SendCMD_TIMEOUT\r\n");
            /* 没有校准完成，就退出 */
            if (_DCM_Calibrate_Done != DCM_CalibrateState_Done)
                return EOS_Ret_Handled;

            if (me->LAN9255_InitDone == 1)
            {
                LOG("event cat sendcmd timeout\r\n");
                return EOS_Ret_Handled;
            }
            
            if (me->cmd_state != cmd_rsp_ok)
            {
                me->timeoout_idx++;
                if (me->timeoout_idx >= 2)
                {
                    me->timeoout_idx = 0;
                    me->repeatCnt += 1;
                    //LOG("previewevent : %d\r\n", me->previewEvent);
                    eos_event_pub_topic(me->previewEvent);
                }
            }

            if (Read_LAN9255_RST() == 0)
                LAN9255_RST(1);

            /* 大于重试次数后，重启9255 */
            if (me->repeatCnt > MaxRepeatCnt)
            {
                LOG("goto reStart LAN9255!\r\n");
                me->repeatCnt = 0;
                RestartLAN9255_SendCMD();       //发送重启指令，通知9255要重启
                LAN9255_RST(0);                 //复位信号
            } 
            return EOS_Ret_Handled;

        default:
            if (me->LAN9255_InitDone == 1)
                LOG("Event_ECAT_state_InitLAN9255 default\r\n");
            return EOS_SUPER(eos_state_top);
    }
}

#endif

/********************************************************************************
 * @brief  4、主状态机：读取恒流模块的各个参数，控制恒流模块的电流输出，EtherCat的数据收发
 * 从PLC读取数据
 * 往PLC发送数据
 * 输入：控制输出电流 DCM_Ictrl ; 使能恒流模块 DCM_EN ; 
 * 输出：读取输出电流 DCM_Iout ; 读取输出电压 DCM_Vout ; 读取恒流模块的状态 DCM_Fault
 * 
 ********************************************************************************/
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */


/* static state function ---------------------------------------------------- */
static eos_ret_t HMD500_state_init(eos_sm_hmd500_t * const me, eos_event_t const * const e);
static eos_ret_t HMD500_state_Process_Stop(eos_sm_hmd500_t * const me, eos_event_t const * const e);
static eos_ret_t HMD500_state_Process_Start(eos_sm_hmd500_t * const me, eos_event_t const * const e);

/* api ---------------------------------------------------- */
void eos_sm_HMD500_init(void)
{
    LOG("eos sm HMD500 init \r\n");
    eos_sm_init(&sm_HMD500.super, 0, EOS_NULL);
    eos_sm_start(&sm_HMD500.super, EOS_STATE_CAST(HMD500_state_init));
    sm_HMD500.status = 0;
}

/* static state function ---------------------------------------------------- */
static eos_ret_t HMD500_state_init(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_HMD_WaitStart);

    EOS_EVENT_SUB(Event_HMD_Start_MainFSM);
    EOS_EVENT_SUB(Event_HMD_End_MainFSM);

    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadLAN9255State_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_RecvRSP);

    EOS_EVENT_SUB(Event_DCM_ReadDCMState);
    EOS_EVENT_SUB(Event_DCM_Adjust);
    EOS_EVENT_SUB(Event_DCM_CtrlOUTCurr);
    EOS_EVENT_SUB(Event_DCM_Save_AdjustPara);
//    EOS_EVENT_SUB(Event_ECAT_StartTransData);
//    EOS_EVENT_SUB(Event_ECAT_CancelTransData);
#endif
    /* 先从FLASH中读取校准参数 */
    uint8_t eereaddata[30];
    ee_read(_EE_USE_AdjustCurr_START_ADDR, DCModuleNum * 4 + 1, eereaddata);
    if (eereaddata[0] == ADJ_Whole_Curr)
    {    
        LOG("-------------");
        for (int i = 0; i < DCModuleNum; i++)
        {
            memcpy(&(DCM_AdjustOUTCurr_Value[i]), eereaddata + 1 + i * 1, 1); 
            LOG("%d,  ", DCM_AdjustOUTCurr_Value[i]);
        }
        LOG("\r\n");
    }
    else
    {
        memset(DCM_AdjustOUTCurr_Value, 0, sizeof(DCM_AdjustOUTCurr_Value));
        LOG("~~~~~~%f\r\n", DCM_AdjustOUTCurr_Value[4]);
    }
    LOG("~~~eereaddata : %d,  %d,  ", DCM_AdjustOUTCurr_Value[0], DCM_AdjustOUTCurr_Value[1]);
    LOG("%d,  %d, ", DCM_AdjustOUTCurr_Value[2], DCM_AdjustOUTCurr_Value[3]);
    LOG("%d\r\n", DCM_AdjustOUTCurr_Value[4]);

    /* 去设置PLC发送过来的调整输出电流值 */
    for (int i = 0; i < DCModuleNum; i++)
        dcm_set_adjustcurr(i, DCM_AdjustOUTCurr_Value[i]);

    eos_event_pub_period(Event_HMD_WaitStart, PeriodTime_1000ms);//PeriodTime_100ms);
    LOG("HMD500 main state init \r\n");
    return EOS_TRAN(HMD500_state_Process_Stop);
//    return EOS_SUPER(eos_state_top);
}

static eos_ret_t HMD500_state_Process_Stop(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            //LOG_info("HMD500_state_Process_Stop ----------Event Enter\r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("HMD500_state_Process_Stop ---------Event exit\r\n");
            return EOS_Ret_Handled;

        case Event_HMD_WaitStart:
            //LOG("HMD500_state_Process_Stop ---------Event HMD_WaitStart\r\n");
            if (me->status == 1)
            {
                eos_event_time_cancel(Event_HMD_WaitStart);
                eos_event_pub_topic(Event_HMD_Start_MainFSM);
                return EOS_TRAN(HMD500_state_Process_Start);
            }
            return EOS_Ret_Handled;

        default:
            return EOS_SUPER(eos_state_top);
    }            
}

#if 1
static eos_ret_t HMD500_state_Process_Start(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
    byte *emsgdata;
    uint8_t *outdata_change;
    uint8_t *dcmcurrnum;
 //   DCM_RunCMD_FromPLC_t *dcm_on_off_flag;    //用于控制dcm开或关的完成标志
 //   uint8_t dcm_on_off_idx;
 //   uint8_t *dcm_adjust_change;   //用于确认哪个通道的dcm需要做电流调整
    uint8_t dcm_adjust_idx;

    uint8_t adjdata[30];
    uint8_t adj_whole_curr = ADJ_Whole_Curr;

    uint8_t issaved = 0;
    uint8_t saved_show_idx = 0;

    switch (e->topic) {
        case Event_Enter:
            LOG_info("HMD500_state_Process\r\n");
            //me->ECATstatus = LAN9255State_DisconnectedToPLC;
            //me->DCMOUTCurr_Cnt = 0;
            me->TransData_Status = 0;
            me->reTry_Idx = 0;
            memset(INDataValue_DCMState_Cache, 10201, sizeof(INDataValue_DCMState_Cache));      //默认都为正常状态
            memset(INDataValue_DCModule_Curr_Cache, 0, sizeof(INDataValue_DCModule_Curr_Cache));

            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("HMD500    main process ---------Event exit\r\n");
            return EOS_Ret_Handled;

        case Event_HMD_Start_MainFSM:
            LOG("HMD500    main process ---------Event HMD_Start_MainFSM\r\n");
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);      //启动读取PLC数据的事件
            eos_event_pub_period(Event_DCM_ReadDCMState, PeriodTime_10ms);                     //启动循环读取恒流模块的状态和电流值
            return EOS_Ret_Handled;

        case Event_HMD_End_MainFSM:
            LOG("HMD500    main process ---------Event HMD_End_MainFSM\r\n");
            me->status = 0;                                                 //将状态清零，等待重新开启主状态机
            eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
            eos_event_time_cancel(Event_DCM_ReadDCMState);
            //eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);

            sm_EtherCatPDO.status = 1;              //开启lan9255初始化的状态机

            eos_event_pub_period(Event_HMD_WaitStart, PeriodTime_1000ms);   //启动等待事件
            return EOS_TRAN(HMD500_state_Process_Stop);                     //转到等待开启主状态机的状态机中

        /* 循环读LAN9255状态 */
        case Event_ECAT_ReadLAN9255State_SendCMD:
            //LOG("HMD500    .....Event_ECAT_ReadLAN9255State_SendCMD\r\n");
            ReadLAN9255State_SendCMD();
            return EOS_Ret_Handled;
            
        case Event_ECAT_ReadLAN9255State_RecvRSP:
            emsgdata = (byte *)e->data; 
            LOG_info("HMD500     MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[emsgdata[0]]);
            if (emsgdata[0] == s_WaitInit) 
                eos_event_pub_topic(Event_HMD_End_MainFSM);
            
            else if (emsgdata[0] == s_Connected) 
            {
                eos_event_pub_topic(Event_ECAT_ReadDataFromLAN9255_SendCMD);
                //LOG("start_transdata : %d \r\n", me->TransData_Status);
                if (me->TransData_Status == 0)
                {
                    me->TransData_Status = 1;
                    eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, PeriodTime_500ms);
                }

            }
            else if (emsgdata[0] == s_DisConnected)
            {
                if (me->TransData_Status == 1)
                {
                    me->TransData_Status = 0;
                    eos_event_time_cancel(Event_ECAT_WriteDataToLAN9255_SendCMD);
                    //eos_event_pub_topic(Event_ECAT_CancelTransData);
                }
            }
            return EOS_Ret_Handled;       

        /* 循环发送数据给PLC */
        case Event_ECAT_WriteDataToLAN9255_SendCMD:
            //LOG("HMD500    .....Event_ECAT_WriteDataToLAN9255_SendCMD\r\n");
            WriteDataToLAN9255_SendCMD(PDO_MsgType);
            return EOS_Ret_Handled;

        case Event_ECAT_WriteDataToLAN9255_RecvRSP:
            LOG("write data to LAN9255 used SDO\r\n");
            return EOS_Ret_Handled;

        /* 循环从PLC读取数据 */
        case Event_ECAT_ReadDataFromLAN9255_SendCMD:
            ReadDataFromLAN9255_SendCMD();
            return EOS_Ret_Handled;

        /* 解析从PLC读取到的数据，并执行相应动作 */
        case Event_ECAT_ReadDataFromLAN9255_RecvRSP:
            outdata_change = (uint8_t *)e->data;   
            dcm_adjust_idx = 0;
            if (e->size <= 0)
                return EOS_Ret_Handled;

            /* 保存读取到的数据 */
            memcpy(&OUTDataValue_Cache, &OUTDataValue, sizeof(OUTDataValue));

            for (int i = 0; i < e->size; i++)
            {
                if (OUTDataValue_Cache.OUTData_DCModule_Enable[outdata_change[i]] == ADJ_Whole_Curr)  
                {
                    uint8_t adjd_f = OUTDataValue_Cache.OUTData_DCModule_AdjustOUTCurr[outdata_change[i]];
                    if((adjd_f >= 50 && adjd_f <= 150) || adjd_f == 0)
                    {
                        DCM_AdjustOUTCurr_Value[outdata_change[i]] = adjd_f;
                        DCM_Adjust_Flag[outdata_change[i]] = DCM_Adjust_FromPLC_Goto;
                        dcm_adjust_idx++;
                    }
                    else
                    {
                        LOG_error("OUTData_DCModule_AdjustOUTCurr error\r\n");
                        DCM_Adjust_Flag[outdata_change[i]] = DCM_Adjust_FromPLC_SetData_Err;
                    }

                }
                if (DCM_Adjust_Flag[outdata_change[i]] == DCM_Adjust_FromPLC_Saved)
                {
                    saved_show_idx++;
                    if (saved_show_idx > 2)
                    {
                        saved_show_idx = 0;
                        DCM_Adjust_Flag[outdata_change[i]] = DCM_Adjust_FromPLC_Default;
                    }
                }
            }

            eos_event_pub(Event_DCM_CtrlOUTCurr, outdata_change, e->size);

            return EOS_Ret_Handled;

        /* 循环读取恒流模块的状态 和 输出电流值 */
        case Event_DCM_ReadDCMState:
            //LOG("Event_DCM_ReadDCMState\r\n");
            /* 读取各个通道的输出电流值 & 输出电压值 & 错误状态Fault & 是否有输入电压*/
            for (int i = 0; i < DCModuleNum; i++)
            { 
                INDataValue_DCMState_Cache[i] = dcm_get_dcmstate(i);
                if (INDataValue_DCMState_Cache[i] != DCMState_Fault)
                    INDataValue.INData_DCMState[i]= INDataValue_DCMState_Cache[i];        //dcm_get_dcmstate(i);
                
                /* 如果没有输入电压，则输出电流值都显示为0 */
                if (dcm_get_pdet(i) != DCM_No_INVolt)
                {
                    float get_out_curr = round(dcm_get_outcurrent(i) * 100) / 100;
                    if(get_out_curr > 0.2)
                        INDataValue.INData_DCModule_Curr[i] = get_out_curr;     //获取输出电流值
                }

            }
            return EOS_Ret_Handled;

        /* 对恒流模块执行校准操作 */
        case Event_DCM_Adjust:
            dcm_adjust_idx = 0;
            uint8_t goto_save = 0;
            for (int i = 0; i < DCModuleNum; i++)
            {
                //LOG("~~~~~~~~~~~~DCM_Adjust_Flag[%d] : %d\r\n", i, DCM_Adjust_Flag[i]);
                if (DCM_Adjust_Flag[i] == DCM_Adjust_FromPLC_Goto)
                    dcm_adjust_idx++;

                else if (DCM_Adjust_Flag[i] == DCM_Adjust_FromPLC_Doing)
                    goto_save = 1;
            }
            //LOG("dcm_adjust_idx : %d\r\n", dcm_adjust_idx);
            /* 如果还有调整参数等待被保存，就先退出。等所有调整参数都确定后，再一起保存 */
            if (goto_save == 1 && dcm_adjust_idx == 0)
                eos_event_pub_topic(Event_DCM_Save_AdjustPara);
            
            return EOS_Ret_Handled;

        case Event_DCM_Save_AdjustPara:
            issaved = 0;
            
            for (int i = 0; i < 3; i++)
            {
                memset(adjdata, 0, sizeof(adjdata));
                memcpy(adjdata, &adj_whole_curr, 1);        //先保存标志
                //LOG("adjdata : ");
                for (int i = 0; i < DCModuleNum; i++)
                {
                    memcpy(adjdata + 1 + i * 1, &(DCM_AdjustOUTCurr_Value[i]), 1);
                //    LOG(" %f------", DCM_AdjustOUTCurr_Value[i]);
                //    LOG("%X%X%X%X,  ", adjdata[1 + i*4], adjdata[1 + i*4 + 1], adjdata[1 + i*4 + 2], adjdata[1 + i*4 + 3]);
                }
                //LOG("\r\n");
                
                if (ee_SavetoFLASH(_EE_USE_AdjustCurr_START_ADDR, DCModuleNum * 1 + 1, adjdata) == true)        //保存参数到flash
                {
                    LOG_info("Save Adjust data to FLASH Success! \r\n");
                    issaved = 1;
                    break;
                }
                else
                {
                    LOG_error("save adjust-from-plc error\r\n");
                }     
            }

            LOG("is saved : %d\r\n", issaved);
            for(int i = 0; i < DCModuleNum; i++)
            {
                if (issaved)
                {
                    dcm_set_adjustcurr(i, DCM_AdjustOUTCurr_Value[i]);
                    DCM_Adjust_Flag[i] = DCM_Adjust_FromPLC_Saved;
                }
                else 
                    DCM_Adjust_Flag[i] = DCM_Adjust_FromPLC_Save_Err;
                
                LOG(" %d,  ", DCM_Adjust_Flag[i]);
            }
            LOG("\r\n");

            return EOS_Ret_Handled;

        /* 对恒流模块执行控制：使能 & 控制输出电流 */
        case Event_DCM_CtrlOUTCurr:
            //LOG("Event_DCM_CtrlOUTCurr\r\n");
            dcmcurrnum = (uint8_t *)e->data; 
            /* 判断读取到的数据，并去执行相应的动作 */
            dcm_adjust_idx = 0;
            for(int i = 0; i < e->size; i++)
            {
                if (OUTDataValue_Cache.OUTData_DCModule_Enable[dcmcurrnum[i]] == 1)                         /* 1. 执行打开恒流模块通道的操作 */
                {
                    float set_outcurr_v = OUTDataValue_Cache.OUTData_DCModule_SetCurr[dcmcurrnum[i]];              //(OUTDataValue_Cache.OUTData_DCModule_SetCurr[dcmcurrnum[i]] * DCM_AdjustOUTCurr_Value[dcmcurrnum[i]]) / 100;
                    open_dcm_outcurr(dcmcurrnum[i], set_outcurr_v);
                }
                else if (OUTDataValue_Cache.OUTData_DCModule_Enable[dcmcurrnum[i]] == 0)                    /* 2. 执行关闭恒流模块通道的操作 或者保存校准参数 */
                {
                    /* 先关闭对应通道 */
                    close_dcm_outcurr(dcmcurrnum[i]);

                    /* 如果前一个标志是正在校准，则校准完成，去保存参数到FLASH */
                    if (DCM_Adjust_Flag[dcmcurrnum[i]] == DCM_Adjust_FromPLC_Goto)
                    {
                        DCM_Adjust_Flag[dcmcurrnum[i]] = DCM_Adjust_FromPLC_Doing;
                        dcm_adjust_idx++;
                    }
                }
            }
            if (dcm_adjust_idx > 0)
                eos_event_pub_topic(Event_DCM_Adjust);    //eos_event_pub(Event_DCM_Adjust, dcmcurrnum, e->size);

            return EOS_Ret_Handled;

        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

#endif

