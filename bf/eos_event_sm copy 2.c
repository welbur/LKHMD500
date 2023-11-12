/* include ------------------------------------------------------------------ */
#include "eos_event.h"
#include "eventos.h"
#include "event_def.h"
//#include <stdio.h>
//#include "LOG.h"
//#include "MSP_GPIO.h"
//#include "FrameFormat.h"
#include "EtherCatPDO.h"
#include "DCMCtrl.h"
#include "main.h"

OUTDATAValueHandle OUTDataValue_Cache;

float _DCM_OUT_Volt[DCModuleNum];           //每个恒流模块的输出电压
uint8_t _DCM_INVolt_EN[DCModuleNum];        //检测是否有输入电压
uint8_t _DCM_Fault[DCModuleNum];            //每个恒流模块的错误状态


typedef struct eos_sm_hmd500_tag {
    eos_sm_t super;
    DCModule_State_t DCMstatus;
//    uint8_t DCM_Enable[DCModuleNum];
//    float DCM_SetCurr[DCModuleNum];
    uint8_t repeatCnt;
    LAN9255State_t ECATstatus;
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
} eos_sm_led_t;

eos_sm_led_t sm_led;

uint8_t status_button = 0;  // = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);

/* static state function ---------------------------------------------------- */
static eos_ret_t workled_state_init(eos_sm_led_t * const me, eos_event_t const * const e);
static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e);
//static eos_ret_t workled_state_off(eos_sm_led_t * const me, eos_event_t const * const e);


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
    // 按下按下或者抬起
    
//    eos_event_pub(Event_Button, (void *)&status_button, sizeof(uint8_t));
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_WorkLed_Blink);
    EOS_EVENT_SUB(Event_Button);
    EOS_EVENT_SUB(Event_PrintLOG);
#endif
    eos_event_pub_period(Event_WorkLed_Blink, PeriodTime_500ms);
    eos_event_pub_period(Event_PrintLOG, PeriodTime_2000ms);
//    eos_event_pub_period(Event_PrintLOG, 200);
    LOG("state init \r\n");
    return EOS_TRAN(workled_state_on);
}


static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e)
{
//    LOG("eos state on \r\n");
    switch (e->topic) {
        case Event_Enter:
            me->status = 1;
            //WorkLed(me->status);
            LOG("eos state on-----Event_Enter \r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("eos state on-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_PrintLOG:
            //LOG("prnit log..........................%d\r\n", eos_time());
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

#if 1
        case Event_WorkLed_Blink:
            //LOG("eos state on-----Event_Time_500ms %d\r\n", HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin));
            WorkLed_TogglePin;
            
            return EOS_Ret_Handled;     //EOS_TRAN(state_off);
#endif

        case Event_Button:
            //eos_event_time_cancel(Event_Button);
            LOG("eos state on-----Event_Button %d\r\n", *(uint8_t *)e->data);
            return EOS_Ret_Handled; 
        default:
            LOG("eos state on-----default \r\n");
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
    LAN9255State_t status;
} eos_sm_ecat_t;

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
   
    sm_EtherCatPDO.status = LAN9255State_Default;
}

/* static state function ---------------------------------------------------- */
static eos_ret_t EtherCatPDO_state_init(eos_sm_ecat_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
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
//    EOS_EVENT_SUB(Event_ECAT_TransDataToLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_WriteDataToLAN9255_RecvRSP);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_SendCMD);
    EOS_EVENT_SUB(Event_ECAT_ReadDataFromLAN9255_RecvRSP);
#endif

    eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);
//    eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, PeriodTime_500ms);
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
            LOG_info("EtherCatPDO_state_InitLAN9255\r\n");
            me->status = LAN9255State_Initting;
            me->repeatCnt = 0;
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        /*********************      初始化过程中，通讯异常，就重启LAN9255      *********************/
        case Event_ECAT_RestartLAN9255S_SendCMD:
            //RestartLAN9255();
            LOG("test.....444444\r\n");
            me->repeatCnt += 1;
            if (me->repeatCnt >= 3)
            {
                me->repeatCnt = 0;
                LOG("test.....555555\r\n");
                //EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);
                eos_event_time_cancel(Event_ECAT_RestartLAN9255S_SendCMD);
                eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);
            }
            return EOS_Ret_Handled;

        case Event_ECAT_RestartLAN9255S_RecvRSP:
            me->repeatCnt = 0;
            /*需要发送一个rst复位信号 引脚复位*/
            
            EOS_EVENT_UNSUB(Event_ECAT_RestartLAN9255S_SendCMD);

            /*复位之后，重新启动循环事件：读取LAN9255状态*/
            eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        /*********************      刚上电的时候，先读取LAN9255状态      *********************/
        case Event_ECAT_ReadLAN9255State_SendCMD:
            ReadLAN9255State_SendCMD();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, PeriodTime_5000ms);
            }
            return EOS_Ret_Handled;

        case Event_ECAT_ReadLAN9255State_RecvRSP:
            //byte *edata = (byte *)e->data;    //(byte *)&e->data[0];
            //LOG("e->data : %d\r\n", edata[0]);
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[edata[0]]);
            //if ((byte *)&e->data[0] == s_WaitInit)
            if (edata[0] == s_WaitInit)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                //eos_event_pub_period(Event_ECAT_AddParaList_SendCMD, PeriodTime_500ms);
                eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
            }
            return EOS_Ret_Handled;        

        /*********************      添加属性表      *********************/
        case Event_ECAT_AddParaList_SendCMD:
            //me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                //eos_event_time_cancel(Event_ECAT_AddParaList_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, PeriodTime_5000ms);
            } else
                AddParaList_SendCMD(ParaList_Num);
            return EOS_Ret_Handled;       

        case Event_ECAT_AddParaList_RecvRSP:
            LOG_info("MsgType is 0x02 : Add ParaList!......result : Add [ %s ] to paraList %s\r\n", OBJ_H[ParaList_Num].ObjName, Result_Log[edata[2]]);
            if (edata[2] == 1)
            {
                me->repeatCnt = 0;
                ParaList_Num++;
                if (ParaList_Num >= ObjList_Num)
                {
                    ParaList_Num = 0;
                    eos_event_pub_period(Event_ECAT_SetMsgTimeout_SendCMD, PeriodTime_1000ms);        
                }
                else 
                {
                    eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
                }
            }
            else
            {
                me->repeatCnt += 1;
                eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
            }
