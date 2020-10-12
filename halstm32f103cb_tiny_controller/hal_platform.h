#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#if HAL_CAN
    #define ERRNO_LED_NORMAL  0
    #define ERRNO_LED_ERROR   1
    #define HAL_LED_NUM   2
    #define HAL_LED_PORTS  { 1, 1 }
    #define HAL_LED_PINS  { 10, 11 }
#else
    #define ERRNO_LED_NORMAL  0
    #define ERRNO_LED_ERROR   2
    #define HAL_LED_NUM   4
    #define HAL_LED_PORTS  { 1, 1, 1, 1 }
    #define HAL_LED_PINS  { 8, 10, 9, 11 }
#endif


#ifndef USE_CMD_BEEP
#define USE_CMD_BEEP  1
#define PORT_BEEP       GPIOB
#define PIN_BEEP        GPIO_Pin_0
#endif

#ifndef USE_CMD_SGPIO
#define USE_CMD_SGPIO  1
#endif



void hal_pwm_init( int freq, int range, int value );
void hal_pwm_deinit( void );
void hal_pwm_set( int index, int value );
int hal_pwm_get( int index );
int hal_pwm_get_num( void );


void hal_adc_init( void );
void hal_adc_deinit( void );
int hal_adc_get_num( void );
float hal_adc_get( int index );


#define USE_HAL_USB_DISCONNECT  0

#define USE_CMD_POWER   0
void hal_power_set(int enable);
int hal_is_power_set(void);

//#define HAL_RESET_USB_PINS  1


#include "flash_config.h"


#endif
