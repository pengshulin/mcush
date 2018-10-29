#include "hal.h"


const unsigned int baudrate=9600;

 
int hal_init(void)
{
    hal_wdg_init();
    hal_clk_init();
#if HAL_REBOOT_COUNTER
    hal_reboot_counter_init();
#endif
    hal_gpio_init();
    hal_led_init();
#if HAL_RTC
    hal_rtc_init();
#endif
#if USE_CMD_BEEP
    hal_beep_init();
#endif
#if USE_CMD_SGPIO
    hal_sgpio_init();
#endif
    hal_platform_init();
    if( !hal_uart_init(baudrate) )
        return 0;
    return 1;
}



