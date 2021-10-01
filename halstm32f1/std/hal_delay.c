#include "mcush.h"

/* stm32f103 @ 72M Hz */

#pragma GCC optimize("O3")

__weak void hal_delay_us(uint32_t us)
{
    volatile uint32_t a;
    while( us-- )
    {
        for(a=5; a; a--); 
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
        //__NOP(); __NOP(); __NOP(); __NOP();
    }
}


__weak void hal_delay_ms(uint32_t ms)
{
    volatile uint32_t a;
    while(ms--)
    {
        for(a=6000; a; a--); 
    }
}





