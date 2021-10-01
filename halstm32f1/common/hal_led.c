/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

#ifndef HAL_LED_NUM
    #define HAL_LED_NUM  0
#endif

static const uint8_t led_num = HAL_LED_NUM;
#if HAL_LED_NUM
    static const uint8_t led_ports[] = HAL_LED_PORTS;
    static const uint8_t led_pins[] = HAL_LED_PINS;
    #if HAL_LED_NUM <= 8
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 16
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 32
        typedef uint8_t led_bitmap_t;
    #elif HAL_LED_NUM <= 64
        typedef uint8_t led_bitmap_t;
    #else
        #error "HAL_LED_NUM too large"
    #endif
    static led_bitmap_t led_bitmap;
#endif
    

void hal_led_init(void)
{
#if HAL_LED_NUM
    int i;

    for( i=0; i<led_num; i++ )
    {
        hal_gpio_set_output( led_ports[i], 1<<led_pins[i], 0 );
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
        led_bitmap |= 1<<index;
#else
    (void)index;
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
        led_bitmap &= ~(1<<index);
#else
    (void)index;
#endif
}

void hal_led_toggle(int index)
{
#if HAL_LED_NUM
    hal_gpio_toggle( led_ports[index], 1<<led_pins[index]);
    led_bitmap ^= 1<<index;
#else
    (void)index;
#endif
}

int hal_led_get(int index)
{
#if HAL_LED_NUM
    return (led_bitmap & (1<<index)) ? 1 : 0;
#else
    (void)index;
    return 0;
#endif
}


void hal_led_update(void)
{
#if HAL_LED_NUM
    int i;
    for( i=0; i<HAL_LED_NUM; i++ )
    {
        if( led_bitmap & (1<<i) )
            hal_gpio_set( led_ports[i], 1<<led_pins[i]);
        else
            hal_gpio_clr( led_ports[i], 1<<led_pins[i]);
    }
#endif
}


