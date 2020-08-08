#include "hal.h"
#include <math.h>


/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PB0  TIMER3_3 ------> BEEPER
   ----------------------------- 
 */

#ifndef HAL_BEEP_DEFINE 
    #define HAL_BEEP_RCC_GPIO_ENABLE_CMD    LL_AHB1_GRP1_EnableClock
    #define HAL_BEEP_RCC_GPIO_ENABLE_BIT    LL_AHB1_GRP1_PERIPH_GPIOB
    #define HAL_BEEP_RCC_TIMER_ENABLE_CMD   LL_APB1_GRP1_EnableClock
    #define HAL_BEEP_RCC_TIMER_ENABLE_BIT   LL_APB1_GRP1_PERIPH_TIM3
    #define HAL_BEEP_TIMER                  TIM3
    #define HAL_BEEP_TIMER_CHANNEL          LL_TIM_CHANNEL_CH3
    #define HAL_BEEP_TIMER_OC_SETCMP_CMD    LL_TIM_OC_SetCompareCH3
    #define HAL_BEEP_PORT                   GPIOB
    #define HAL_BEEP_PIN                    GPIO_PIN_0
    //#define HAL_BEEP_AF                     LL_GPIO_AF_2
#endif

    
#ifndef HAL_BEEPER_FREQ_DEF
    #define HAL_BEEPER_FREQ_DEF             4000
#endif
 

extern uint32_t SystemCoreClock;


void hal_beep_init( void )
{
    GPIO_InitTypeDef gpio_init;
    LL_TIM_InitTypeDef tim_init;
    LL_TIM_OC_InitTypeDef timoc_init;

    // clock
    //HAL_BEEP_RCC_GPIO_ENABLE_CMD( HAL_BEEP_RCC_GPIO_ENABLE_BIT );
    HAL_BEEP_RCC_TIMER_ENABLE_CMD( HAL_BEEP_RCC_TIMER_ENABLE_BIT );

    // alternative output port
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pin = HAL_BEEP_PIN;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init( HAL_BEEP_PORT, &gpio_init );

    // generate square wave by timer
    tim_init.CounterMode = TIM_COUNTERMODE_UP;
    tim_init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_init.Prescaler = 0;
    tim_init.Autoreload = SystemCoreClock/HAL_BEEPER_FREQ_DEF;
    tim_init.RepetitionCounter = 0;
    LL_TIM_Init( HAL_BEEP_TIMER, &tim_init );
   
    LL_TIM_OC_StructInit( &timoc_init ); 
    timoc_init.OCMode = LL_TIM_OCMODE_PWM1;
    timoc_init.CompareValue = 0;
    timoc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    timoc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init( HAL_BEEP_TIMER, HAL_BEEP_TIMER_CHANNEL, &timoc_init );
    LL_TIM_OC_ConfigOutput( HAL_BEEP_TIMER, HAL_BEEP_TIMER_CHANNEL, LL_TIM_OCPOLARITY_HIGH );
    LL_TIM_EnableCounter( HAL_BEEP_TIMER );
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

    LL_TIM_SetCounter( HAL_BEEP_TIMER, 0 ); 
    LL_TIM_SetAutoReload( HAL_BEEP_TIMER, auto_reload_val ); 
    LL_TIM_SetPrescaler( HAL_BEEP_TIMER, auto_reload_val );
    HAL_BEEP_TIMER_OC_SETCMP_CMD( HAL_BEEP_TIMER, auto_reload_val/2 ); 
}


void hal_beep_off( void )
{
    HAL_BEEP_TIMER_OC_SETCMP_CMD( HAL_BEEP_TIMER, 0 ); 
}

