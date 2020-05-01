#include "hal.h"

const uint8_t port_num = 9;
const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI };


static void _port_init( int port, int bits, GPIOMode_TypeDef mode, GPIOOType_TypeDef otype, GPIOPuPd_TypeDef pull )
{
    GPIO_InitTypeDef init;
    int i;
        
    GPIO_StructInit(&init);
    init.GPIO_Mode = mode;
    init.GPIO_OType = otype;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_PuPd = pull;
    for( i=0; i<32; i++ )
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
    RCC_AHB1PeriphClockCmd( \
        RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
        RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
        RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | \
        RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH | \
        RCC_AHB1Periph_GPIOI, ENABLE );
}


void hal_gpio_set_input(int port, int bits)
{
    _port_init( port, bits, GPIO_Mode_IN, 0, GPIO_PuPd_NOPULL );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _port_init( port, bits, GPIO_Mode_IN, 0, pull ? GPIO_PuPd_UP : GPIO_PuPd_DOWN );
}


void hal_gpio_set_output(int port, int bits)
{
    _port_init( port, bits, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL );
}


void hal_gpio_set_output_open_drain(int port, int bits)
{
    _port_init( port, bits, GPIO_Mode_OUT, GPIO_OType_OD, GPIO_PuPd_UP );
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



