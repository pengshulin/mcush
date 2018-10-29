#include "hal.h"

static uint8_t _power_set;

int hal_is_power_set(void)
{
    return _power_set;
}

void hal_power_set(int enable)
{
    if( enable )
    {
    	hal_gpio_set_output( HAL_POWER_PORT, 1<<HAL_POWER_PIN );
        hal_gpio_clr( HAL_POWER_PORT, 1<<HAL_POWER_PIN );
        _power_set = 1;
    }
    else
    {
        hal_gpio_set_input( HAL_POWER_PORT, 1<<HAL_POWER_PIN );
        _power_set = 0;
    }
}
