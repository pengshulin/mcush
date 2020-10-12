/* minimum hal apis  */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_CORE_H__
#define __HAL_CORE_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


void hal_clk_init(void);

void hal_wdg_init(void);
void hal_wdg_clear(void);

void hal_delay_us(uint32_t us);
void hal_delay_ms(uint32_t ms);

void hal_reboot(void);

void hal_led_init(void);
int hal_led_get_num(void);
void hal_led_set(int index);
void hal_led_clr(int index);
void hal_led_toggle(int index);
int hal_led_get(int index);

int hal_gpio_get_port_num(void);
void hal_gpio_init(void);
void hal_gpio_set_input(int port, int bits, int pull);
void hal_gpio_set_output(int port, int bits, int open_drain);
void hal_gpio_set(int port, int bits);
void hal_gpio_clr(int port, int bits);
void hal_gpio_toggle(int port, int bits);
int hal_gpio_get(int port, int bits);



#ifdef __cplusplus
}
#endif


#endif
