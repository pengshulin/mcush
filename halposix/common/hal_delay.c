#include <stdint.h>
#include <unistd.h>


void hal_delay_us(uint32_t us)
{
    usleep( us );
}


void hal_delay_ms(uint32_t ms)
{
    usleep( ms * 1000 );
}


