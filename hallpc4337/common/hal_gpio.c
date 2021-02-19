#include "hal.h"

const uint8_t port_num = 8;



int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    /* pull up/down is configured from SCU */
    (void)pull;  /* not available */
    Chip_GPIO_SetDir( LPC_GPIO_PORT, port, bits, 0 );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    (void)open_drain;  /* not available, always push-pull except I2C pins */
    Chip_GPIO_SetDir( LPC_GPIO_PORT, port, bits, 1 );
}


void hal_gpio_set(int port, int bits)
{
    Chip_GPIO_SetPortOutHigh( LPC_GPIO_PORT, port, bits );
}


void hal_gpio_clr(int port, int bits)
{
    Chip_GPIO_SetPortOutLow( LPC_GPIO_PORT, port, bits );
}


void hal_gpio_toggle(int port, int bits)
{
    Chip_GPIO_SetPortToggle( LPC_GPIO_PORT, port, bits );
}


int hal_gpio_get(int port, int bits)
{
    return Chip_GPIO_GetPortValue( LPC_GPIO_PORT, port ) & bits;
}



