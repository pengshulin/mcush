/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_FCFS_H__
#define __MCUSH_VFS_FCFS_H__
#include <stdint.h>


typedef struct {
    uint16_t offset;
    uint16_t len;
} fcfs_file_t;


typedef struct {
    const fcfs_file_t *file;
    const char *contents;
    int pos;
} fcfs_file_desc_t;

#define FCFS_MAGIC_CODE       0x53464346   // ascii 'FCFS'

#define FCFS_FDS_NUM     3



int mcush_fcfs_mount( void );
int mcush_fcfs_mounted( void );
int mcush_fcfs_info( int *total, int *used );
int mcush_fcfs_format( void );
int mcush_fcfs_check( void );
int mcush_fcfs_remove( const char *path );
int mcush_fcfs_rename( const char *old, const char *newPath );
int mcush_fcfs_open( const char *pathname, const char *mode );
int mcush_fcfs_read( int fh, void *buf, int len );
int mcush_fcfs_seek( int fh, int offs, int where );
int mcush_fcfs_write( int fh, void *buf, int len );
int mcush_fcfs_flush( int fh );
int mcush_fcfs_close( int fh );
int mcush_fcfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );


extern const mcush_vfs_driver_t mcush_fcfs_driver;

#endif
