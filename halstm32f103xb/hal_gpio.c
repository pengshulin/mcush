#include "hal.h"

const char gpio_start='a', gpio_stop='g';

const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG };



static void _set_dir( int port, int bits, GPIOMode_TypeDef mode )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;
        
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = mode;
    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
        { 
            GPIO_InitStructure.GPIO_Pin = 1<<i;
            GPIO_Init((GPIO_TypeDef*)ports[port], &GPIO_InitStructure);
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return gpio_stop - gpio_start + 1;
}


void hal_gpio_init(void)
{
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, ENABLE);
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_IPU );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, pull ? GPIO_Mode_IPU : GPIO_Mode_IPD );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_Out_PP );
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
    GPIO_Write((GPIO_TypeDef*)ports[port], \
        GPIO_ReadInputData((GPIO_TypeDef*)ports[port]) ^ bits);
}


int hal_gpio_get(int port, int bits)
{
    return GPIO_ReadInputData((GPIO_TypeDef*)ports[port]) & bits;
}



