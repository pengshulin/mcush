/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"


const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD };

const uint8_t port_num = sizeof(ports)/sizeof(const GPIO_TypeDef *);


static void _set_mode( int port, int bits, int mode, int otype, int pull, int alternate )
{
    LL_GPIO_InitTypeDef gpio_init;
    //int i;
        
    gpio_init.Mode = mode;
    gpio_init.OutputType = otype;
    gpio_init.Pull = pull;
    gpio_init.Alternate = alternate;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pin = bits;
    LL_GPIO_Init((GPIO_TypeDef *)ports[port], &gpio_init);
    ///for( i=0; i<16; i++ )
    //{
    //    if( bits & (1<<i) )
    //    { 
    //        gpio_init.Pin = 1<<i;
    //        LL_GPIO_Init((GPIO_TypeDef *)ports[port], &gpio_init);
    //    }
    //}
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    LL_IOP_GRP1_EnableClock( LL_IOP_GRP1_PERIPH_GPIOA | LL_IOP_GRP1_PERIPH_GPIOB | LL_IOP_GRP1_PERIPH_GPIOC | LL_IOP_GRP1_PERIPH_GPIOD);
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    _set_mode( port, bits, LL_GPIO_MODE_INPUT, 0, pull==0?LL_GPIO_PULL_NO:(pull>0?LL_GPIO_PULL_UP:LL_GPIO_PULL_DOWN), 0 );
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    if( open_drain )
        _set_mode( port, bits, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, 0 );
    else
        _set_mode( port, bits, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_NO, 0 );
}


void hal_gpio_set(int port, int bits)
{
    LL_GPIO_SetOutputPin((GPIO_TypeDef *)ports[port], bits);
}


void hal_gpio_clr(int port, int bits)
{
    LL_GPIO_ResetOutputPin((GPIO_TypeDef *)ports[port], bits);
}


void hal_gpio_toggle(int port, int bits)
{
    int reset, set;
        
    reset = LL_GPIO_ReadOutputPort((GPIO_TypeDef *)ports[port]) & bits;
    set = ~reset & bits;
    
    if( set )
        LL_GPIO_SetOutputPin((GPIO_TypeDef *)ports[port], set);
    if( reset )
        LL_GPIO_ResetOutputPin((GPIO_TypeDef *)ports[port], reset);
}


int hal_gpio_get(int port, int bits)
{
    return LL_GPIO_ReadInputPort((GPIO_TypeDef *)ports[port]) & bits;
}

