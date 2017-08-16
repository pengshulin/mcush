#include "hal.h"

const uint8_t port_num = 8;


static void _set_dir( int port, int bits, int mode )
{
    if( mode )
        Chip_GPIO_SetPortDIROutput( LPC_GPIO_PORT, port, bits );
    else
        Chip_GPIO_SetPortDIRInput( LPC_GPIO_PORT, port, bits );
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, 0 );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, 0 );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, 1 );
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
    Chip_GPIO_SetPinToggle( LPC_GPIO_PORT, port, bits );
}


int hal_gpio_get(int port, int bits)
{
    return Chip_GPIO_ReadPortBit( LPC_GPIO_PORT, port, bits );
}



