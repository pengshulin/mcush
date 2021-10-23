/* extra hal apis  */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_EXTRA_H__
#define __HAL_EXTRA_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* gpio emulated buses */

#if defined(SUPPORT_I2C)
typedef struct {
    uint8_t port_scl, port_sda;
    uint8_t pin_scl, pin_sda;
    uint16_t pin_scl_bit, pin_sda_bit;
    uint32_t delay_us;
    uint32_t addr;
    uint8_t lsb;
} i2c_cb_t;

void emu_i2c_init_structure( i2c_cb_t *i2c_init );
int emu_i2c_init( int i2c_index, i2c_cb_t *i2c_init );
void emu_i2c_deinit( int i2c_index );
int emu_i2c_write(int i2c_index, uint8_t *buf_out, uint8_t *buf_in, int write_bytes, int read_bytes, int address, int no_stop );
#endif


#if defined(SUPPORT_SPI)
typedef struct {
    uint8_t port_sdi, port_sdo, port_sck, port_cs;
    uint8_t pin_sdi, pin_sdo, pin_sck, pin_cs;
    uint16_t pin_sdi_bit, pin_sdo_bit, pin_sck_bit, pin_cs_bit;
    uint32_t delay_us;
    uint32_t width;
    uint8_t cpol, cpha, lsb;
} spi_cb_t;

void emu_spi_init_structure( spi_cb_t *spi_init );
int emu_spi_init( int spi_index, spi_cb_t *spi_init );
int emu_spi_update( int spi_index, spi_cb_t *update );
void emu_spi_deinit( int spi_index );
int emu_spi_write(int spi_index, uint32_t *buf_out, uint32_t *buf_in, int length);
spi_cb_t *emu_spi_get_cb( int spi_index );
#endif


#if defined(SUPPORT_PULSE)
typedef struct {
    uint8_t port;
    uint8_t pin;
    uint8_t invert;
    uint16_t pin_bit;
    uint32_t delay_us;
} pulse_cb_t;

void emu_pulse_init_structure( pulse_cb_t *pulse_init );
int emu_pulse_init( int pulse_index, pulse_cb_t *pulse_init );
void emu_pulse_deinit( int pulse_index );
int emu_pulse_generate(int pulse_index, int number );
#endif


#if defined(SUPPORT_DS1W)
typedef struct {
    uint8_t port;
    uint8_t pin;
    uint16_t pin_bit;
} ds1w_cb_t;

void emu_ds1w_init_structure( ds1w_cb_t *ds1w_init );
int emu_ds1w_init( int ds1w_index, ds1w_cb_t *ds1w_init );
void emu_ds1w_deinit( int ds1w_index );
int emu_ds1w_reset( int ds1w_index );
void emu_ds1w_write_bit( int ds1w_index, int val );
void emu_ds1w_write_byte( int ds1w_index, char byte );
int emu_ds1w_read_bit( int ds1w_index );
char emu_ds1w_read_byte( int ds1w_index );
#endif


#if defined(SUPPORT_WS2812)
int ws2812_init( int length, int group_length,  int port, int pin );
void ws2812_deinit(void);
int ws2812_write(int offset, int dat, int swap_rg);
int ws2812_push(int forward, int dat, int swap_rg, int offset, int length);
void ws2812_flush(void);
void ws2812_init_group_length_list( uint8_t *list, int list_length );
#endif



#ifdef __cplusplus
}
#endif

#endif
