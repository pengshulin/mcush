#ifndef _HAL_H_
#define _HAL_H_
#include <stdint.h>
#include "chip.h"


#define UNIQUE_ID0  0x1FFFF7E8
#define UNIQUE_ID1  0x1FFFF7EC
#define UNIQUE_ID2  0x1FFFF7F0


void hal_debug_init(void);

void hal_led_init(void);
int hal_led_get_num(void);
void hal_led_set(int index);
void hal_led_clr(int index);
void hal_led_toggle(int index);
int hal_led_get(int index);

void hal_delay_us(uint32_t us);
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

void hal_wdg_init(void);
int hal_wdg_is_enable(void);
void hal_wdg_enable(void);
void hal_wdg_disable(void);
void hal_wdg_clear(void);

int hal_get_serial_number( char *buf );

int hal_init(void);


#include "hal_platform.h"

#endif
