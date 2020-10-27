#include "hal.h"

/* stm32f103 @ 72M Hz */

void hal_delay_us(uint32_t us)
{
    while( us-- )
    {
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP();
    }
}


void hal_delay_ms(uint32_t ms)
{
    volatile uint32_t a;
    while(ms--)
    {
        for(a=6000; a; a--); 
    }
}





