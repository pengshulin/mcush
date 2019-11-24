/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"


const GPIO_TypeDef * const ports[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE
#if defined(GPIOF)
    ,GPIOF
#endif
#if defined(GPIOG)
    ,GPIOG
#endif
#if defined(GPIOH)
    ,GPIOH
#endif
#if defined(GPIOI)
    ,GPIOI
#endif
#if defined(GPIOJ)
    ,GPIOJ
#if defined(GPIOK)
    ,GPIOK
#endif
#endif
    };

const uint8_t port_num = sizeof(ports)/sizeof(const GPIO_TypeDef *);


static void _set_dir( int port, int bits, int mode, int otype, int pull, int alternate )
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
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
#if defined(GPIOF)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
#endif
#if defined(GPIOG)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
#endif
#if defined(GPIOH)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
#endif
#if defined(GPIOI)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI);
#endif
#if defined(GPIOJ)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOJ);
#endif
#if defined(GPIOK)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOK);
#endif
}


void hal_gpio_set_input(int port, int bits)
{
    _set_dir( port, bits, LL_GPIO_MODE_INPUT, 0, LL_GPIO_PULL_NO, 0 );
}


void hal_gpio_set_input_pull(int port, int bits, int pull)
{
    _set_dir( port, bits, LL_GPIO_MODE_INPUT, 0, pull==0?LL_GPIO_PULL_NO:(pull>0?LL_GPIO_PULL_UP:LL_GPIO_PULL_DOWN), 0 );
}


void hal_gpio_set_output(int port, int bits)
{
    _set_dir( port, bits, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_PULL_NO, 0 );
}


void hal_gpio_set_output_open_drain(int port, int bits)
{
    _set_dir( port, bits, LL_GPIO_MODE_OUTPUT, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_PULL_UP, 0 );
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
    LL_GPIO_TogglePin((GPIO_TypeDef *)ports[port], bits);
}


int hal_gpio_get(int port, int bits)
{
    return LL_GPIO_ReadInputPort((GPIO_TypeDef *)ports[port]) & bits;
}

