/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_FATFS_H__
#define __MCUSH_VFS_FATFS_H__


#if MCUSH_FATFS
#include "ff.h"
#endif

void hal_fatfs_init(void);
//s32_t *hal_fatfs_read(u32_t addr, u32_t size, u8_t *dst);
//s32_t *hal_fatfs_write(u32_t addr, u32_t size, u8_t *src);
//s32_t *hal_fatfs_erase(u32_t addr, u32_t size);

int mcush_fatfs_mount( void );
int mcush_fatfs_umount( void );
int mcush_fatfs_mounted( void );
int mcush_fatfs_info( int *total, int *used );
int mcush_fatfs_format( void );
int mcush_fatfs_check( void );
int mcush_fatfs_remove( const char *path );
int mcush_fatfs_rename( const char *old, const char *newPath );
int mcush_fatfs_open( const char *path, const char *mode );
int mcush_fatfs_read( int fh, void *buf, int len );
int mcush_fatfs_seek( int fh, int offs, int where );
int mcush_fatfs_write( int fh, void *buf, int len );
int mcush_fatfs_flush( int fh );
int mcush_fatfs_close( int fh );
int mcush_fatfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );


extern const mcush_vfs_driver_t mcush_fatfs_driver;

#endif
