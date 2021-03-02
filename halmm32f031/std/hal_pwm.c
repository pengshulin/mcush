/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PB4  TIMER3_1 ------> PWM0
   PB5  TIMER3_2 ------> PWM1
   PB0  TIMER3_3 ------> PWM2
   PB1  TIMER3_4 ------> PWM3
   ----------------------------- 
 */

#ifndef HAL_PWM_DEFINE 
    #define HAL_PWM_RCC_GPIO_ENABLE_CMD    RCC_AHBPeriphClockCmd
    #define HAL_PWM_RCC_GPIO_ENABLE_BIT    RCC_AHBPeriph_GPIOB
    #define HAL_PWM_RCC_TIMER_ENABLE_CMD   RCC_APB1PeriphClockCmd
    #define HAL_PWM_RCC_TIMER_ENABLE_BIT   RCC_APB1Periph_TIM3
    #define HAL_PWM_NUM                    4
    #define HAL_PWM_PORTS                  {1, 1, 1, 1}
    #define HAL_PWM_PINS                   {4, 5, 0, 1} 
    #define HAL_PWM_PINSRC                 {GPIO_PinSource4, GPIO_PinSource5, GPIO_PinSource0, GPIO_PinSource1}
    #define HAL_PWM_AF                     GPIO_AF_1
    #define HAL_PWM_TIMER                  TIM3
    #define HAL_PWM_TIMER_OC_INIT_CMD      {TIM_OC1Init, TIM_OC2Init, TIM_OC3Init, TIM_OC4Init}
    #define HAL_PWM_TIMER_OC_SETCMP_CMD    {TIM_SetCompare1, TIM_SetCompare2, TIM_SetCompare3, TIM_SetCompare4}
#endif

    
#ifndef HAL_PWM_FREQ_DEF
    #define HAL_PWM_FREQ_DEF               1000
#endif

#ifndef HAL_PWM_RANGE_DEF
    #define HAL_PWM_RANGE_DEF              100
#endif
 
#ifndef HAL_PWM_VALUE_DEF
    #define HAL_PWM_VALUE_DEF              50
#endif

#if HAL_PWM_VALUE_DEF > HAL_PWM_RANGE_DEF
    #error "HAL_PWM_VALUE_DEF larger than HAL_PWM_RANGE_DEF"
#endif


extern uint32_t SystemCoreClock;
extern const GPIO_TypeDef * const ports[];

static const uint8_t _pwm_ports[] = HAL_PWM_PORTS;
static const uint8_t _pwm_pins[] = HAL_PWM_PINS;
static const uint16_t _pwm_pinsrc[] = HAL_PWM_PINSRC;

static void (*_pwm_oc_init[HAL_PWM_NUM])(TIM_TypeDef*, TIM_OCInitTypeDef*) = HAL_PWM_TIMER_OC_INIT_CMD;
static void (*_pwm_oc_setcmp[HAL_PWM_NUM])(TIM_TypeDef*, uint32_t) = HAL_PWM_TIMER_OC_SETCMP_CMD;

//static short _pwm_freq = HAL_PWM_FREQ_DEF;
static short _pwm_range = HAL_PWM_RANGE_DEF;
static short _pwm_value[HAL_PWM_NUM];



void hal_pwm_init( int freq, int range, int value )
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef timbase;
    TIM_OCInitTypeDef timoc;
    int i;

    // clock
    HAL_PWM_RCC_GPIO_ENABLE_CMD( HAL_PWM_RCC_GPIO_ENABLE_BIT, ENABLE );
    HAL_PWM_RCC_TIMER_ENABLE_CMD( HAL_PWM_RCC_TIMER_ENABLE_BIT, ENABLE );

    // alternative output port
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    for( i=0; i<HAL_PWM_NUM; i++ )
    {
        gpio_init.GPIO_Pin = 1<<(_pwm_pins[i]);
        GPIO_Init( (GPIO_TypeDef*)ports[_pwm_ports[i]], &gpio_init );
        GPIO_PinAFConfig( (GPIO_TypeDef*)ports[_pwm_ports[i]], _pwm_pinsrc[i], HAL_PWM_AF );
    }

    // generate tick by timer
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    if( freq == 0 )
        freq = HAL_PWM_FREQ_DEF;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = 0;
    TIM_TimeBaseInit( HAL_PWM_TIMER, &timbase );
    
    TIM_OCStructInit( &timoc );  
    timoc.TIM_OCMode = TIM_OCMode_PWM1;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    for( i=0; i<HAL_PWM_NUM; i++ )
    {
        (*_pwm_oc_init[i])( HAL_PWM_TIMER, &timoc );
    }
    TIM_Cmd( HAL_PWM_TIMER, ENABLE );
        
    if( range <= 0 )
        range = HAL_PWM_RANGE_DEF;
    _pwm_range = range;
    TIM_SetCounter( HAL_PWM_TIMER, 0 ); 
    TIM_SetAutoreload( HAL_PWM_TIMER, _pwm_range - 1 ); 
    TIM_PrescalerConfig( HAL_PWM_TIMER, SystemCoreClock/(freq*range)-1, TIM_PSCReloadMode_Immediate );

    if( value < 0 )
        value = HAL_PWM_VALUE_DEF;
    for( i=0; i<HAL_PWM_NUM; i++ )
    {
        (*_pwm_oc_setcmp[i])( HAL_PWM_TIMER, value );
        _pwm_value[i] = value;
    }
}

 
void hal_pwm_deinit( void )
{
    int i;
    for( i=0; i<HAL_PWM_NUM; i++ )
        hal_gpio_set_input( _pwm_ports[i], 1<<(_pwm_pins[i]), 0 );
}


void hal_pwm_set( int index, int value )
{
    (*_pwm_oc_setcmp[index])( HAL_PWM_TIMER, value );
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
