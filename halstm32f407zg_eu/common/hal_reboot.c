#include "hal.h"


void hal_reboot(void)
{
    NVIC_SystemReset();
    while(1);  /* should not run to here */
}

