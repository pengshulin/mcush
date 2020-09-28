/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include <math.h>


/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PB0  TIMER3_3 ------> BEEPER
   ----------------------------- 
 */

#ifndef HAL_BEEP_DEFINE 
    #define HAL_BEEP_RCC_GPIO_ENABLE_CMD    RCC_APB2PeriphClockCmd
    #define HAL_BEEP_RCC_GPIO_ENABLE_BIT    RCC_APB2Periph_GPIOB
    #define HAL_BEEP_RCC_TIMER_ENABLE_CMD   RCC_APB1PeriphClockCmd
    #define HAL_BEEP_RCC_TIMER_ENABLE_BIT   RCC_APB1Periph_TIM3
    #define HAL_BEEP_TIMER                  TIM3
    #define HAL_BEEP_TIMER_OC_INIT_CMD      TIM_OC3Init
    #define HAL_BEEP_TIMER_OC_SETCMP_CMD    TIM_SetCompare3
    #define HAL_BEEP_PORT                   GPIOB
    #define HAL_BEEP_PIN                    GPIO_Pin_0
    #define HAL_BEEP_PINSRC                 GPIO_PinSource0
    #define HAL_BEEP_AF                     GPIO_AF_TIMER3
#endif

    
#ifndef HAL_BEEPER_FREQ_DEF
    #define HAL_BEEPER_FREQ_DEF             4000
#endif
 

extern uint32_t SystemCoreClock;


void hal_beep_init( void )
{
    GPIO_InitTypeDef gpio_init;
    TIM_TimeBaseInitTypeDef timbase;
    TIM_OCInitTypeDef timoc;

    // clock
    //HAL_BEEP_RCC_GPIO_ENABLE_CMD( HAL_BEEP_RCC_GPIO_ENABLE_BIT, ENABLE );
    HAL_BEEP_RCC_TIMER_ENABLE_CMD( HAL_BEEP_RCC_TIMER_ENABLE_BIT, ENABLE );

    // alternative output port
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin = HAL_BEEP_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( HAL_BEEP_PORT, &gpio_init );

    // generate square wave by timer
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = SystemCoreClock/HAL_BEEPER_FREQ_DEF;
    TIM_TimeBaseInit( HAL_BEEP_TIMER, &timbase );
    
    timoc.TIM_OCMode = TIM_OCMode_PWM1;
    timoc.TIM_Pulse = 0;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    timoc.TIM_OCPolarity = TIM_OCPolarity_High;
    HAL_BEEP_TIMER_OC_INIT_CMD( HAL_BEEP_TIMER, &timoc );
    TIM_Cmd( HAL_BEEP_TIMER, ENABLE );
}

 
static short _freq;
static short _auto_reload_val;

void hal_beep_on( int freq )
{
    short auto_reload_val;

    if( freq != _freq )
    {
        _freq = freq;
        _auto_reload_val = auto_reload_val = (short)sqrtf( SystemCoreClock / freq );
    }
    else
        auto_reload_val = _auto_reload_val;

    TIM_SetCounter( HAL_BEEP_TIMER, 0 ); 
    TIM_SetAutoreload( HAL_BEEP_TIMER, auto_reload_val ); 
    TIM_PrescalerConfig( HAL_BEEP_TIMER, auto_reload_val, TIM_PSCReloadMode_Immediate );
    HAL_BEEP_TIMER_OC_SETCMP_CMD( HAL_BEEP_TIMER, auto_reload_val/2 ); 
}

void hal_beep_off( void )
{
    HAL_BEEP_TIMER_OC_SETCMP_CMD( HAL_BEEP_TIMER, 0 ); 
}


