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
#if HAL_LCD
    hal_lcd_init();
#endif
#if USE_CMD_SGPIO
    hal_sgpio_init();
#endif
    hal_platform_init();
    if( !hal_uart_init(SHELL_DEFAULT_BAUDRATE) )
        return 0;
    return 1;
}

