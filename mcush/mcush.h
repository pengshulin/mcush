/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_H__
#define __MCUSH_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "mcush_freertos_api.h"
#include "shell.h"
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
 * 6 -- highest (timer task)
 * 5 -- reserved
 * 4 -- reserved
 * 3 -- shell task default
 * 2 -- reserved
 * 1 -- reserved
 * 0 -- lowest (idle task)
 */
#ifndef MCUSH_PRIORITY
    #define MCUSH_PRIORITY  (3)
#endif

#ifndef MCUSH_STACK_SIZE
    #define MCUSH_STACK_SIZE  (3*1024)
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
    #define TASK_IDLE_PRIORITY  (tskIDLE_PRIORITY)
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
#ifndef USE_CMD_WAIT
    #define USE_CMD_WAIT  1
#endif
#ifndef USE_CMD_ECHO
    #define USE_CMD_ECHO  1
#endif
#ifndef USE_CMD_MKBUF
    #define USE_CMD_MKBUF  1
#endif
#ifndef USE_CMD_WDG
    #define USE_CMD_WDG  1
#endif
#ifndef USE_CMD_UPTIME
    #define USE_CMD_UPTIME  1
#endif

#ifndef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  1
    #ifndef USE_CMD_SYSTEM_HEAP
        #define USE_CMD_SYSTEM_HEAP  1
    #endif
    #ifndef USE_CMD_SYSTEM_STACK
        #define USE_CMD_SYSTEM_STACK  1
    #endif
#endif

#ifndef USE_CMD_MAPI
    #define USE_CMD_MAPI  1
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
#endif
#ifndef USE_CMD_SPI
    #define USE_CMD_SPI  1
#endif
#ifndef USE_CMD_PWM
    #define USE_CMD_PWM  0
#endif
#ifndef USE_CMD_COUNTER
    #define USE_CMD_COUNTER  0
#endif
#ifndef USE_CMD_RTC
    #define USE_CMD_RTC  0
#endif
#ifndef USE_CMD_LOOP
    #define USE_CMD_LOOP  1
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
    #ifndef USE_CMD_CAT
        #define USE_CMD_CAT  1
    #endif
    #ifndef USE_CMD_RM
        #define USE_CMD_RM  1
    #endif
    #ifndef USE_CMD_RENAME
        #define USE_CMD_RENAME  1
    #endif
    #ifndef USE_CMD_CP
        #define USE_CMD_CP  1
    #endif
    #ifndef USE_CMD_LS
        #define USE_CMD_LS  1
    #endif
    #ifndef USE_CMD_LOAD
        #define USE_CMD_LOAD  1
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

   
#if USE_CMD_I2C
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

 
#if USE_CMD_SPI
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
void emu_spi_deinit( int spi_index );
int emu_spi_write(int spi_index, uint32_t *buf_out, uint32_t *buf_in, int bytes);
#endif


extern void mcush_init(void);
extern void mcush_start(void);
extern TaskHandle_t task_mcush;


#ifndef NULL
    #define NULL  (void*)0
#endif



#ifdef __cplusplus
}
#endif


#endif
