#include "hal.h"

const char gpio_start='a', gpio_stop='f';

GPIO_TypeDef *ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF };



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
            GPIO_Init(ports[port], &GPIO_InitStructure);
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return gpio_stop - gpio_start + 1;
}


void hal_gpio_init(void)
{
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOF, ENABLE);
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_IN );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, pull ? GPIO_Mode_IN : GPIO_Mode_IN );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_OUT );
}


void hal_gpio_set(int port, int bits)
{
    GPIO_SetBits(ports[port], bits);
}


void hal_gpio_clr(int port, int bits)
{
    GPIO_ResetBits(ports[port], bits);
}


void hal_gpio_toggle(int port, int bits)
{
    GPIO_Write(ports[port], GPIO_ReadInputData(ports[port]) ^ bits);
}


int hal_gpio_get(int port, int bits)
{
    return GPIO_ReadInputData(ports[port]) & bits;
}



