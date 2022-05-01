/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_POSIX_H__
#define __MCUSH_VFS_POSIX_H__


#define POSIX_FDS_NUM  10

int mcush_posix_mount( void );
int mcush_posix_umount( void );
int mcush_posix_mounted( void );
int mcush_posix_info( int *total, int *used );
int mcush_posix_format( void );
int mcush_posix_check( void );
int mcush_posix_remove( const char *path );
int mcush_posix_rename( const char *old, const char *newPath );
int mcush_posix_open( const char *path, const char *mode );
int mcush_posix_read( int fh, char *buf, int len );
int mcush_posix_seek( int fh, int offs, int where );
int mcush_posix_write( int fh, char *buf, int len );
int mcush_posix_flush( int fh );
int mcush_posix_close( int fh );
int mcush_posix_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );


extern const mcush_vfs_driver_t mcush_posix_driver;

#endif
