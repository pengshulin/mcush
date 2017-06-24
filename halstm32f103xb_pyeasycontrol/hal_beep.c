#include "hal.h"
#include <math.h>

/* test on platform: stm32f103rb, piezoelectric buzzer connected on PA2
 */
 
extern uint32_t SystemCoreClock;

#define PORT_BEEP                           GPIOA
#define PIN_BEEP                            GPIO_Pin_2
#define RCC_APB2Periph_BEEP                 RCC_APB2Periph_GPIOA

void hal_beep_init( void )
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef timbase;
    TIM_OCInitTypeDef timoc;

    // GPIO clock
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_BEEP, ENABLE );

    // as 50Mhz  
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    // BEEP as alternative output
    gpio.GPIO_Pin = PIN_BEEP;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( PORT_BEEP, &gpio );

    // TIM2 clock enable
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);
    
    // TIM2_CN3 generate square wave
    timbase.TIM_CounterMode = TIM_CounterMode_Up;
    timbase.TIM_ClockDivision = TIM_CKD_DIV1;
    timbase.TIM_Prescaler = 0;
    timbase.TIM_Period = 72000/4; // default freq: 4KHz
    TIM_TimeBaseInit( TIM2, &timbase );
    // set output
    timoc.TIM_OCMode = TIM_OCMode_PWM1;
    timoc.TIM_Pulse = 0;
    timoc.TIM_OutputState = TIM_OutputState_Enable;
    timoc.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init( TIM2, &timoc );
    TIM_Cmd( TIM2, ENABLE );

}

void hal_beep_on( int freq )
{
    short val = sqrt( SystemCoreClock / freq );

    TIM2->CNT = 0;
    TIM2->ARR = val;
    TIM_PrescalerConfig( TIM2, val, TIM_PSCReloadMode_Immediate );
    
    TIM2->CCR3 = TIM2->ARR/2;
}

void hal_beep_off( void )
{
    TIM2->CCR3 = 0;
}


