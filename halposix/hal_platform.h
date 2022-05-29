#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_NUM  5
#define HAL_LED_PORTS  { 1, 1, 1, 1, 1 }
#define HAL_LED_PINS  { 0, 1, 2, 3, 4 }

#define HAL_WS2812_PORT  0
#define HAL_WS2812_PIN   0

#include "hal_daq.h"

int hal_stepper_motor_init( int id );
void hal_stepper_motor_set_free( int id, int free );
void hal_stepper_motor_set_dir( int id, int dir );
int hal_stepper_motor_set_clk_freq( int id, float freq );
int hal_stepper_motor_border_check( int id, int direction );

/* alarm lamp */
void hal_set_alarm_pwr(int on);
void hal_set_alarm(int on);
void hal_set_lamp(int r, int g, int b);
int hal_set_lamp_calib(int cal);
void hal_set_led_pwr(int on);



#endif
