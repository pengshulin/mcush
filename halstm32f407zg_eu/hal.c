#include "mcush.h"


const unsigned int baudrate=9600;


void hal_delay_ms(uint32_t ms)
{
    volatile uint32_t a;
    while(ms--)
    {
        for(a=1600; a; a--); 
    }
}

void test_hal_delay_ms(void)
{
    while(1)
    {
        hal_delay_ms(1000);
    }
}


void hal_rcc_init(void)
{  
    /* Enable ADC & SYSCFG clocks */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);

    /* NVIC_PriorityGroup_4: 4 bits for pre-emption priority, 0 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

#ifdef MCUSH_NON_OS
extern uint32_t SystemCoreClock;
unsigned int _sys_tick_cnt;
void init_sys_tick(void)
{
    #define portNVIC_SYSTICK_CTRL_REG     ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
    #define portNVIC_SYSTICK_LOAD_REG     ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
	#define portNVIC_SYSTICK_CLK_BIT      ( 1UL << 2UL )
    #define portNVIC_SYSTICK_INT_BIT      ( 1UL << 1UL )
    #define portNVIC_SYSTICK_ENABLE_BIT   ( 1UL << 0UL )
    portNVIC_SYSTICK_LOAD_REG = ( SystemCoreClock / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
    _sys_tick_cnt = 0;
}


unsigned int get_sys_tick_count(void)
{
    return _sys_tick_cnt;
}

void SysTick_Handler( void )
{
    _sys_tick_cnt += 1;
}

#endif

void hal_debug_init(void)
{
}

 
int hal_init(void)
{
    hal_wdg_init();
    hal_rcc_init();
    hal_debug_init();
    hal_gpio_init();
    hal_sgpio_init();
    hal_led_init();
    if( !hal_uart_init(baudrate) )
        return 0;
#ifdef MCUSH_NON_OS
    init_sys_tick();
#endif
    return 1;
}



