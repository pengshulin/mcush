/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_ROMFS_H__
#define __MCUSH_VFS_ROMFS_H__


int mcush_romfs_mount( void );
int mcush_romfs_mounted( void );
int mcush_romfs_info( int *total, int *used );
int mcush_romfs_format( void );
int mcush_romfs_check( void );
int mcush_romfs_remove( const char *path );
int mcush_romfs_rename( const char *old, const char *newPath );
int mcush_romfs_open( const char *path, const char *mode );
int mcush_romfs_read( int fh, void *buf, int len );
int mcush_romfs_seek( int fh, int offs, int where );
int mcush_romfs_write( int fh, void *buf, int len );
int mcush_romfs_flush( int fh );
int mcush_romfs_close( int fh );
int mcush_romfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );


const mcush_vfs_driver_t mcush_romfs_driver;

#endif
