#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__

#define HAL_LED_REV
#define HAL_LED_NUM   2
#define HAL_LED_PORTS  { GPIOC, GPIOD } 
#define HAL_LED_PINS  { GPIO_Pin_0, GPIO_Pin_3 }


#define USE_CMD_SGPIO  1
#define SGPIO_FREQ_MIN  1.0
#define SGPIO_FREQ_DEF  1000000.0
#define SGPIO_FREQ_MAX  4000000.0

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


#define USE_SPIFFS  1
#define HAL_SPIFFS_CHIPID  0xEF4017  // W25P64
#include "spiffs.h"
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd
#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_13
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2
#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2
#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2
#define sFLASH_CS_PIN                        GPIO_Pin_12
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

int hal_spiffs_mount( void );
int hal_spiffs_mounted( void );
int hal_spiffs_info( int *total, int *used );
int hal_spiffs_format( void );
int hal_spiffs_check( void );
int hal_spiffs_remove( const char *path );
int hal_spiffs_rename( const char *old, const char *newPath );
int hal_spiffs_open( const char *path, const char *mode );
int hal_spiffs_read( int fh, void *buf, int len );
int hal_spiffs_seek( int fh, int offs, int where );
int hal_spiffs_write( int fh, void *buf, int len );
int hal_spiffs_flush( int fh );
int hal_spiffs_close( int fh );
spiffs_DIR *hal_spiffs_opendir( const char *name, spiffs_DIR *d );
struct spiffs_dirent *hal_spiffs_readdir( spiffs_DIR *d, struct spiffs_dirent *e );
int hal_spiffs_closedir( spiffs_DIR *d );
int hal_spiffs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );

#include "mcush_vfs.h"

#endif
