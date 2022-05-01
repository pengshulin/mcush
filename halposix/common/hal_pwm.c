#include "hal.h"

#ifndef HAL_PWM_NUM
    #define HAL_PWM_NUM                 8
#endif
    
#ifndef HAL_PWM_FREQ_DEF
    #define HAL_PWM_FREQ_DEF            1000
#endif

#ifndef HAL_PWM_RANGE_DEF
    #define HAL_PWM_RANGE_DEF           100
#endif
 
#ifndef HAL_PWM_VALUE_DEF
    #define HAL_PWM_VALUE_DEF           50
#endif

#if HAL_PWM_VALUE_DEF > HAL_PWM_RANGE_DEF
    #error "HAL_PWM_VALUE_DEF larger than HAL_PWM_RANGE_DEF"
#endif


extern uint32_t SystemCoreClock;

static short _pwm_range = HAL_PWM_RANGE_DEF;
static short _pwm_value[HAL_PWM_NUM];



void hal_pwm_init( int freq, int range, int value )
{
    if( freq == 0 )
        freq = HAL_PWM_FREQ_DEF;
    if( range <= 0 )
        range = HAL_PWM_RANGE_DEF;
    if( value < 0 )
        value = HAL_PWM_VALUE_DEF;
    _pwm_range = range;

}

 
void hal_pwm_deinit( void )
{
}


void hal_pwm_set( int index, int value )
{
    _pwm_value[index] = value;
}


int hal_pwm_get( int index )
{
    return _pwm_value[index];
}


int hal_pwm_get_num( void )
{
    return HAL_PWM_NUM;
}

