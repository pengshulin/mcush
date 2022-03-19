#include "mcush.h"


int hal_init(void)
{
    hal_clk_init();
    hal_wdg_init();
#if HAL_REBOOT_COUNTER
    hal_reboot_counter_init();
#endif
    hal_gpio_init();
    hal_led_init();
#if HAL_RNG
    hal_rng_init();
#endif
#if HAL_RTC
    hal_rtc_init();
#endif
    hal_platform_init();
    return 1;
}

