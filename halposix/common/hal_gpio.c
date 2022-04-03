/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include <stdio.h>

#ifndef PORT_NUM
    #define PORT_NUM  10
#endif

int port_mode[PORT_NUM];
int port_set[PORT_NUM];
int port_otype[PORT_NUM];
int port_pullup[PORT_NUM];
int port_pulldown[PORT_NUM];



static void _set_mode( int port, int bits, int mode, int otype, int pull )
{
    int i;

    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
        {
            printf("port %d, bits 0x%04X, mode %d, otype %d, pull %d\n", port, bits, mode, otype, pull);
            if( mode )
                port_mode[port] |= 1>>i;
            else
                port_mode[port] &= ~(1>>i);
            if( otype )
                port_otype[port] |= 1>>i;
            else
                port_otype[port] &= ~(1>>i);
            if( pull > 0 )
            {
                port_pullup[port] |= 1>>i;
                port_pulldown[port] &= ~(1>>i);
            }
            else if( pull < 0 )
            {
                port_pullup[port] &= ~(1>>i);
                port_pulldown[port] |= 1>>i;
            }
            else
            {
                port_pullup[port] &= ~(1>>i);
                port_pulldown[port] &= ~(1>>i);
            }
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return PORT_NUM;
}


void hal_gpio_init(void)
{
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    _set_mode( port, bits, 0, 0, pull );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    if( open_drain )
        _set_mode( port, bits, 1, 1, 1 );
    else
        _set_mode( port, bits, 1, 0, 0 );
}


void hal_gpio_set(int port, int bits)
{
    port_set[port] |= bits;
    printf("port %d, set 0x%04X\n", port, bits);
 
}


void hal_gpio_clr(int port, int bits)
{
    port_set[port] &= ~bits;
    printf("port %d, clr 0x%04X\n", port, bits);
}


void hal_gpio_toggle(int port, int bits)
{
    port_set[port] ^= bits;
    printf("port %d, toggle 0x%04X\n", port, bits);
}


int hal_gpio_get(int port, int bits)
{
    return port_set[port] & bits;
}


