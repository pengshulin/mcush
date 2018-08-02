/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_FATFS
#include "ff.h"

#define static



uint8_t _inited;

int mcush_fatfs_mounted( void )
{
    return _inited;
}


int mcush_fatfs_mount( void )
{
    if( ! _inited )
    {
        hal_fatfs_init();
        _inited = 1;
        f_mount( 0, 0, 1 );
    }
    return 1;
}


int mcush_fatfs_umount( void )
{
    return 1;
}


int mcush_fatfs_check( void )
{
    return 0;
}


int mcush_fatfs_remove( const char *path )
{
    return 0;
}


int mcush_fatfs_rename( const char *old, const char *newPath )
{
    return 0;
}


//fatfs_flags parse_mode_flags( const char *mode )
//{
//    fatfs_flags flags = 0;
//    uint8_t r=0, w=0, c=0, a=0;
//    while( mode && *mode )
//    {
//        if( *mode == 'r' )
//            r = 1;
//        else if( *mode == 'w' )
//            w = 1;
//        else if( *mode == '+' )
//            c = 1;
//        else if( *mode == 'a' )
//            a = w = 1;
//        mode++;
//    }
//    if( r && w )
//        flags = SPIFFS_RDWR;
//    else if( r && !w )
//        flags = SPIFFS_RDONLY;
//    else if( w && !r )
//        flags = SPIFFS_WRONLY;
//    if( a )
//        flags |= SPIFFS_APPEND;
//    else if( w )
//        flags |= SPIFFS_TRUNC;
//    if( c )
//        flags |= SPIFFS_CREAT;
//    return flags;
//}


int mcush_fatfs_open( const char *path, const char *mode )
{
    return 0;
}


int mcush_fatfs_read( int fh, void *buf, int len )
{
    return 0;
}


int mcush_fatfs_write( int fh, void *buf, int len )
{
    return 0;
}


int mcush_fatfs_seek( int fh, int offs, int where )
{
    return 0;
}


int mcush_fatfs_flush( int fh )
{
    return 0;
}


int mcush_fatfs_close( int fh )
{
    return 0;
}


int mcush_fatfs_format( void )
{
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


static int mcush_fatfs_driver_errno;

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
