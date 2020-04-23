/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

#if USE_CMD_WS2812
static uint8_t ws2812_port;
static volatile uint32_t *ws2812_set_reg;
static uint32_t ws2812_pin_set_mask;
static uint32_t ws2812_pin_clr_mask;

extern uint8_t port_num;
extern GPIO_TypeDef * const ports[];



int hal_ws2812_init(int port, int pin)
{
    if( (port < 0) || (port >= port_num) || (pin < 0) || (pin > 15) )
        return 0;
    ws2812_set_reg = &(ports[port]->BSRR);
    ws2812_port = port;
    ws2812_pin_set_mask = 1<<pin;
    ws2812_pin_clr_mask = (1<<pin)<<16;
    hal_gpio_set_output(port, ws2812_pin_set_mask);
    hal_gpio_clr(port, ws2812_pin_set_mask);
    return 1;
}


void hal_ws2812_deinit(void)
{
    if( ws2812_set_reg )
    {
        hal_gpio_set_input(ws2812_port, ws2812_pin_set_mask);
        ws2812_set_reg = 0;
    }
}


void hal_ws2812_clr(void)
{
    *ws2812_set_reg = ws2812_pin_clr_mask;                                                                     
}


/*  Write 1:
        T1H 0.80us +/-150ns
        T1L 0.45us +/-150ns
    Write 0:
        T1H 0.40us +/-150ns
        T1L 0.85us +/-150ns
 */
#pragma GCC optimize("O3")
void hal_ws2812_write0(void)
{
    *ws2812_set_reg = ws2812_pin_set_mask;                                                                     
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); 
    *ws2812_set_reg = ws2812_pin_clr_mask;                                                                     
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
}


void hal_ws2812_write1(void)
{
    *ws2812_set_reg = ws2812_pin_set_mask;
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    *ws2812_set_reg = ws2812_pin_clr_mask;                                                                     
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
}


#endif

