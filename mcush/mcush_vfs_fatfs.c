/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_FATFS
#include "ff.h"


#ifndef FATFS_FD_NUM
#define FATFS_FD_NUM  MCUSH_VFS_FILE_DESCRIPTOR_NUM
#endif

FATFS fs;

static int mcush_fatfs_driver_errno;
static uint8_t _mounted;
static FIL *_fds[FATFS_FD_NUM];

int mcush_fatfs_mounted( void )
{
    return _mounted;
}


int mcush_fatfs_mount( void )
{
    FRESULT r;

    if( _mounted )
        return 1;
          
    /* option: 0 - delayed mount until scheduler runs */
    r = f_mount( &fs, "", 0 );
    if( r != FR_OK )
    {
        return 0;
    }
    memset( &_fds, 0, sizeof(FIL)*FATFS_FD_NUM );
    _mounted = 1;
    return 1;
}


int mcush_fatfs_umount( void )
{
    if( ! _mounted )
        return 1;

    /* TODO: close all unclosed files */
    
    f_mount( 0, "", 0 );
    _mounted = 0;
    return 1;
}


int mcush_fatfs_check( void )
{
    return 0;
}


int mcush_fatfs_remove( const char *path )
{
    if( ! _mounted )
        return 0;
    return 0;
}


int mcush_fatfs_rename( const char *old, const char *newPath )
{
    if( ! _mounted )
        return 0;
     return 0;
}


static uint8_t parse_fatfs_mode_flags( const char *mode )
{
    uint8_t flags = 0;
    uint8_t r=0, w=0, c=0, a=0;
    while( mode && *mode )
    {
        if( *mode == 'r' )
            r = 1;
        else if( *mode == 'w' )
            w = 1;
        else if( *mode == '+' )
            c = 1;
        else if( *mode == 'a' )
            a = w = 1;
        mode++;
    }
    if( r && w )
        flags = FA_READ | FA_WRITE;
    else if( r && !w )
        flags = FA_READ;
    else if( w && !r )
        flags = FA_WRITE;
    if( a )
        flags |= FA_OPEN_APPEND;
    else if( w )
        flags |= FA_CREATE_ALWAYS;
    else
        flags |= FA_OPEN_EXISTING;
    if( c )
        flags |= FA_CREATE_NEW;
    return flags;
}


int get_fil_free_slot(void)
{
    int i;
    for( i=0; i<FATFS_FD_NUM; i++ )
    {
        if( _fds[i] == NULL )
            return i;
    }
    return -1;
}


int mcush_fatfs_open( const char *path, const char *mode )
{
    FRESULT ret;
    FIL *pfil;
    int fil_idx = get_fil_free_slot();

    if( fil_idx < 0 )
        return 0;

    pfil = os_malloc( sizeof(FIL) );
    if( pfil == NULL )
        return 0;

    ret = f_open( pfil, path, parse_fatfs_mode_flags(mode) );
    if( ret != FR_OK )
    {
        mcush_fatfs_driver_errno = ret;
        os_free(pfil);
        return 0;
    }

    _fds[fil_idx] = pfil;
    return fil_idx+1;
}


int mcush_fatfs_read( int fh, char *buf, int len )
{
    memset( (void*)buf, 0x55, len );
    return len;
}


int mcush_fatfs_write( int fh, char *buf, int len )
{
    return 0;
}


int mcush_fatfs_seek( int fh, int offs, int where )
{
    return 0;
}


int mcush_fatfs_flush( int fh )
{
    FRESULT ret;
    FIL *pfil;

    if( (fh <= 0) || (fh > FATFS_FD_NUM) )
        return 0;
    pfil = _fds[--fh];
    ret = f_sync( pfil );
    return (ret==FR_OK) ? 1 : 0;
}


int mcush_fatfs_close( int fh )
{
    FRESULT ret;
    FIL *pfil;

    if( (fh <= 0) || (fh > FATFS_FD_NUM) )
        return 0;
    pfil = _fds[--fh];
    ret = f_close( pfil );
    os_free( pfil );
    _fds[fh] = 0;
    return 1;
}


int mcush_fatfs_format( void )
{
    FRESULT r;
    
    r = f_mkfs( "", FM_FAT32, 0, 0, 0 );
    if( r != FR_OK )
    {
        shell_printf( "%d\n", r );
        return 1;
    }
    return 0;
}


int mcush_fatfs_size( const char *name, int *size )
{
    return 0;
}


int mcush_fatfs_info( int *total, int *used )
{
    return 0;
}


int mcush_fatfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    return 0;
}



const mcush_vfs_driver_t mcush_fatfs_driver = {
    &mcush_fatfs_driver_errno,
    mcush_fatfs_mount,
    mcush_fatfs_umount,
    mcush_fatfs_info,
    mcush_fatfs_format,
    mcush_fatfs_check,
    mcush_fatfs_remove,
    mcush_fatfs_rename,
    mcush_fatfs_open,
    mcush_fatfs_read,
    mcush_fatfs_seek,
    mcush_fatfs_write,
    mcush_fatfs_flush,
    mcush_fatfs_close,
    mcush_fatfs_size,
    mcush_fatfs_list,
};


#endif
