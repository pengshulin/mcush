/* modbus driver (hal_uart driver encapsulated) */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#if HAL_MODBUS

extern int hal_uart_putc( char c, os_tick_t block_ticks );
extern int hal_uart_getc( char *c, os_tick_t block_ticks );

int hal_modbus_init( uint32_t baudrate )
{
    return hal_uart_init(baudrate);
}


void hal_modbus_reset(void)
{
    hal_uart_reset();
}


int hal_modbus_putc( char c, os_tick_t block_ticks )
{
    return hal_uart_putc( c, block_ticks );
}


int hal_modbus_getc( char *c, os_tick_t block_ticks )
{
    return hal_uart_getc( c, block_ticks );
}

#endif

