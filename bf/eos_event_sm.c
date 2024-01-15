/* include ------------------------------------------------------------------ */
#include "eos_event.h"
#include "eventos.h"
#include "event_def.h"
#include "EtherCatPDO.h"
#include "DCMCtrl.h"
#include "ee.h"
#include "math.h"

#define USE_Event_UserTest
extern float Get_OUTCurr_Volt[DCModuleNum];
extern float Get_SetCurr_Volt[DCModuleNum]; 
extern uint16_t DCM_OUTCurr_Gain[20];

extern uint8_t DCM_SetCurr_Now_Cnt[DCModuleNum];

extern UART_HandleTypeDef huart3;

/* 用于校准的参数 */
const uint16_t ee_StartFrame = ee_use_StartFrame;

/*用于跟PC通讯的参数*/
uint8_t rDataFromPC[2];
uint8_t wDataToPC[2];

OUTDATAValueHandle OUTDataValue_Cache;
uint16_t INDataValue_DCMState_Cache[DCModuleNum];
float INDataValue_DCModule_Curr_Cache[DCModuleNum];

uint16_t DCMOUTCurr_Cnt = 0;

float _DCM_OUT_Volt[DCModuleNum];           //每个恒流模块的输出电压
uint8_t _DCM_INVolt_EN[DCModuleNum];        //检测是否有输入电压
uint8_t _DCM_Fault[DCModuleNum];            //每个恒流模块的错误状态

DCM_CalibrateState_t _DCM_Calibrate_Done = DCM_CalibrateState_Wait;            //校准完成标志


const char *HMD500_FirmWare = "V0.2.0";
const char* Enable_DCM_Log[]                = {"Disable!", "Enable!"};   
//char* DCModule_State_Log[]            = {"Working Fine!", "OUTPUT Over Current!", "No INPUT Voltage!", };

typedef struct eos_sm_hmd500_tag {
    eos_sm_t super;
    DCModule_State_t DCMstatus;
//    uint8_t DCM_Enable[DCModuleNum];
//    float DCM_SetCurr[DCModuleNum];
//    uint8_t repeatCnt;
//    uint16_t DCMOUTCurr_Cnt;
    LAN9255State_t ECATstatus;
    uint8_t TransData_Status;
    uint8_t testidx;
} eos_sm_hmd500_t;

eos_sm_hmd500_t sm_HMD500;

//EMessageHandler_t _eMsgH;
LAN9255State_t L9255State = LAN9255State_WaitInit; 

float ao = 0.1;     //test
/********************************************************************************
 * @brief 1、工作状态指示灯：led event sm
 * 
 ********************************************************************************/
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
    eos_event_pub_period(Event_DCM_ReadDataFromPC_Recv, PeriodTime_2ms);
    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_50ms);    
//    eos_event_pub_topic(Event_DCM_Calibrate);
#ifdef USE_Event_UserTest
    //eos_event_pub_period(Event_UserTest, 20);
//    eos_event_pub_topic(Event_UserTest);
#endif
    LOG("workled state init \r\n");
    return EOS_TRAN(dcm_calibrate);    //return EOS_TRAN(workled_state_on);
}

uint16_t tt[] = {1370, 1336, 1344, 1336, 1340, 1336, 1334, 1329, 1325, 1314, 
                 1313, 1310, 1307, 1306, 1301, 1299, 1298, 1298, 1298, 1298};
uint8_t eett[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t eereaddata[40];
uint16_t readtt[20];
static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            LOG("eos workled state -----Event_Enter \r\n");
            eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);
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
                LOG("and Set OUT Current : %f( Ictrl_v = %f )\r\n", OUTDataValue_Cache.OUTData_DCModule_SetCurr[i], Get_SetCurr_Volt[i]);
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
	    //    if (ee_read(0, sizeof(me->ee_use_sf), me->ee_use_sf) == false)
        //        LOG_error("read eeprom data faild!\r\n");
        //    if (memcmp(me->ee_use_sf, ee_use_StartFrame, sizeof(me->ee_use_sf)) == 1);


	
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


