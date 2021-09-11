#include "hal.h"

const uint16_t gpio_clks[] = GPIO_CLOCKS;
const uint8_t port_num = sizeof(gpio_clks)/sizeof(uint16_t);




int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    int i;
    for(i=0;i<port_num;i++)
        CLOCK_EnableClock(gpio_clks[i]);
    CLOCK_EnableClock(kCLOCK_Iocon);
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    /* TODO: support pull_up/down mode */
    (void)pull;
    GPIO->DIR[port] &= ~bits;
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    /* TODO: support open_drain mode */
    (void)open_drain;
    GPIO->DIR[port] |= bits;
}


void hal_gpio_set(int port, int bits)
{
    GPIO->SET[port] = bits;
}


void hal_gpio_clr(int port, int bits)
{
    GPIO->CLR[port] = bits;
}


void hal_gpio_toggle(int port, int bits)
{
    GPIO->NOT[port] = bits;
}


int hal_gpio_get(int port, int bits)
{
    return GPIO->MPIN[port] & bits;
}



