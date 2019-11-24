/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

#if HAL_REBOOT_COUNTER

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
#if defined(LL_AHB1_GRP1_PERIPH_BKPSRAM)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_BKPSRAM);
#endif
    LL_PWR_EnableBkUpAccess();
    hal_delay_ms(1);
    
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

#endif

