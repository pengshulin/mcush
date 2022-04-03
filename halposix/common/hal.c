#include "mcush.h"

 
int hal_init(void)
{
    hal_wdg_init();
    hal_clk_init();
    hal_gpio_init();
    hal_led_init();
#if HAL_RTC
    hal_rtc_init();
#endif
    hal_platform_init();
    return 1;
}

