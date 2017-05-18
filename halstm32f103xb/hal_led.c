#include "hal.h"
#include "hal_led.def"


void hal_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    for( i=0; i<led_num; i++ )
    {
        GPIO_InitStructure.GPIO_Pin = led_pins[i];
        GPIO_Init((GPIO_TypeDef*)led_ports[i], &GPIO_InitStructure);
#if defined(LED_REV)
        GPIO_SetBits((GPIO_TypeDef*)led_ports[i], led_pins[i]);
#else
        GPIO_ResetBits((GPIO_TypeDef*)led_ports[i], led_pins[i]);
#endif
    }
}

int hal_led_get_num(void)
{
    return led_num;
}

void hal_led_set(int index)
{
#if defined(LED_REV)
    GPIO_ResetBits((GPIO_TypeDef*)led_ports[index], led_pins[index]);
#else
    GPIO_SetBits((GPIO_TypeDef*)led_ports[index], led_pins[index]);
#endif
}

void hal_led_clr(int index)
{
#if defined(LED_REV)
    GPIO_SetBits((GPIO_TypeDef*)led_ports[index], led_pins[index]);
#else
    GPIO_ResetBits((GPIO_TypeDef*)led_ports[index], led_pins[index]);
#endif
}

void hal_led_toggle(int index)
{
    GPIO_WriteBit((GPIO_TypeDef*)led_ports[index], led_pins[index], \
            GPIO_ReadOutputDataBit((GPIO_TypeDef*)led_ports[index], led_pins[index]) ? 0 : 1);
}

int hal_led_get(int index)
{
    return GPIO_ReadOutputDataBit((GPIO_TypeDef*)led_ports[index], led_pins[index]) ?
#if defined(LED_REV)
            0 : 1;
#else
            1 : 0;
#endif
}


