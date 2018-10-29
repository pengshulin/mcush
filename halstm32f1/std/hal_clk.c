#include "hal.h"


void hal_clk_init(void)
{  
    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
