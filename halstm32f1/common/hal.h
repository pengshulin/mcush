#ifndef __HAL_H__
#define __HAL_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_chip.h"

void hal_clk_init(void);
void hal_debug_init(void);

void hal_led_init(void);
int hal_led_get_num(void);
void hal_led_set(int index);
void hal_led_clr(int index);
void hal_led_toggle(int index);
int hal_led_get(int index);

int hal_key_get_num(void);
int hal_key_get(int mask);

void hal_delay_us(uint32_t us);
void hal_delay_ms(uint32_t ms);
void hal_delay_10ms(uint32_t ms10);

void hal_reboot(void);
void hal_reboot_counter_init(void);
void hal_reboot_counter_reset(void);
unsigned int hal_reboot_counter_get(void);

int hal_gpio_get_port_num(void);
void hal_gpio_init(void);
void hal_gpio_set_input(int port, int bits);
void hal_gpio_set_input_pull(int port, int bits, int pull);
void hal_gpio_set_output(int port, int bits);
void hal_gpio_set_output_open_drain(int port, int bits);
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

int hal_fcfs_erase(void);
int hal_fcfs_program(int offset, int *buf, int len);


typedef struct _sgpio_cfg_t
{
    uint8_t inited;
    uint8_t run;
    uint8_t loop_mode;
    uint8_t port;
    uint16_t input_mode;
    uint16_t output_mode;
    void *buf_out;
    void *buf_in;
    uint32_t buf_len;
    float freq;
} sgpio_cfg_t;

int hal_sgpio_init( void );
int hal_sgpio_setup( int loop_mode, int port, int output_mode, int input_mode, void *buf_out, void *buf_in, int buf_len, float freq );
int hal_sgpio_start( void );
void hal_sgpio_stop( void );
int hal_sgpio_set_freq( float freq );
sgpio_cfg_t *hal_sgpio_info( void );

#ifndef SGPIO_FREQ_MIN
#define SGPIO_FREQ_MIN  1.0
#endif
#ifndef SGPIO_FREQ_DEF
#define SGPIO_FREQ_DEF  1000000.0
#endif
#ifndef SGPIO_FREQ_MAX
#define SGPIO_FREQ_MAX  4000000.0
#endif


void hal_beep_init( void );
void hal_beep_on( int freq );
void hal_beep_off( void );

void hal_pwm_init( int freq, int range, int value );
void hal_pwm_deinit( void );
void hal_pwm_set( int index, int value );
int hal_pwm_get( int index );
int hal_pwm_get_num( void );

void hal_adc_init( void );
void hal_adc_deinit( void );
int hal_adc_get_num( void );
int hal_adc_in_use( void );
float hal_adc_get( int index );

#if defined(SUPPORT_WS2812)
int hal_ws2812_init(int port, int pin);
void hal_ws2812_deinit(void);
void hal_ws2812_clr(void);
void hal_ws2812_write0(void);
void hal_ws2812_write1(void);
#endif

int hal_init(void);
int hal_get_serial_number( char *buf );
void init_sys_tick(void);
unsigned int get_sys_tick_count(void);
void hal_platform_reset(void);
void hal_platform_init(void);


#include "hal_platform.h"

#if defined(HAL_DAQ) && HAL_DAQ
#include "hal_daq.h"
#endif

#if HAL_CAN
#include "hal_can.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