static eos_ret_t dcm_calibrate(eos_sm_led_t * const me, eos_event_t const * const e)
{
    uint16_t dcm_get_oc;
    uint16_t eeuseSF = ee_use_StartFrame;
    uint8_t *edata = (uint8_t *)e->data; 
    switch (e->topic) {
        case Event_Enter:
            LOG("dcm calibrate -----Event_Enter \r\n");
            me->status = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("dcm calibrate -----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_DCM_ReadDataFromPC_Recv:
            /* 等待接收PC发送过来的指令 */
	        if (HAL_UART_Receive_IT(&huart3, rDataFromPC, 2) == HAL_OK)
            {
                /* 1、先检测是否接收到进入校准模式的指令 */
                LOG("rdata : %x, %x\r\n", rDataFromPC[0], rDataFromPC[1]);
                if (rDataFromPC[0] == 0xCE && rDataFromPC[1] == 0xCE)
                {
                    _DCM_Calibrate_Done = DCM_CalibrateState_Doing;
                    LOG("DCM_CalibrateState_Doing\r\n");
                }

                /* 2、等待设置输出电流指令 rDataFromPC[1]:设置输出电流标志位, rDataFromPC[0]:输出电流值 */
                if (((rDataFromPC[1] & 0xF0) == 0xA0) && (_DCM_Calibrate_Done == DCM_CalibrateState_Doing))
                {
                    uint8_t dcm_idx = (rDataFromPC[1] & 0x0F) - 1;
                    LOG("dcm_idx : %d\r\n", dcm_idx);
                    LOG("outcurr value : %f\r\n", (rDataFromPC[0] / 10.0));
                    dcm_set_outcurrent(dcm_idx, (rDataFromPC[0] / 10.0));
                    dcm_enable_output(dcm_idx, 1);  
                    eos_event_pub(Event_DCM_SendDataToPC_Send, &dcm_idx, 1);
                }

                /* 4、退出校准模式 */
                if (rDataFromPC[0] == 0xEC && rDataFromPC[1] == 0xEC)
                {
                    /* 去保存参数到flash的127页面 */
                    //ee_write(0, sizeof(me->ee_use_sf), me->ee_use_sf);

                    /* 退出校准dcm的状态机 */
                    eos_event_time_cancel(Event_WorkLed_Blink);
                    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_1000ms);
                    _DCM_Calibrate_Done = DCM_CalibrateState_Done;
                    return EOS_TRAN(workled_state_on);
                }
            }
            return EOS_Ret_Handled;

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

        case Event_WorkLed_Blink:
            //LOG("dcm calibrate workled blink -----Event_Time_100ms");    
            WorkLed_TogglePin;

            /* 如果正在校准中，就退出 */
            if (_DCM_Calibrate_Done == DCM_CalibrateState_Doing)
                return EOS_Ret_Handled;

            me->status++;
            if (me->status >= 100)      //设置等待10s
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
                    eos_event_time_cancel(Event_WorkLed_Blink);
                    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_1000ms);
                    _DCM_Calibrate_Done = DCM_CalibrateState_Done;
                    return EOS_TRAN(workled_state_on);
                }
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
 * @brief  2、跟LAN9255通讯用的相关状态机  
 * 
 ********************************************************************************/
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */

typedef struct eos_sm_ecat_tag {
    eos_sm_t super;
    uint8_t repeatCnt;
    uint8_t timeoout_idx;
    uint8_t status;
    uint16_t previewEvent;
    uint8_t LAN9255_InitDone;

} eos_sm_ecat_t;

enum {
    cmd_first_send,
    cmd_rsp_wait,
    cmd_rsp_ok,
    cmd_rsp_err
};

eos_sm_ecat_t sm_EtherCatPDO;

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e);
static eos_ret_t EtherCatPDO_state_InitLAN9255(eos_sm_ecat_t * const me, eos_event_t const * const e);
static eos_ret_t HMD500_state_Process(eos_sm_hmd500_t * const me, eos_event_t const * const e);

