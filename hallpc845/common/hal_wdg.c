#include "compiler.h"
#include "hal.h"

static uint8_t _wdg_enable=0;

void hal_wdg_enable(void);

/* override wdg apis for your target board
 * eg. toggle specific pin to feed external watchdog chip
 */
__weak void hal_wdg_init(void)
{
    //Chip_WWDT_Init(WWDT);
    //Chip_WWDT_SetOption(WWDT, WWDT_WDMOD_WDRESET);
    //Chip_WWDT_SetTimeOut(WWDT, 0xFFFFFF );  /* 1/(12MHz) * 2**24 * 4 --> 5.6s */

#if HAL_WDG_ENABLE
    hal_wdg_enable();
#endif
}

__weak int hal_wdg_is_enable(void)
{
    return _wdg_enable;
}

__weak void hal_wdg_enable(void)
{
    //Chip_WWDT_Start(LPC_WWDT);
    _wdg_enable = 1;
}

__weak void hal_wdg_disable(void)
{
    /* invalid */ 
}

__weak void hal_wdg_clear(void)
{
    //Chip_WWDT_Feed(LPC_WWDT);
}

