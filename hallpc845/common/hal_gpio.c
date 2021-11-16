#include "hal.h"

const clock_ip_name_t gpio_clks[] = GPIO_CLOCKS;
const uint8_t port_num = sizeof(gpio_clks)/sizeof(uint16_t);
const uint8_t iocon_index_map[][32] = {
{ 
    IOCON_INDEX_PIO0_0,
    IOCON_INDEX_PIO0_1,
    IOCON_INDEX_PIO0_2,
    IOCON_INDEX_PIO0_3,
    IOCON_INDEX_PIO0_4,
    IOCON_INDEX_PIO0_5,
    IOCON_INDEX_PIO0_6,
    IOCON_INDEX_PIO0_7,
    IOCON_INDEX_PIO0_8,
    IOCON_INDEX_PIO0_9,
    IOCON_INDEX_PIO0_10,
    IOCON_INDEX_PIO0_11,
    IOCON_INDEX_PIO0_12,
    IOCON_INDEX_PIO0_13,
    IOCON_INDEX_PIO0_14,
    IOCON_INDEX_PIO0_15,
    IOCON_INDEX_PIO0_16,
    IOCON_INDEX_PIO0_17,
    IOCON_INDEX_PIO0_18,
    IOCON_INDEX_PIO0_19,
    IOCON_INDEX_PIO0_20,
    IOCON_INDEX_PIO0_21,
    IOCON_INDEX_PIO0_22,
    IOCON_INDEX_PIO0_23,
    IOCON_INDEX_PIO0_24,
    IOCON_INDEX_PIO0_25,
    IOCON_INDEX_PIO0_26,
    IOCON_INDEX_PIO0_27,
    IOCON_INDEX_PIO0_28,
    IOCON_INDEX_PIO0_29,
    IOCON_INDEX_PIO0_30,
    IOCON_INDEX_PIO0_31
},
{
    IOCON_INDEX_PIO1_0,
    IOCON_INDEX_PIO1_1,
    IOCON_INDEX_PIO1_2,
    IOCON_INDEX_PIO1_3,
    IOCON_INDEX_PIO1_4,
    IOCON_INDEX_PIO1_5,
    IOCON_INDEX_PIO1_6,
    IOCON_INDEX_PIO1_7,
    IOCON_INDEX_PIO1_8,
    IOCON_INDEX_PIO1_9,
    IOCON_INDEX_PIO1_10,
    IOCON_INDEX_PIO1_11,
    IOCON_INDEX_PIO1_12,
    IOCON_INDEX_PIO1_13,
    IOCON_INDEX_PIO1_14,
    IOCON_INDEX_PIO1_15,
    IOCON_INDEX_PIO1_16,
    IOCON_INDEX_PIO1_17,
    IOCON_INDEX_PIO1_18,
    IOCON_INDEX_PIO1_19,
    IOCON_INDEX_PIO1_20,
    IOCON_INDEX_PIO1_21
}
};





int hal_gpio_get_port_num(void)
{
    return port_num;
}


void hal_gpio_init(void)
{
    int i;
    for(i=0;i<port_num;i++)
        CLOCK_EnableClock(gpio_clks[i]);
    CLOCK_EnableClock(kCLOCK_Iocon);
}


void hal_gpio_set_input(int port, int bits, int pull)
{
    uint32_t mode = (pull==0)?IOCON_MODE_PULLUP:((pull>0)?IOCON_MODE_PULLUP:IOCON_MODE_PULLDOWN);
    int i;
    
    GPIO->DIR[port] &= ~bits;
    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
            IOCON_PinMuxSet( IOCON, iocon_index_map[port][i], mode );
    }
}


void hal_gpio_set_output(int port, int bits, int open_drain)
{
    uint32_t mode = open_drain ? IOCON_OPENDRAIN_EN : 0;
    int i;

    GPIO->DIR[port] |= bits;
    for( i=0; i<32; i++ )
    {
        if( bits & (1<<i) )
            IOCON_PinMuxSet( IOCON, iocon_index_map[port][i], mode );
    }
}


void hal_gpio_set(int port, int bits)
{
    GPIO->SET[port] = bits;
}


void hal_gpio_clr(int port, int bits)
{
    GPIO->CLR[port] = bits;
}


void hal_gpio_toggle(int port, int bits)
{
    GPIO->NOT[port] = bits;
}


int hal_gpio_get(int port, int bits)
{
    return GPIO->MPIN[port] & bits;
}



