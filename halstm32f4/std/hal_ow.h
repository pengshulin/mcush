#ifndef __HAL_OW_H__
#define __HAL_OW_H__
#include "hal.h"

#ifndef OW_GPIO
    #error "OW_GPIO not defined"
#endif
#ifndef OW_PIN
    #error "OW_PIN not defined"
#endif

#define hal_ow_init()   {hal_gpio_set_output_open_drain( OW_GPIO, 1<<OW_PIN ); \
                         hal_gpio_set( OW_GPIO, 1<<OW_PIN );}


/**
 * OW_OUTPUT - set one-wire gpio pin to output
 * OW_INPUT - set one-wire gpio pin to input
 */
#define OW_OUTPUT()     
#define OW_INPUT()      hal_gpio_set( OW_GPIO, 1<<OW_PIN )

/**
 * OW_HIGH - drive one-wire gpio pin high
 * OW_LOW - drive one-wire gpio pin low
 */
#define OW_HIGH()       hal_gpio_set( OW_GPIO, 1<<OW_PIN )
#define OW_LOW()        hal_gpio_clr( OW_GPIO, 1<<OW_PIN )

/**
 * OW_READ - read state of one-wire gpio pin
 */
#define OW_READ()       hal_gpio_get( OW_GPIO, 1<<OW_PIN ) ? 1 : 0

#define __delay_us(us)  hal_delay_us((us))

#endif
