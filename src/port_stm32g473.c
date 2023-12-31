#include "eventos.h"
#include "main.h"
//#include "LOG.h"

void eos_port_critical_enter(void)
{
    __disable_irq();
}


void eos_port_critical_exit(void)
{
    __enable_irq();
}

eos_u32_t eos_error_id = 0;
void eos_port_assert(eos_u32_t error_id)
{
    LOG("------------------------------------\n");
    LOG("ASSERT >>> Module: EventOS Nano, ErrorId: %d.\n", error_id);
    LOG("------------------------------------\n");

    eos_error_id = error_id;

    while (1) {
    }
}

void eos_hook_idle(void)
{
    EtherCatPDO_process();
    //LOG("eos hook idle\r\n");
}

void eos_hook_start(void)
{

}

void eos_hook_stop(void)
{

}