//static eos_ret_t EtherCatPDO_state_DataTransferWithPLC(eos_sm_ecat_t * const me, eos_event_t const * const e);


/* api ---------------------------------------------------- */
void eos_sm_EtherCatPDO_init(void)
{
    LOG("eos sm EtherCatPDO init \r\n");
    eos_sm_init(&sm_EtherCatPDO.super, 1, EOS_NULL);
    eos_sm_start(&sm_EtherCatPDO.super, EOS_STATE_CAST(EtherCatPDO_state_init));
   
    sm_EtherCatPDO.status = cmd_first_send; //cmd_rsp_ok;
    
}

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
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

    EOS_EVENT_SUB(Event_ECAT_StartEtherCat);
#endif

    eos_event_pub_period(Event_ECAT_SendCMD_TIMEOUT, PeriodTime_2000ms);
//    eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);
    LOG("Event_EtherCatPDO state init \r\n");
    return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
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
            LOG("EtherCatPDO_state_InitLAN9255-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        /*********************      刚上电的时候，先读取LAN9255状态      *********************/
        case Event_ECAT_ReadLAN9255State_SendCMD:
            //LOG("Event_ECAT_ReadLAN9255State_SendCMD \r\n");
            me->status = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_ReadLAN9255State_SendCMD;
            ReadLAN9255State_SendCMD();

            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_RecvRSP:
            //me->status = cmd_rsp_ok;
            //byte *edata = (byte *)e->data;    //(byte *)&e->data[0];
            //LOG("e->data : %d\r\n", edata[0]);
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[edata[0]]);
            //if ((byte *)&e->data[0] == s_WaitInit)
            if (edata[0] == s_WaitInit)
            {
                me->status = cmd_rsp_ok;
                //me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                //eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, PeriodTime_500ms);
                eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
            }
            else
            {
                me->status = cmd_rsp_err;
                me->repeatCnt = MaxRepeatCnt;
            }
            return EOS_Ret_Handled;        

        /*********************      添加属性表      *********************/
        case Event_ECAT_AddParaList_SendCMD:
            //me->repeatCnt += 1;
            me->status = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_AddParaList_SendCMD;
            AddParaList_SendCMD(ParaList_Num);
            return EOS_Ret_Handled;       

        case Event_ECAT_AddParaList_RecvRSP:
            LOG_info("MsgType is 0x02 : Add ParaList!......result : Add [ %s ] to paraList %s\r\n", OBJ_H[ParaList_Num].ObjName, Result_Log[edata[2]]);
            if (edata[2] == 1)
            {
                me->status = cmd_rsp_ok;
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
                me->status = cmd_rsp_err;
            }
            return EOS_Ret_Handled;
        
        /*********************      设置关键数据的超时时间      *********************/
        case Event_ECAT_SetMsgTimeout_SendCMD:
            me->status = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_SetMsgTimeout_SendCMD;
            setMsgCheckTimeout_SendCMD();

            return EOS_Ret_Handled;         

        case Event_ECAT_SetMsgTimeout_RecvRSP:
            LOG_info("MsgType is 0x0A : Set Msg Time Out!......result : ");
            if (KeyOUTData_TimeOut == word(edata[1], edata[0]))
            {
                me->status = cmd_rsp_ok;
                LOG("Success!\r\n");
                eos_event_pub_topic(Event_ECAT_StartLAN9255_SendCMD);
            }
            else
            {
                me->status = cmd_rsp_err;
                LOG("Fail!\r\n");
            }
            return EOS_Ret_Handled;

        /*********************      启动LAN9255      *********************/
        case Event_ECAT_StartLAN9255_SendCMD:
            me->status = cmd_rsp_wait;
            me->previewEvent = Event_ECAT_StartLAN9255_SendCMD;
            StartDevice_SendCMD();     
            return EOS_Ret_Handled;

        case Event_ECAT_StartLAN9255_RecvRSP:
            LOG_info("MsgType is 0x03 : Start Device!......result : %s\r\n", Result_Log[edata[0]]);
            if (edata[0] == 1) 
            {
                me->LAN9255_InitDone = 1;   //初始化完成
                me->status = cmd_rsp_ok;
                eos_event_pub_topic(Event_ECAT_END_InitLAN9255);
            }
            else 
                me->status = cmd_rsp_err;
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
            
            if (me->status != cmd_rsp_ok)
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

        case Event_ECAT_Start_InitLAN9255:
            LOG("Event_ECAT_Start_InitLAN9255 \r\n");
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);
            return EOS_Ret_Handled;
        
        case Event_ECAT_END_InitLAN9255:
            LOG("Event_ECAT_END_InitLAN9255 \r\n");
            eos_event_pub_topic(Event_ECAT_StartEtherCat);
            return EOS_TRAN(HMD500_state_Process);  //转到主状态机

        default:
            if (me->LAN9255_InitDone == 1)
                LOG("Event_ECAT_state_InitLAN9255 default\r\n");
            return EOS_SUPER(eos_state_top);
    }
}

