/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_ROMFS

int mcush_romfs_mount( void )
{
    return 1;
}


int mcush_romfs_umount( void )
{
    return 1;
}


int mcush_romfs_info( int *total, int *used )
{
    *total = *used = 0;
    return 0;
}


int mcush_romfs_format( void )
{
    return 0;
}


int mcush_romfs_check( void )
{
    return 0;
}


int mcush_romfs_remove( const char *path )
{
    return 0;
}


int mcush_romfs_rename( const char *old, const char *newPath )
{
    return 0;
}


int mcush_romfs_open( const char *path, const char *mode )
{
    return 0;
}


int mcush_romfs_read( int fh, void *buf, int len )
{
    return -1;
}


int mcush_romfs_write( int fh, void *buf, int len )
{
    return -1;
}


int mcush_romfs_seek( int fh, int offs, int where )
{
    return 0;
}


int mcush_romfs_flush( int fh )
{
    return 0;
}


int mcush_romfs_close( int fh )
{
    return 1;
}


int mcush_romfs_size( const char *name, int *size )
{
    return 0;
}


int mcush_romfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    return 1;
}





static int mcush_romfs_driver_errno;

const mcush_vfs_driver_t mcush_romfs_driver = {
    &mcush_romfs_driver_errno,
    mcush_romfs_mount,
    mcush_romfs_umount,
    mcush_romfs_info,
    mcush_romfs_format,
    mcush_romfs_check,
    mcush_romfs_remove,
    mcush_romfs_rename,
    mcush_romfs_open,
    mcush_romfs_read,
    mcush_romfs_seek,
    mcush_romfs_write,
    mcush_romfs_flush,
    mcush_romfs_close,
    mcush_romfs_size,
    mcush_romfs_list,
    };

#endif

