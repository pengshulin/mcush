/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include <math.h>


/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PA7  TIMER17_1 ------> BEEPER
   ----------------------------- 
 */

#ifndef HAL_BEEP_DEFINE 
    #define HAL_BEEP_RCC_GPIO_ENABLE_CMD    RCC_AHBPeriphClockCmd
    #define HAL_BEEP_RCC_GPIO_ENABLE_BIT    RCC_AHBPeriph_GPIOA
    #define HAL_BEEP_RCC_TIMER_ENABLE_CMD   RCC_APB2PeriphClockCmd
    #define HAL_BEEP_RCC_TIMER_ENABLE_BIT   RCC_APB2Periph_TIM17
    #define HAL_BEEP_TIMER                  TIM17
    #define HAL_BEEP_TIMER_OC_INIT_CMD      TIM_OC1Init
    #define HAL_BEEP_TIMER_OC_SETCMP_CMD    TIM_SetCompare1
    #define HAL_BEEP_PORT                   GPIOA
    #define HAL_BEEP_PIN                    GPIO_Pin_7
    #define HAL_BEEP_PINSRC                 GPIO_PinSource7
    #define HAL_BEEP_AF                     GPIO_AF_5
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
    HAL_BEEP_RCC_GPIO_ENABLE_CMD( HAL_BEEP_RCC_GPIO_ENABLE_BIT, ENABLE );
    HAL_BEEP_RCC_TIMER_ENABLE_CMD( HAL_BEEP_RCC_TIMER_ENABLE_BIT, ENABLE );

    // alternative output port
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Pin = HAL_BEEP_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( HAL_BEEP_PORT, &gpio_init );
    GPIO_PinAFConfig( (GPIO_TypeDef*)HAL_BEEP_PORT, HAL_BEEP_PINSRC, HAL_BEEP_AF );

    // generate square wave by timer
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = SystemCoreClock/HAL_BEEPER_FREQ_DEF;
    timbase.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit( HAL_BEEP_TIMER, &timbase );
   
    TIM_OCStructInit( &timoc );  
    timoc.TIM_OCMode = TIM_OCMode_PWM1;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    HAL_BEEP_TIMER_OC_INIT_CMD( HAL_BEEP_TIMER, &timoc );
    TIM_Cmd( HAL_BEEP_TIMER, ENABLE );

    /* enable PWM main output */
    TIM_CtrlPWMOutputs( HAL_BEEP_TIMER, ENABLE );
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


