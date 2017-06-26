#include <math.h>
#include "hal.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

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


void hal_debug_init(void)
{
}

 
int hal_init(void)
{
    hal_wdg_init();
    hal_rcc_init();
    hal_debug_init();
    hal_gpio_init();
    hal_led_init();
    if( !hal_uart_init(baudrate) )
        return 0;

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
  
    return 1;
}



