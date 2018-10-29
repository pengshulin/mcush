#ifndef __HAL_OW_H__
#define __HAL_OW_H__
#include "hal.h"

#define OW_GPIO GPIOA
#define OW_PIN 2

#define hal_ow_init()  { \
    GPIO_InitTypeDef gpio_init; \
    gpio_init.GPIO_Pin = (1 << OW_PIN); \
    gpio_init.GPIO_Mode = GPIO_Mode_OUT; \
    gpio_init.GPIO_OType = GPIO_OType_PP; \
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP; \
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz; \
    GPIO_Init(OW_GPIO, &gpio_init); }

 
/**
 * OW_OUTPUT - set one-wire gpio pin to output
 * OW_INPUT - set one-wire gpio pin to input
 */
#define OW_OUTPUT() ( OW_GPIO->MODER = ( ( OW_GPIO->MODER & ~( 3 << (2*OW_PIN) ) ) | ( 0x1 << (2*OW_PIN) ) ) )
#define OW_INPUT() ( OW_GPIO->MODER = ( OW_GPIO->MODER & ~( 3 << (2*OW_PIN) ) ) )

/**
 * OW_HIGH - drive one-wire gpio pin high
 * OW_LOW - drive one-wire gpio pin low
 */
#define OW_HIGH() ( OW_GPIO->BSRRL = 1 << OW_PIN )
#define OW_LOW() ( OW_GPIO->BSRRH = 1 << OW_PIN )

/**
 * OW_READ - read state of one-wire gpio pin
 */
#define OW_READ() ( ( OW_GPIO->IDR >> OW_PIN ) & 1 )

#define __delay_us(us)  hal_delay_us((us))

#endif
