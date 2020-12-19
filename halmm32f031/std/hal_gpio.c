/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

const uint8_t port_num = 5;
const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE };



static void _set_mode( int port, int bits, GPIOMode_TypeDef mode )
{
    GPIO_InitTypeDef init;
    int i;
        
    init.GPIO_Mode = mode;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    for( i=0; i<16; i++ )
    {
        if( bits & (1<<i) )
        { 
            init.GPIO_Pin = 1<<i;
            GPIO_Init((GPIO_TypeDef*)ports[port], &init);
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
        RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE, ENABLE);
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    _set_mode( port, bits, pull==0 ? GPIO_Mode_FLOATING : (pull>0 ? GPIO_Mode_IPU : GPIO_Mode_IPD) );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    if( open_drain )
        _set_mode( port, bits, GPIO_Mode_Out_OD );
    else
        _set_mode( port, bits, GPIO_Mode_Out_PP );
}


void hal_gpio_set(int port, int bits)
{
    GPIO_SetBits((GPIO_TypeDef*)ports[port], bits);
}


void hal_gpio_clr(int port, int bits)
{
    GPIO_ResetBits((GPIO_TypeDef*)ports[port], bits);
}


void hal_gpio_toggle(int port, int bits)
{
    int reset, set;
        
    reset = GPIO_ReadOutputData((GPIO_TypeDef*)ports[port]) & bits;
    set = ~reset & bits;
    
    if( set )
        GPIO_SetBits((GPIO_TypeDef*)ports[port], set);
    if( reset )
        GPIO_ResetBits((GPIO_TypeDef*)ports[port], reset);
}


int hal_gpio_get(int port, int bits)
{
    return GPIO_ReadInputData((GPIO_TypeDef*)ports[port]) & bits;
}



