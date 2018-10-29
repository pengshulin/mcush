#include "mcush.h"


void hal_delay_us(uint32_t us)
{
    volatile uint32_t a;
    while( us-- )
    {
        for( a=0; a<4; a++ )
        {
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
            __NOP();
        }
    }
}


void hal_delay_ms(uint32_t ms)
{
    volatile uint32_t a;
    while(ms--)
    {
        for(a=39000; a; a--); 
    }
}


void hal_delay_10ms(uint32_t ms10)
{
    hal_delay_ms( 10 * ms10 );
}



