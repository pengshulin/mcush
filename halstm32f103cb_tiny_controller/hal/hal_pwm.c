#include "hal.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PA0  TIMER2_1 ------> PWM0
   PA1  TIMER2_2 ------> PWM1
   PA2  TIMER2_3 ------> PWM2
   PA3  TIMER2_4 ------> PWM3
   ----------------------------- 
 */

#ifndef HAL_PWM_DEFINE 
    #define HAL_PWM_RCC_GPIO_ENABLE_CMD    LL_AHB1_GRP1_EnableClock
    #define HAL_PWM_RCC_GPIO_ENABLE_BIT    LL_AHB1_GRP1_PERIPH_GPIOA
    #define HAL_PWM_RCC_TIMER_ENABLE_CMD   LL_APB1_GRP1_EnableClock
    #define HAL_PWM_RCC_TIMER_ENABLE_BIT   LL_APB1_GRP1_PERIPH_TIM2
    #define HAL_PWM_NUM                    4
    #define HAL_PWM_PORT                   GPIOA
    #define HAL_PWM_PINS                   (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3)
    #define HAL_PWM_AF                     LL_GPIO_AF_1
    #define HAL_PWM_TIMER                  TIM2
    #define HAL_PWM_TIMER_CHANNEL          {LL_TIM_CHANNEL_CH1, LL_TIM_CHANNEL_CH2, LL_TIM_CHANNEL_CH3, LL_TIM_CHANNEL_CH4}
    #define HAL_PWM_TIMER_OC_SETCMP_CMD    {LL_TIM_OC_SetCompareCH1, LL_TIM_OC_SetCompareCH2, LL_TIM_OC_SetCompareCH3, LL_TIM_OC_SetCompareCH4}
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

static uint16_t hal_pwm_timer_channel[HAL_PWM_NUM] = HAL_PWM_TIMER_CHANNEL;
static void (*_pwm_oc_setcmp[HAL_PWM_NUM])(TIM_TypeDef*, uint32_t) = HAL_PWM_TIMER_OC_SETCMP_CMD;

//static short _pwm_freq = HAL_PWM_FREQ_DEF;
static short _pwm_range = HAL_PWM_RANGE_DEF;
static short _pwm_value[HAL_PWM_NUM];



void hal_pwm_init( int freq, int range, int value )
{
    GPIO_InitTypeDef gpio_init;
    LL_TIM_InitTypeDef tim_init;
    LL_TIM_OC_InitTypeDef timoc_init;
    int i;

    // clock
    //HAL_PWM_RCC_GPIO_ENABLE_CMD( HAL_PWM_RCC_GPIO_ENABLE_BIT );
    HAL_PWM_RCC_TIMER_ENABLE_CMD( HAL_PWM_RCC_TIMER_ENABLE_BIT );

    // alternative output port
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    //gpio_init.Alternate = HAL_PWM_AF;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Pin = HAL_PWM_PINS;
    HAL_GPIO_Init( HAL_PWM_PORT, &gpio_init );

    if( freq == 0 )
        freq = HAL_PWM_FREQ_DEF;
    if( range <= 0 )
        range = HAL_PWM_RANGE_DEF;
    if( value < 0 )
        value = HAL_PWM_VALUE_DEF;
    _pwm_range = range;

    /* stop for new setting */
    LL_TIM_DisableCounter( HAL_PWM_TIMER );

    /* generate base_clock = base_freq * range */
    tim_init.CounterMode = TIM_COUNTERMODE_UP;
    tim_init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_init.Prescaler = SystemCoreClock/(2*freq*range)-1;
    tim_init.Autoreload = _pwm_range - 1;
    tim_init.RepetitionCounter = 0;
    LL_TIM_Init( HAL_PWM_TIMER, &tim_init );
  
    /* compare output for each channel */
    LL_TIM_OC_StructInit( &timoc_init ); 
    timoc_init.OCMode = LL_TIM_OCMODE_PWM1;
    timoc_init.CompareValue = 0;
    timoc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    timoc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    for( i=0; i<HAL_PWM_NUM; i++ )
    {
        LL_TIM_OC_Init( HAL_PWM_TIMER, hal_pwm_timer_channel[i], &timoc_init );
        LL_TIM_OC_ConfigOutput( HAL_PWM_TIMER, hal_pwm_timer_channel[i], LL_TIM_OCPOLARITY_HIGH );
        (*_pwm_oc_setcmp[i])( HAL_PWM_TIMER, value );
        _pwm_value[i] = value;
    }

    /* ready to run */
    LL_TIM_EnableCounter( HAL_PWM_TIMER );
}

 
void hal_pwm_deinit( void )
{
    GPIO_InitTypeDef gpio_init;
    
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_INPUT;
    //gpio_init.Alternate = 0;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Pin = HAL_PWM_PINS;
    HAL_GPIO_Init( HAL_PWM_PORT, &gpio_init );

    LL_TIM_DisableCounter( HAL_PWM_TIMER );
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

