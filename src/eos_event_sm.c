/* include ------------------------------------------------------------------ */
#include "eos_event.h"
#include "eventos.h"
#include "event_def.h"
#include <stdio.h>
#include "LOG.h"
#include "MSP_GPIO.h"


/**
 * @brief led event sm
 * 
 */
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */
typedef struct eos_sm_led_tag {
    eos_sm_t super;

    eos_u8_t status;
} eos_sm_led_t;

eos_sm_led_t sm_led;

/* static state function ---------------------------------------------------- */
static eos_ret_t workled_state_init(eos_sm_led_t * const me, eos_event_t const * const e);
static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e);
//static eos_ret_t state_off(eos_sm_led_t * const me, eos_event_t const * const e);

/* api ---------------------------------------------------- */
void eos_sm_led_init(void)
{
    LOG("eos sm led init \r\n");
    eos_sm_init(&sm_led.super, 1, EOS_NULL);
    eos_sm_start(&sm_led.super, EOS_STATE_CAST(workled_state_init));
   
    sm_led.status = 0;
}

/* static state function ---------------------------------------------------- */
static eos_ret_t workled_state_init(eos_sm_led_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_WorkLed_Blink);
#endif
    eos_event_pub_period(Event_WorkLed_Blink, workLed_Wait_ms);
//    eos_event_pub_period(Event_PrintLOG, 200);
    LOG("state init \r\n");
    return EOS_TRAN(workled_state_on);
}

static eos_ret_t workled_state_on(eos_sm_led_t * const me, eos_event_t const * const e)
{
//    LOG("eos state on \r\n");
    switch (e->topic) {
        case Event_Enter:
            //me->status = 1;
            //WorkLed(me->status);
            //LOG("eos state on-----Event_Enter \r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("eos state on-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_WorkLed_Blink:
            //LOG("eos state on-----Event_Time_500ms \r\n");
            WorkLed_TogglePin;
            return EOS_Ret_Handled;     //EOS_TRAN(state_off);

        default:
//            LOG("eos state on-----default \r\n");
            return EOS_SUPER(eos_state_top);
    }
}
#if 0
static eos_ret_t state_off(eos_sm_led_t * const me, eos_event_t const * const e)
{
    LOG("eos state off \r\n");
    switch (e->topic) {
        case Event_Enter:
            me->status = 0;
            //WorkLed(me->status);
            LOG("eos state off-----Event_Enter \r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            LOG("eos state off-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_WorkLed_Blink:
            LOG("eos state off-----Event_Time_500ms \r\n");
            WorkLed_TogglePin;
            return EOS_Ret_Handled;     //EOS_TRAN(state_on);

        case Event_PrintLOG:
            LOG("event print log \r\n");
            return EOS_TRAN(state_on);

        default:
//            LOG("eos state off-----default \r\n");
            return EOS_SUPER(eos_state_top);
    }
}
#endif

#endif


/**
 * @brief printlog event sm
 * 
 */
#if (EOS_USE_SM_MODE != 0)
/* data structure ----------------------------------------------------------- */
typedef struct eos_sm_log_tag {
    eos_sm_t super;

    eos_u8_t status;
} eos_sm_log_t;

eos_sm_log_t sm_printlog;

/* static state function ---------------------------------------------------- */
static eos_ret_t printlog_state_init(eos_sm_led_t * const me, eos_event_t const * const e);
static eos_ret_t printlog_state_on(eos_sm_led_t * const me, eos_event_t const * const e);
//static eos_ret_t state_off(eos_sm_led_t * const me, eos_event_t const * const e);

/* api ---------------------------------------------------- */
void eos_sm_printlog_init(void)
{
    LOG("eos sm printlog init \r\n");
    eos_sm_init(&sm_printlog.super, 2, EOS_NULL);
    eos_sm_start(&sm_printlog.super, EOS_STATE_CAST(printlog_state_init));
   
    sm_printlog.status = 0;
}

/* static state function ---------------------------------------------------- */
static eos_ret_t printlog_state_init(eos_sm_led_t * const me, eos_event_t const * const e)
{
#if (EOS_USE_PUB_SUB != 0)
    EOS_EVENT_SUB(Event_PrintLOG);
#endif
//    eos_event_pub_period(Event_WorkLed_Blink, workLed_Wait_ms);
    eos_event_pub_period(Event_PrintLOG, 2000);
    LOG("printlog state init \r\n");
    return EOS_TRAN(printlog_state_on);
}

static eos_ret_t printlog_state_on(eos_sm_led_t * const me, eos_event_t const * const e)
{
    switch (e->topic) {
        case Event_Enter:
            //me->status = 1;
            //WorkLed(me->status);
            //LOG("printlog-----Event_Enter \r\n");
            return EOS_Ret_Handled;
        
        case Event_Exit:
            //LOG("printlog-----Event_Exit \r\n");
            return EOS_Ret_Handled;

        case Event_PrintLOG:
            //LOG("prnit log..........................%d\r\n", eos_time());
            return EOS_Ret_Handled;         //EOS_TRAN(state_off);

        default:
//            LOG("eos state on-----default \r\n");
            return EOS_SUPER(eos_state_top);
    }
}

#endif
