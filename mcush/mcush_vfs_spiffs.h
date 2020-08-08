/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_SPIFFS_H__
#define __MCUSH_VFS_SPIFFS_H__

#include "hal.h"

#ifndef SPIFLASH_AUTO_DETECT
    #ifndef SPIFLASH_CFG_PHYS_SZ
        #error "SPIFLASH_CFG_PHYS_SZ not defined"
    #endif
#endif

#ifndef SPIFLASH_CFG_PHYS_ERASE_SZ
    #define SPIFLASH_CFG_PHYS_ERASE_SZ  (65536)
#endif

#ifndef SPIFLASH_CFG_PHYS_ADDR 
    #define SPIFLASH_CFG_PHYS_ADDR      (0)
#endif

#ifndef SPIFLASH_CFG_LOG_BLOCK_SZ
    #define SPIFLASH_CFG_LOG_BLOCK_SZ   (65536)
#endif

#ifndef SPIFLASH_CFG_LOG_PAGE_SZ
    #define SPIFLASH_CFG_LOG_PAGE_SZ    (256)
#endif

#if MCUSH_SPIFFS
#include "spiffs.h"
#endif

void hal_spiffs_flash_init(void);
int hal_spiffs_flash_read_id(void);
int hal_spiffs_flash_read_status(void);
void hal_spiffs_flash_lock(int lock);
s32_t *hal_spiffs_flash_read(u32_t addr, u32_t size, u8_t *dst);
s32_t *hal_spiffs_flash_write(u32_t addr, u32_t size, u8_t *src);
s32_t *hal_spiffs_flash_erase(u32_t addr, u32_t size);

int mcush_spiffs_mount( void );
int mcush_spiffs_umount( void );
int mcush_spiffs_mounted( void );
int mcush_spiffs_info( int *total, int *used );
int mcush_spiffs_format( void );
int mcush_spiffs_check( void );
int mcush_spiffs_remove( const char *path );
int mcush_spiffs_rename( const char *old, const char *newPath );
int mcush_spiffs_open( const char *path, const char *mode );
int mcush_spiffs_read( int fh, char *buf, int len );
int mcush_spiffs_seek( int fh, int offs, int where );
int mcush_spiffs_write( int fh, char *buf, int len );
int mcush_spiffs_flush( int fh );
int mcush_spiffs_close( int fh );
int mcush_spiffs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );


extern const mcush_vfs_driver_t mcush_spiffs_driver;

#endif