#if 0
            if (ParaList_Num >= ObjList_Num)
            {
                LOG("ParaList_Num : %d\r\n", ParaList_Num);
                ParaList_Num = 0;
                //eos_event_time_cancel(Event_ECAT_AddParaList_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限制
                eos_event_pub_period(Event_ECAT_SetMsgTimeout_SendCMD, PeriodTime_1000ms);        
            }
#endif
            return EOS_Ret_Handled;
        
        /*********************      设置关键数据的超时时间      *********************/
        case Event_ECAT_SetMsgTimeout_SendCMD:
            setMsgCheckTimeout_SendCMD();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_SetMsgTimeout_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, PeriodTime_5000ms);
            }
            return EOS_Ret_Handled;         

        case Event_ECAT_SetMsgTimeout_RecvRSP:
            LOG_info("MsgType is 0x0A : Set Msg Time Out!......result : ");
            if (KeyOUTData_TimeOut == word(edata[1], edata[0]))
            {
                me->repeatCnt = 0;
                LOG("Success!\r\n");
                eos_event_time_cancel(Event_ECAT_SetMsgTimeout_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限�
                eos_event_pub_period(Event_ECAT_StartLAN9255_SendCMD, PeriodTime_1000ms);
            }
            else
                LOG("Fail!\r\n");
            return EOS_Ret_Handled;

        /*********************      启动LAN9255      *********************/
        case Event_ECAT_StartLAN9255_SendCMD:
            StartDevice_SendCMD();
            me->repeatCnt += 1;
            if (me->repeatCnt >= MaxRepeatCnt)
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_StartLAN9255_SendCMD);
                eos_event_pub_period(Event_ECAT_RestartLAN9255S_SendCMD, PeriodTime_5000ms);
            }          
            return EOS_Ret_Handled;

        case Event_ECAT_StartLAN9255_RecvRSP:
            LOG_info("MsgType is 0x03 : Start Device!......result : %s\r\n", Result_Log[edata[0]]);
            if (edata[0] == 1) 
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_StartLAN9255_SendCMD);     //取消没用的延时或周期时间，防止时间事件超过最大限
                eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);      //启动读取PLC数据的事件
                eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, PeriodTime_5000ms);    //启动与PLC交换数据的事件
                return EOS_TRAN(HMD500_state_Process);  //转到主状态机
            }
            return EOS_Ret_Handled;

        default:
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
#endif
    eos_event_pub_period(Event_DCM_ReadDCMState, PeriodTime_1000ms);
    LOG("HMD500 main state init \r\n");
    return EOS_TRAN(HMD500_state_Process);
}

