#include "hal.h"

const uint8_t port_num = 11;
const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK };



static void _set_dir( int port, int bits, GPIOMode_TypeDef mode, GPIOPuPd_TypeDef pull, GPIOOType_TypeDef output_type )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;
        
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_PuPd = pull;
    GPIO_InitStructure.GPIO_OType = output_type;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
        { 
            GPIO_InitStructure.GPIO_Pin = 1<<i;
            GPIO_Init((GPIO_TypeDef *)ports[port], &GPIO_InitStructure);
        }
    }
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI, ENABLE);
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_IN, GPIO_PuPd_NOPULL, GPIO_OType_PP );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, GPIO_Mode_IN, pull ? GPIO_PuPd_UP : GPIO_PuPd_DOWN, GPIO_OType_OD );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_OUT, GPIO_PuPd_NOPULL, GPIO_OType_PP );
}


void hal_gpio_set_output_open_drain(int port, int bits)
{
    _set_dir( port, bits, GPIO_Mode_OUT, GPIO_PuPd_UP, GPIO_OType_OD );
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
    GPIO_Write((GPIO_TypeDef *)ports[port], GPIO_ReadInputData((GPIO_TypeDef *)ports[port]) ^ bits);
}


int hal_gpio_get(int port, int bits)
{
    return GPIO_ReadInputData((GPIO_TypeDef *)ports[port]) & bits;
}



