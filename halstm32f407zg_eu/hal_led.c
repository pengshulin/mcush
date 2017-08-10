#include "hal.h"

#ifndef HAL_LED_NUM
    #define HAL_LED_NUM  0
#endif

const uint8_t led_num = HAL_LED_NUM;
#if HAL_LED_NUM
const GPIO_TypeDef * const led_ports[] = HAL_LED_PORTS;
const uint16_t led_pins[] = HAL_LED_PINS;
#endif



void hal_led_init(void)
{
#if HAL_LED_NUM
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    for( i=0; i<led_num; i++ )
    {
        GPIO_InitStructure.GPIO_Pin = led_pins[i];
        GPIO_Init((GPIO_TypeDef *)led_ports[i], &GPIO_InitStructure);
        GPIO_SetBits((GPIO_TypeDef *)led_ports[i], led_pins[i]);
    }
#endif
}

int hal_led_get_num(void)
{
    return led_num;
}

void hal_led_set(int index)
{
#if HAL_LED_NUM
    GPIO_ResetBits((GPIO_TypeDef *)led_ports[index], led_pins[index]);
#endif
}

void hal_led_clr(int index)
{
#if HAL_LED_NUM
    GPIO_SetBits((GPIO_TypeDef *)led_ports[index], led_pins[index]);
#endif
}

void hal_led_toggle(int index)
{
#if HAL_LED_NUM
    GPIO_WriteBit((GPIO_TypeDef *)led_ports[index], led_pins[index], \
            GPIO_ReadOutputDataBit((GPIO_TypeDef *)led_ports[index], led_pins[index]) ? 0 : 1);
#endif
}

int hal_led_get(int index)
{
#if HAL_LED_NUM
    return GPIO_ReadOutputDataBit((GPIO_TypeDef *)led_ports[index], led_pins[index]) ? 0 : 1;
#else
    return 0;
#endif
}


