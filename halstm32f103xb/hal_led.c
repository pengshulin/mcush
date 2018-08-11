#include "hal.h"

#ifndef HAL_LED_NUM
    #define HAL_LED_NUM  0
#endif

const uint8_t led_num = HAL_LED_NUM;
#if HAL_LED_NUM
const uint8_t led_ports[] = HAL_LED_PORTS;
const uint8_t led_pins[] = HAL_LED_PINS;
#endif

void hal_led_init(void)
{
#if HAL_LED_NUM
    int i;

    for( i=0; i<led_num; i++ )
    {
        hal_gpio_set_output( led_ports[i], 1<<led_pins[i] );
#if defined(HAL_LED_REV)
        hal_gpio_set( led_ports[i], 1<<led_pins[i]);
#else
        hal_gpio_clr( led_ports[i], 1<<led_pins[i]);
#endif
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
#if defined(HAL_LED_REV)
    hal_gpio_clr( led_ports[index], 1<<led_pins[index]);
#else
    hal_gpio_set( led_ports[index], 1<<led_pins[index]);
#endif
#endif
}

void hal_led_clr(int index)
{
#if HAL_LED_NUM
#if defined(HAL_LED_REV)
    hal_gpio_set( led_ports[index], 1<<led_pins[index]);
#else
    hal_gpio_clr( led_ports[index], 1<<led_pins[index]);
#endif
#endif
}

void hal_led_toggle(int index)
{
#if HAL_LED_NUM
    hal_gpio_toggle( led_ports[index], 1<<led_pins[index]);
#endif
}

int hal_led_get(int index)
{
#if HAL_LED_NUM
    return hal_gpio_get( led_ports[index], 1<<led_pins[index]) ? 
#if defined(HAL_LED_REV)
            0 : 1;
#else
            1 : 0;
#endif
#else
    return 0;
#endif
}


