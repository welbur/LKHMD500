#include "eventos.h"
#include "main.h"
#include "event_def.h"
// #include "LOG.h"
extern IWDG_HandleTypeDef hiwdg;

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

    while (1)
    {
    }
}
//uint8_t status_button = 0;
void eos_hook_idle(void)
{
    EtherCatPDO_ReceiveFrame();
    if(HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
    {
      /* Refresh Error */
      Error_Handler();
    }
}

void eos_hook_start(void)
{
}

void eos_hook_stop(void)
{
}
