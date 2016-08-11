#ifndef _HAL_H_
#define _HAL_H_
#include <stdint.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"



void hal_debug_init(void);
void hal_debug_led(uint8_t index, int8_t mode);

void hal_delay_ms(uint32_t ms);

void hal_reset(void);

int hal_uart_init(uint32_t baudrate);
void hal_uart_enable(uint8_t enable);
void hal_uart_reset(void);
signed portBASE_TYPE hal_uart_putc( char c, TickType_t xBlockTime );
signed portBASE_TYPE hal_uart_getc( char *c, TickType_t xBlockTime );

int hal_init(void);

#endif



