#include "hal.h"



void hal_reset(void)
{
    NVIC_SystemReset();
    /* should not run to here */
    while(1);
}

