
#ifndef EVENT_DEF_H__
#define EVENT_DEF_H__

#include "eventos.h"

#define WorkLed_Normal_ms   1000
#define workLed_Wait_ms     500
#define WorkLed_Error_ms    100

enum {
    Event_Test = Event_User,
    Event_WorkLed_Blink,
    Event_PrintLOG,

    Event_ActEnd,
    
    Event_Max
};

#endif
