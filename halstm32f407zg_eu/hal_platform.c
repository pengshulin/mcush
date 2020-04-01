#include "mcush.h"

/* platform related reset */
void hal_platform_reset(void)
{
}


/* platform related init */
void hal_platform_init(void)
{
#if HAL_CAN_INIT
    hal_can_init();
#endif
}