#if 1
static eos_ret_t HMD500_state_Process(eos_sm_hmd500_t * const me, eos_event_t const * const e)
{
    //byte *edata = (byte *)e->data; 
    byte *emsgdata;
    uint8_t *ocdata;
    uint8_t *dcmcurrnum;
    switch (e->topic) {
        case Event_Enter:
            LOG_info("HMD500_state_Process\r\n");
            me->ECATstatus = LAN9255State_ConnectedToPLC;
            me->repeatCnt = 0;
            //me->DCMstatus = DCM_Stop;
            //eos_event_pub_period(Event_ECAT_ReadLAN9255State_SendCMD, PeriodTime_2000ms);      //启动读取PLC数据的事件
            //eos_event_pub_period(Event_ECAT_WriteDataToLAN9255_SendCMD, PeriodTime_5000ms);    //启动与PLC交换数据的事件
            return EOS_Ret_Handled;
        
        case Event_Exit:
            return EOS_Ret_Handled;

        /* 循环读LAN9255状态 */
        case Event_ECAT_ReadLAN9255State_SendCMD:
//            LOG("HMD500    .....Event_ECAT_ReadLAN9255State_SendCMD\r\n");
            ReadLAN9255State_SendCMD();
            return EOS_Ret_Handled;
            
        case Event_ECAT_ReadLAN9255State_RecvRSP:
            emsgdata = (byte *)e->data; 
            LOG_info("MsgType is 0x01 : Get Device Status!......result : %s\r\n", getDeviceState_Log[emsgdata[0]]);
            if (emsgdata[0] == s_WaitInit) 
            {
                me->repeatCnt = 0;
                eos_event_time_cancel(Event_ECAT_ReadLAN9255State_SendCMD);
                eos_event_pub_topic(Event_ECAT_AddParaList_SendCMD);
                return EOS_TRAN(EtherCatPDO_state_InitLAN9255);
            } 
            else if (emsgdata[0] == s_Connected) 
            {
                me->ECATstatus = LAN9255State_ConnectedToPLC;
                eos_event_pub_topic(Event_ECAT_ReadDataFromLAN9255_SendCMD);
            }
            else if (emsgdata[0] == s_DisConnected)
            {
                me->ECATstatus = LAN9255State_DisconnectedToPLC;
            }
            return EOS_Ret_Handled;       

        /* 循环发送数据给PLC */
        case Event_ECAT_WriteDataToLAN9255_SendCMD:
            //LOG("HMD500    .....Event_ECAT_WriteDataToLAN9255_SendCMD\r\n");
            if (me->ECATstatus == LAN9255State_ConnectedToPLC)
            {
                WriteDataToLAN9255_SendCMD(PDO_MsgType);
            }
            return EOS_Ret_Handled;

        case Event_ECAT_WriteDataToLAN9255_RecvRSP:
            LOG("write data to LAN9255 used SDO\r\n");
            return EOS_Ret_Handled;

        /* 循环从PLC读取数据 */
        case Event_ECAT_ReadDataFromLAN9255_SendCMD:
            ReadDataFromLAN9255_SendCMD();
            return EOS_Ret_Handled;

        case Event_ECAT_ReadDataFromLAN9255_RecvRSP:
            ocdata = (uint8_t *)e->data;   
            //LOG("read data from LAN9255\r\n");
            if (e->size <= 0)
                return EOS_Ret_Handled;

            if (memcmp(&OUTDataValue_Cache, &OUTDataValue, sizeof(OUTDataValue)))
            {
                memcpy(&OUTDataValue_Cache, &OUTDataValue, sizeof(OUTDataValue));
                eos_event_pub(Event_DCM_CtrlOUTCurr, ocdata, e->size);
            }
            return EOS_Ret_Handled;

        /* 循环读取恒流模块的状态 和 输出电流值 */
        case Event_DCM_ReadDCMState:
            //LOG("Event_DCM_ReadDCMState\r\n");
            /* 读取各个通道的输出电流值 & 输出电压值 & 错误状态Fault & 是否有输入电压*/
            for (int i = 0; i < DCModuleNum; i++)
            {
                INDataValue.INData_DCModule_Curr[i] = dcm_get_outcurrent(i);    //获取输出电流值
                INDataValue.INData_DCMState[i]= dcm_get_dcmstate(i);
                //_DCM_OUT_Volt[i] = dcm_get_outvoltage(i);                       //获取输出电压值
                //dcm_get_dcmfault(i);                                            //获取恒流模块的fault信号
                //dcm_get_pdet(i);                                                //获取衡量模块输入电压的状态值
            }

            return EOS_Ret_Handled;

        /* 对恒流模块执行控制：使能 & 控制输出电流 */
        case Event_DCM_CtrlOUTCurr:
            //LOG("Event_DCM_CtrlOUTCurr\r\n");
            dcmcurrnum = (uint8_t *)e->data; 

            /* 判断读取到的数据，并去执行相应的动作 */
            for(int i = 0; i < e->size; i++)
            {
                if (dcmcurrnum[i] < DCModuleNum)
                {
                    LOG("channel %d active : %d\r\n", dcmcurrnum[i], OUTDataValue_Cache.OUTData_DCModule_Enable[dcmcurrnum[i]]);
                    dcm_enable_output(dcmcurrnum[i], OUTDataValue_Cache.OUTData_DCModule_Enable[dcmcurrnum[i]]);
                }
                else if ((dcmcurrnum[i] >= DCModuleNum) && (dcmcurrnum[i] < DCModuleNum * 2))
                {
                    LOG("channel %d outdatavalue  : %f\r\n", dcmcurrnum[i], OUTDataValue_Cache.OUTData_DCModule_SetCurr[dcmcurrnum[i] - DCModuleNum]);
                    dcm_set_outcurrent(dcmcurrnum[i] - DCModuleNum, OUTDataValue_Cache.OUTData_DCModule_SetCurr[dcmcurrnum[i] - DCModuleNum]);
                }
                else 
                {
                    LOG_error("channel error!\r\n");
                }
            }

            return EOS_Ret_Handled;

        default:
            return EOS_SUPER(eos_state_top);
    }
}
#endif

#endif

