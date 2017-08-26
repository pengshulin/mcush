#ifndef __HAL_OW_H__
#define __HAL_OW_H__
#include "hal.h"

#ifndef OW_GPIO
    #define OW_GPIO  GPIOC
    #define OW_PIN  0
#endif

#define hal_ow_init()  {\
    GPIO_InitTypeDef gpio_init; \
    gpio_init.GPIO_Pin = (1 << OW_PIN); \
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP; \
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz; \
    GPIO_Init(OW_GPIO, &gpio_init); }


/**
 * OW_OUTPUT - set one-wire gpio pin to output
 * OW_INPUT - set one-wire gpio pin to input
 */
#if OW_PIN > 7
  #define OW_OUTPUT() ( OW_GPIO->CRH = ( ( OW_GPIO->CRH & ~( 0xf << (4*(OW_PIN-8)) ) ) | ( 0x7 << (4*(OW_PIN-8)) ) ) )
  #define OW_INPUT()  ( OW_GPIO->CRH = ( ( OW_GPIO->CRH & ~( 0xf << (4*(OW_PIN-8)) ) ) | ( 0x4 << (4*(OW_PIN-8)) ) ) )
#else
  #define OW_OUTPUT() ( OW_GPIO->CRL = ( ( OW_GPIO->CRL & ~( 0xf << (4*OW_PIN) ) ) | ( 0x7 << (4*OW_PIN) ) ) )
  #define OW_INPUT()  ( OW_GPIO->CRL = ( ( OW_GPIO->CRL & ~( 0xf << (4*OW_PIN) ) ) | ( 0x4 << (4*OW_PIN) ) ) )
#endif

/**
 * OW_HIGH - drive one-wire gpio pin high
 * OW_LOW - drive one-wire gpio pin low
 */
#define OW_HIGH() ( OW_GPIO->BSRR = 1 << OW_PIN )
#define OW_LOW() ( OW_GPIO->BRR = 1 << OW_PIN )
//#define OW_HIGH()  GPIO_SetBits(OW_GPIO, 1<<OW_PIN)  
//#define OW_LOW()   GPIO_ResetBits(OW_GPIO, 1<<OW_PIN) 


/**
 * OW_READ - read state of one-wire gpio pin
 */
#define OW_READ() ( ( OW_GPIO->IDR >> OW_PIN ) & 1 )
//#define OW_READ() GPIO_ReadInputDataBit(OW_GPIO, 1<<OW_PIN)?1:0

#define __delay_us(us)  hal_delay_us((us))

#endif
