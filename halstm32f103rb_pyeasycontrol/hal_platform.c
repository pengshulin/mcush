#include "mcush.h"


/* platform related reset */
void hal_platform_reset(void)
{

}


/* platform related init */
void hal_platform_init(void)
{
    hal_power_set(1);
    hal_gpio_set_output( HAL_USB_EN_PORT, 1<<HAL_USB_EN_PIN );
    hal_gpio_set( HAL_USB_EN_PORT, 1<<HAL_USB_EN_PIN );
}
