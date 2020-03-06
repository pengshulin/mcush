/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

#ifndef HAL_KEY_NUM
    #define HAL_KEY_NUM  0
#endif

#if HAL_KEY_NUM
const uint8_t key_num = HAL_KEY_NUM;
const uint8_t key_ports[] = HAL_KEY_PORTS;
const uint8_t key_pins[] = HAL_KEY_PINS;
#endif


int hal_key_get_num(void)
{
#if HAL_KEY_NUM
    return key_num;
#else
    return 0;
#endif
}


int hal_key_get(int mask)
{
#if HAL_KEY_NUM
    int i, m, ret=0;

    for( i=0, m=1; i<key_num; i++, m<<=1 )
    {
#if defined(HAL_KEY_REV)
        if( (mask & m) && (hal_gpio_get( key_ports[i], 1<<key_pins[i] )==0) )
#else
        if( (mask & m) && hal_gpio_get( key_ports[i], 1<<key_pins[i] ) )
#endif
            ret |= m;
    }
    return ret;
#else
    return 0;
#endif
}


