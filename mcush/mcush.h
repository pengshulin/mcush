/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_H__
#define __MCUSH_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
#include "hal.h"
#include "shell.h"
#include "mcush_os.h"
#include "shell_str.h"
#include "mcush_ver.h"
#include "mcush_opt.h"
#include "mcush_vfs.h"
#include "mcush_base64.h"
#include "mcush_lib.h"
#include "mcush_lib_crc.h"
#include "mcush_lib_fs.h"


#ifdef __cplusplus
extern "C" {
#endif



/* task priority
 * HIGHEST -- highest (timer task)
 * ... (reserved)
 * HIGHER -- reserved
 * HIGH   -- reserved
 * NORMAL -- shell task default
 * LOW    -- reserved
 * LOWER  -- reserved
 * ... (reserved)
 * LOWEST -- lowest (idle task)
 */

#ifndef MCUSH_PRIORITY
    #define MCUSH_PRIORITY  (OS_PRIORITY_NORMAL)
#endif

#ifndef MCUSH_STACK_SIZE
    #define MCUSH_STACK_SIZE  (4*1024)
#endif

#ifndef MCUSH_QUEUE_SIZE
    #define MCUSH_QUEUE_SIZE  (64)
#endif

#ifndef MCUSH_FREERTOS_PEEK_API
    #define MCUSH_FREERTOS_PEEK_API   1
#endif

#if ! MCUSH_FREERTOS_PEEK_API
    #define mcushTaskAddToRegistered( a ) 
    #ifdef USE_CMD_SYSTEM
        #undef USE_CMD_SYSTEM
    #endif
    #define USE_CMD_SYSTEM  0
#endif

#ifndef MCUSH_VFS
    #define MCUSH_VFS  0
#endif

#ifndef MCUSH_ROMFS
    #define MCUSH_ROMFS  0
#endif

#ifndef MCUSH_FCFS
    #define MCUSH_FCFS  0
#endif

#ifndef MCUSH_SPIFFS
    #define MCUSH_SPIFFS  0
#endif

#ifndef MCUSH_FATFS
    #define MCUSH_FATFS  0
#endif


#if !MCUSH_VFS
    #ifdef MCUSH_ROMFS
        #undef MCUSH_ROMFS
        #define MCUSH_ROMFS  0
    #endif
    #ifdef MCUSH_FCFS
        #undef MCUSH_FCFS
        #define MCUSH_RCFS  0
    #endif
    #ifdef MCUSH_SPIFFS
        #undef MCUSH_SPIFFS
        #define MCUSH_SPIFFS  0
    #endif
    #ifdef MCUSH_FATFS
        #undef MCUSH_FATFS
        #define MCUSH_FATFS  0
    #endif
#endif

#ifndef TASK_IDLE_PRIORITY
    #define TASK_IDLE_PRIORITY  OS_PRIORITY_IDLE
#endif



#ifndef USE_CMD_HELP
    #define USE_CMD_HELP  1
#endif
#ifndef USE_CMD_SCPI_IDN
    #define USE_CMD_SCPI_IDN  1
#endif
#ifndef USE_CMD_SCPI_RST
    #define USE_CMD_SCPI_RST  1
#endif
#ifndef USE_CMD_REBOOT
    #define USE_CMD_REBOOT  1
#endif
#ifndef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
#endif
#ifndef USE_CMD_GPIO
    #define USE_CMD_GPIO  1
#endif
#ifndef USE_CMD_LED
    #define USE_CMD_LED  1
#endif
#ifndef USE_CMD_DUMP
    #define USE_CMD_DUMP  1
#endif
#ifndef USE_CMD_WRITE
    #define USE_CMD_WRITE  1
#endif
#ifndef USE_CMD_MFILL
    #define USE_CMD_MFILL  1
#endif
#ifndef USE_CMD_DELAY
    #define USE_CMD_DELAY  1
#endif
#ifndef USE_CMD_ECHO
    #define USE_CMD_ECHO  0  /* to be obseleted */
#endif
#ifndef USE_CMD_MKBUF
    #define USE_CMD_MKBUF  1
#endif
#ifndef USE_CMD_UPTIME
    #define USE_CMD_UPTIME  1
#endif

#ifndef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  1
#endif
#if USE_CMD_SYSTEM
    #ifndef USE_CMD_SYSTEM_QUEUE
        #define USE_CMD_SYSTEM_QUEUE  1
    #endif
    #ifndef USE_CMD_SYSTEM_KERNEL
        #define USE_CMD_SYSTEM_KERNEL  1
    #endif
    #ifndef USE_CMD_SYSTEM_HEAP
        #define USE_CMD_SYSTEM_HEAP  0
    #endif
    #ifndef USE_CMD_SYSTEM_STACK
        #define USE_CMD_SYSTEM_STACK  0
    #endif
    #ifndef USE_CMD_SYSTEM_IDLE
        #define USE_CMD_SYSTEM_IDLE  1
    #endif
#endif

#ifndef USE_CMD_MAPI
    #define USE_CMD_MAPI  1
#endif
#if USE_CMD_MAPI
    #ifndef USE_CMD_MAPI_INFO
        #define USE_CMD_MAPI_INFO  0
    #endif
    #ifndef USE_CMD_MAPI_REALLOC
        #define USE_CMD_MAPI_REALLOC  0
    #endif
    #ifndef MAPI_TEST_MALLOC_SIZE
        #define MAPI_TEST_MALLOC_SIZE  65536
    #endif
#endif

#ifndef USE_CMD_BEEP
    #define USE_CMD_BEEP  0
#endif
#ifndef USE_CMD_SGPIO
    #define USE_CMD_SGPIO  0
#endif
#ifndef USE_CMD_POWER
    #define USE_CMD_POWER  0
#endif
#ifndef USE_CMD_I2C
    #define USE_CMD_I2C  1
    #define SUPPORT_I2C  1
#endif
#ifndef USE_CMD_SPI
    #define USE_CMD_SPI  1
    #define SUPPORT_SPI  1
#endif
#ifndef USE_CMD_PULSE
    #define USE_CMD_PULSE  1
    #define SUPPORT_PULSE  1
#endif
#ifndef USE_CMD_DS1W
    #define USE_CMD_DS1W  1
    #define SUPPORT_DS1W  1
#endif
#ifndef USE_CMD_PWM
    #define USE_CMD_PWM  0
#endif
#ifndef USE_CMD_RTC
    #define USE_CMD_RTC  0
#endif
#ifndef USE_CMD_CAN
    #if HAL_CAN
        #define USE_CMD_CAN  1
    #else
        #define USE_CMD_CAN  0
    #endif
#endif

#ifndef USE_CMD_LOOP
    #define USE_CMD_LOOP  1
#endif

#ifndef USE_CMD_TEST
    #define USE_CMD_TEST  1
#endif


#if MCUSH_FCFS
    #ifndef USE_CMD_FCFS
        #define USE_CMD_FCFS  1
    #endif
#else
    #ifdef USE_CMD_FCFS
        #undef USE_CMD_FCFS
    #endif
    #define USE_CMD_FCFS  0
#endif

#if MCUSH_SPIFFS
    #ifndef USE_CMD_SPIFFS
        #define USE_CMD_SPIFFS  1
    #endif
#else
    #ifdef USE_CMD_SPIFFS
        #undef USE_CMD_SPIFFS
    #endif
    #define USE_CMD_SPIFFS  0
#endif

#if MCUSH_FATFS
    #ifndef USE_CMD_FATFS
        #define USE_CMD_FATFS  1
    #endif
#else
    #ifdef USE_CMD_FATFS
        #undef USE_CMD_FATFS
    #endif
    #define USE_CMD_FATFS  0
#endif


#if MCUSH_VFS
    #ifndef USE_CMD_LS
        #define USE_CMD_LS  1
    #endif
    #ifndef USE_CMD_CAT
        #define USE_CMD_CAT  1
    #endif
    #ifndef USE_CMD_CP
        #define USE_CMD_CP  1
    #endif
    #ifndef USE_CMD_RM
        #define USE_CMD_RM  1
    #endif
    #ifndef USE_CMD_RENAME
        #define USE_CMD_RENAME  1
    #endif
    #ifndef USE_CMD_LOAD
        #if MCUSH_FCFS
            #define USE_CMD_LOAD  1
        #else
            #define USE_CMD_LOAD  0
        #endif
    #endif
    #ifndef USE_CMD_STOP
        #define USE_CMD_STOP  USE_CMD_LOAD
    #endif
    #ifndef USE_CMD_CRC
        #define USE_CMD_CRC  0
    #endif
#else
    #ifdef USE_CMD_CAT
        #undef USE_CMD_CAT
    #endif
    #define USE_CMD_CAT  0
    #ifdef USE_CMD_RM
        #undef USE_CMD_RM
    #endif
    #define USE_CMD_RM  0
    #ifdef USE_CMD_RENAME
        #undef USE_CMD_RENAME
    #endif
    #define USE_CMD_RENAME  0
    #ifdef USE_CMD_CP
        #undef USE_CMD_CP
    #endif
    #define USE_CMD_CP  0
    #ifdef USE_CMD_LS
        #undef USE_CMD_LS
    #endif
    #define USE_CMD_LS  0
    #ifdef USE_CMD_LOAD
        #undef USE_CMD_LOAD
    #endif
    #define USE_CMD_LOAD  0
    #ifdef USE_CMD_CRC
        #undef USE_CMD_CRC
    #endif
    #define USE_CMD_CRC  0

    #undef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
#endif

#if USE_CMD_CAT
    #ifndef USE_CMD_CAT_WRITE
        #define USE_CMD_CAT_WRITE  1
    #endif
    #ifndef USE_CMD_CAT_B64
        #define USE_CMD_CAT_B64  1
    #endif
#endif

#include "hal_core.h"

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


#if MCUSH_OS != OS_NONE
void mcush_init(void);
void mcush_start(void);
extern os_task_handle_t task_mcush;
#endif

extern const char build_signature[];


#ifndef NULL
    #define NULL  (void*)0
#endif



#ifdef __cplusplus
}
#endif


#endif
