/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
/* NOTE:
   ST/F1xx/LL_GPIO driver shift the input bits mask with 8 bits left, 
   that's different from the F4xx/LL driver and F10x/STD driver.
   (set LL_GPIO_PIN_XX defination for details)
   So, I gave up to use the initializing function, 
   but I still prefer the set/clr/toggle/get functions for their efficiency.
*/


const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD
#if defined(GPIOE)
    ,GPIOE
#if defined(GPIOF)
    ,GPIOF
#if defined(GPIOG)
    ,GPIOG
#endif
#endif
#endif
    };

const uint8_t port_num = sizeof(ports)/sizeof(const GPIO_TypeDef *);

static void _set_dir( int port, int bits, int mode, int pull )
{
    GPIO_InitTypeDef gpio_init;
    
    gpio_init.Mode = mode;
    gpio_init.Pull = pull;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pin = bits;
    HAL_GPIO_Init((GPIO_TypeDef *)ports[port], &gpio_init);
}


int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    LL_APB2_GRP1_EnableClock(
        LL_APB2_GRP1_PERIPH_GPIOA  
        | LL_APB2_GRP1_PERIPH_GPIOB 
        | LL_APB2_GRP1_PERIPH_GPIOC 
        | LL_APB2_GRP1_PERIPH_GPIOD 
#if defined(GPIOE)
        | LL_APB2_GRP1_PERIPH_GPIOE
#if defined(GPIOF)
        | LL_APB2_GRP1_PERIPH_GPIOF
#if defined(GPIOG)
        | LL_APB2_GRP1_PERIPH_GPIOG
#endif
#endif
#endif
    );
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, GPIO_MODE_INPUT, GPIO_PULLDOWN );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, GPIO_MODE_INPUT, pull==0?GPIO_NOPULL:(pull>0?GPIO_PULLUP:GPIO_PULLDOWN) );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL );
}


void hal_gpio_set_output_open_drain(int port, int bits)
{
    _set_dir( port, bits, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP );
}


void hal_gpio_set(int port, int bits)
{
    LL_GPIO_SetOutputPin((GPIO_TypeDef *)ports[port], bits<<8);
}


void hal_gpio_clr(int port, int bits)
{
    LL_GPIO_ResetOutputPin((GPIO_TypeDef *)ports[port], bits<<8);
}


void hal_gpio_toggle(int port, int bits)
{
    LL_GPIO_TogglePin((GPIO_TypeDef *)ports[port], bits<<8);
}


int hal_gpio_get(int port, int bits)
{
    return LL_GPIO_ReadInputPort((GPIO_TypeDef *)ports[port]) & bits;
}