#endif

/********************************************************************************
 * @brief  3、主状态机：读取恒流模块的各个参数，控制恒流模块的电流输出，EtherCat的数据收发
 * 从PLC读取数据
 * 往PLC发送数据
 * 输入：控制输出电流 DCM_Ictrl ; 使能恒流模块 DCM_EN ; 
 * 输出：读取输出电流 DCM_Iout ; 读取输出电压 DCM_Vout ; 读取恒流模块的状态 DCM_Fault
 * 
 ********************************************************************************/
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */

#if 0
typedef struct eos_sm_hmd500_tag {
    eos_sm_t super;
    DCModule_State_t DCMstatus;
    uint8_t repeatCnt;
    LAN9255State_t ECATstatus;
} eos_sm_hmd500_t;

eos_sm_hmd500_t sm_HMD500;
#endif

/* static state function ---------------------------------------------------- */
static eos_ret_t HMD500_state_init(eos_sm_hmd500_t * const me, eos_event_t const * const e);
//static eos_ret_t HMD500_state_Process(eos_sm_hmd500_t * const me, eos_event_t const * const e);

/* api ---------------------------------------------------- */
void eos_sm_HMD500_init(void)
{
    LOG("eos sm HMD500 init \r\n");
    eos_sm_init(&sm_HMD500.super, 0, EOS_NULL);
    eos_sm_start(&sm_HMD500.super, EOS_STATE_CAST(HMD500_state_init));
    
}

/* static state function ---------------------------------------------------- */
static eos_ret_t HMD500_state_init(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    
    EOS_EVENT_SUB(Event_DCM_ReadDCMState);
    EOS_EVENT_SUB(Event_DCM_CtrlOUTCurr);
    EOS_EVENT_SUB(Event_ECAT_CancelEtherCat);
    EOS_EVENT_SUB(Event_ECAT_StartTransData);
    EOS_EVENT_SUB(Event_ECAT_CancelTransData);
#endif
    eos_event_pub_period(Event_DCM_ReadDCMState, 10);//PeriodTime_100ms);
    LOG("HMD500 main state init \r\n");
    return EOS_TRAN(HMD500_state_Process);
}

