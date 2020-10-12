/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK };
const uint8_t port_num = sizeof(ports)/sizeof(const GPIO_TypeDef *);



static void _set_mode( int port, int bits, GPIOMode_TypeDef mode, GPIOPuPd_TypeDef pull, GPIOOType_TypeDef output_type )
{
    GPIO_InitTypeDef gpio_init;
        
    gpio_init.GPIO_Mode = mode;
    gpio_init.GPIO_PuPd = pull;
    gpio_init.GPIO_OType = output_type;
    gpio_init.GPIO_Speed = GPIO_High_Speed;
    gpio_init.GPIO_Pin = bits & 0xFFFF;
    GPIO_Init((GPIO_TypeDef *)ports[port], &gpio_init);
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    RCC_AHB1PeriphClockCmd( \
        RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
        RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
        RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | \
        RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | \
        RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOJ | \
        RCC_AHB1Periph_GPIOK, ENABLE );
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    _set_mode( port, bits, GPIO_Mode_IN, 
               pull==0 ? GPIO_PuPd_NOPULL : (pull>0? GPIO_PuPd_UP : GPIO_PuPd_DOWN),
               GPIO_OType_OD );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    if( open_drain )
        _set_mode( port, bits, GPIO_Mode_OUT, GPIO_PuPd_UP, GPIO_OType_OD );
    else
        _set_mode( port, bits, GPIO_Mode_OUT, GPIO_PuPd_NOPULL, GPIO_OType_PP );
}


void hal_gpio_set(int port, int bits)
{
    GPIO_SetBits((GPIO_TypeDef *)ports[port], bits);
}


void hal_gpio_clr(int port, int bits)
{
    GPIO_ResetBits((GPIO_TypeDef *)ports[port], bits);
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
    return GPIO_ReadInputData((GPIO_TypeDef *)ports[port]) & bits;
}



