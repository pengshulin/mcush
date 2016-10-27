#ifndef _HAL_H_
#define _HAL_H_
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"



void hal_debug_init(void);


void hal_led_init(void);
int hal_led_get_num(void);
void hal_led_set(int index);
void hal_led_clr(int index);
void hal_led_toggle(int index);
int hal_led_get(int index);

void hal_delay_ms(uint32_t ms);

void hal_reset(void);


int hal_gpio_get_port_num(void);
void hal_gpio_init(void);
void hal_gpio_set_input(int port, int bits);
void hal_gpio_set_input_pull(int port, int bits, int pull);
void hal_gpio_set_output(int port, int bits);
void hal_gpio_set(int port, int bits);
void hal_gpio_clr(int port, int bits);
void hal_gpio_toggle(int port, int bits);
int hal_gpio_get(int port, int bits);



int hal_uart_init(uint32_t baudrate);
void hal_uart_enable(uint8_t enable);
void hal_uart_reset(void);
signed portBASE_TYPE hal_uart_putc( char c, TickType_t xBlockTime );
signed portBASE_TYPE hal_uart_getc( char *c, TickType_t xBlockTime );

int hal_init(void);

#endif