#if 1
static eos_ret_t HMD500_state_Process(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
    byte *emsgdata;
    uint8_t *ocdata;
    uint8_t *dcmcurrnum;
    switch (e->topic) {
        case Event_Enter:
            LOG_info("HMD500_state_Process\r\n");
            me->ECATstatus = LAN9255State_DisconnectedToPLC;
            //me->DCMOUTCurr_Cnt = 0;
            me->TransData_Status = 0;
            me->testidx = 0;
            memset(INDataValue_DCMState_Cache, 0, sizeof(INDataValue_DCMState_Cache));
            memset(INDataValue_DCModule_Curr_Cache, 0, sizeof(INDataValue_DCModule_Curr_Cache));
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("HMD500    main process ---------Event exit\r\n");
            return EOS_Ret_Handled;


        /* 循环读LAN9255状态 */
        case Event_ECAT_ReadLAN9255State_SendCMD:
            //LOG("HMD500    .....Event_ECAT_ReadLAN9255State_SendCMD\r\n");
            ReadLAN9255State_SendCMD();
            return EOS_Ret_Handled;
            
        case Event_ECAT_ReadLAN9255State_RecvRSP:
            //LOG("Event_ECAT_ReadLAN9255State_RecvRSP start_transdata : %d \r\n", me->TransData_Status);
            emsgdata = (byte *)e->data; 
            LOG_info("HMD500     MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[emsgdata[0]]);
            if (emsgdata[0] == s_WaitInit) 
            {
                //eos_event_pub_topic(Event_ECAT_CancelEtherCat);
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_time_cancel(Event_ECAT_WriteDataToLAN9255_SendCMD);
                eos_event_pub_topic(Event_ECAT_Start_InitLAN9255);
                return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            } 
            else if (emsgdata[0] == s_Connected) 
            {
                me->ECATstatus = LAN9255State_ConnectedToPLC;

                eos_event_pub_topic(Event_ECAT_ReadDataFromLAN9255_SendCMD);
                //LOG("start_transdata : %d \r\n", me->TransData_Status);
                if (me->TransData_Status == 0)
                {
                    me->TransData_Status = 1;
                    eos_event_pub_topic(Event_ECAT_StartTransData);
                }

            }
            else if (emsgdata[0] == s_DisConnected)
            {
                me->ECATstatus = LAN9255State_DisconnectedToPLC;
                if (me->TransData_Status == 1)
                {
                    me->TransData_Status = 0;
                    eos_event_pub_topic(Event_ECAT_CancelTransData);
                }
                //eos_event_time_cancel(Event_ECAT_WriteDataToLAN9255_SendCMD);
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
            ocdata = (uint8_t *)e->data;   
            /* 保存读取到的数据 */
            if (e->size > 0)
            {
                //LOG("e->size : %d\r\n", e->size);
                //DCMOUTCurr_Cnt = 301;                                               //用于及时反馈输出电流值
                memcpy(&OUTDataValue_Cache, &OUTDataValue, sizeof(OUTDataValue));
#ifdef USE_AdjustFunc
                LOG(".....active : %d ", OUTDataValue_Cache.OUTData_DCModule_Enable[0]);
                LOG(" %d ", OUTDataValue_Cache.OUTData_DCModule_Enable[1]);
                LOG(" %d ", OUTDataValue_Cache.OUTData_DCModule_Enable[2]);
                LOG(" %d ", OUTDataValue_Cache.OUTData_DCModule_Enable[3]);
                LOG(" %d \r\n", OUTDataValue_Cache.OUTData_DCModule_Enable[4]);
                if (OUTDataValue_Cache.OUTData_DCModule_Enable)
#endif
                eos_event_pub(Event_DCM_CtrlOUTCurr, ocdata, e->size);
            }
            return EOS_Ret_Handled;

        /* 循环读取恒流模块的状态 和 输出电流值 */
        case Event_DCM_ReadDCMState:
            //LOG("Event_DCM_ReadDCMState\r\n");
            /* 读取各个通道的输出电流值 & 输出电压值 & 错误状态Fault & 是否有输入电压*/
            //DCMOUTCurr_Cnt++;
            for (int i = 0; i < DCModuleNum; i++)
            {
                INDataValue.INData_DCModule_Curr[i] = round(dcm_get_outcurrent(i)*100) / 100;    //获取输出电流值
                #if 0
                INDataValue_DCModule_Curr_Cache[i] = round(dcm_get_outcurrent(i) * 100) / 100; 
                if (DCMOUTCurr_Cnt > 150)
                {
                    LOG("DCMOUTCurr_Cnt : %d\r\n", DCMOUTCurr_Cnt);
                    INDataValue.INData_DCModule_Curr[i] = INDataValue_DCModule_Curr_Cache[i];
                    if (i >= DCModuleNum - 1)
                        DCMOUTCurr_Cnt = 0;
                }
                #endif

                INDataValue_DCMState_Cache[i] = dcm_get_dcmstate(i);
                if (INDataValue_DCMState_Cache[i] != DCMState_Fault)
                    INDataValue.INData_DCMState[i]= INDataValue_DCMState_Cache[i];        //dcm_get_dcmstate(i);
            }
            return EOS_Ret_Handled;

        /* 对恒流模块执行控制：使能 & 控制输出电流 */
        case Event_DCM_CtrlOUTCurr:
            //LOG("Event_DCM_CtrlOUTCurr\r\n");
            dcmcurrnum = (uint8_t *)e->data; 
#ifdef Run_AllDCM_mode
            for(int i = 0; i < DCModuleNum; i++)
            {
                //关闭恒流模块的时候，同时将设置的输出电流值改为0
                if (OUTDataValue_Cache.OUTData_DCModule_Enable[i] == 0)
                {
                    dcm_enable_output(i, OUTDataValue_Cache.OUTData_DCModule_Enable[i]);
                    dcm_set_outcurrent(i, 0);
                } 
                else    
                {
                    //打开恒流模块之前，需先确认几个致命错误是否存在？
                    if ((INDataValue.INData_DCMState[i] != DCMState_Set_OutCurr_Err) &&
                        (INDataValue.INData_DCMState[i] != DCMState_IN_NoVolt) &&
                        (OUTDataValue_Cache.OUTData_DCModule_SetCurr[i] > DCM_OUTCurr_Limit))
                    {
                        dcm_enable_output(i, OUTDataValue.OUTData_DCModule_Enable[i]);
                        dcm_set_outcurrent(i, OUTDataValue.OUTData_DCModule_SetCurr[i]);
                    }
                }
            }
#else
            /* 判断读取到的数据，并去执行相应的动作 */
            uint8_t oc_idx = 0;
            for(int i = 0; i < e->size; i++)
            {
                if (dcmcurrnum[i] < DCModuleNum)
                    oc_idx = dcmcurrnum[i];
                else if ((dcmcurrnum[i] >= DCModuleNum) && (dcmcurrnum[i] < DCModuleNum * 2))
                    oc_idx = dcmcurrnum[i] - DCModuleNum;
#ifdef USE_AdjustFunc
                else if (dcmcurrnum[i] >= DCModuleNum * 2)
                    oc_idx = dcmcurrnum[i] - DCModuleNum * 2;
#endif
                else
                {
                    LOG_error("in Event_DCM_ReadDCMState. channel error!\r\n");
                    return EOS_Ret_Handled;
                }
                dcm_set_outcurrent(oc_idx, OUTDataValue_Cache.OUTData_DCModule_SetCurr[oc_idx]);
                dcm_enable_output(oc_idx, OUTDataValue_Cache.OUTData_DCModule_Enable[oc_idx]);  
            }
#endif
            return EOS_Ret_Handled;

        case Event_ECAT_StartTransData:
            //me->TransData_Stattus = 1;   
            LOG("HMD500    .....Event_ECAT_StartTransData  %d\r\n", me->TransData_Status);
            
            eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, PeriodTime_500ms);
            return EOS_Ret_Handled;

        case Event_ECAT_CancelTransData:
            //me->TransData_Stattus = 0;
            LOG("HMD500     Event_ECAT_CancelTransData transdata state: %d\r\n", me->TransData_Status);
            eos_event_time_cancel(Event_ECAT_WriteDataToLAN9255_SendCMD);
            return EOS_Ret_Handled;

        case Event_ECAT_StartEtherCat:
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);      //启动读取PLC数据的事件
            return EOS_Ret_Handled;
        
        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

#endif

