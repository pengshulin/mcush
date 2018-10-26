#include "hal.h"



void hal_reboot(void)
{
    NVIC_SystemReset();
    while(1);  /* should not run to here */
}

#define MAGIC_CODE  0x544F4F42  /* in ascii 'BOOT' */
typedef struct _reboot_counter_cb_t
{
    uint32_t magic_code;
    uint32_t counter; 
    uint32_t counter_rev;
}reboot_counter_cb_t;

reboot_counter_cb_t reboot_counter_cb  __attribute__((section(".bkpsram")));


void hal_reboot_counter_init(void)
{
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_BKPSRAM, ENABLE );
    PWR_BackupAccessCmd(ENABLE);
    
    if( (reboot_counter_cb.magic_code != MAGIC_CODE) 
        || (reboot_counter_cb.counter != ~reboot_counter_cb.counter_rev) )
    {
        hal_reboot_counter_reset();
    }
    else
    {
        reboot_counter_cb.counter++;
        reboot_counter_cb.counter_rev = ~reboot_counter_cb.counter;
    }
}


void hal_reboot_counter_reset(void)
{
    reboot_counter_cb.magic_code = MAGIC_CODE;
    reboot_counter_cb.counter = 0;
    reboot_counter_cb.counter_rev = ~0;
}


unsigned int hal_reboot_counter_get(void)
{
    return reboot_counter_cb.counter;
}


