/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"



int sys_finfo( int *total, int *used )
{
    *total = *used = 0;
    return 0;
}

int sys_fsize( const char *name, int *size )
{
    *size = 0;
    return 0;
}

int sys_fsearch( const char *fname )
{
    return 0;
}

int sys_fremove( const char *fname )
{
    return 0;
}

int sys_frename( const char *foldname, const char *fnewname )
{
    return 0;
}

int sys_fopen( const char *fname, const char *mode )
{
    return 0;
}

int sys_fseek( int fd, int offset, int where )
{
    return 0;
}

int sys_fread( int fd, void *buf, int len )
{
    return 0;
}

int sys_fwrite( int fd, void *buf, int len )
{
    return 0;
}

int sys_fflush( int fd )
{
    return 0;
}

int sys_fclose( int fd )
{
    return 0;
}



mcush_vfs_driver_t fs_sys_driver = {
    sys_finfo,
    sys_fsize,
    sys_fsearch,
    sys_fremove,
    sys_frename,
    sys_fopen,
    sys_fseek,
    sys_fread,
    sys_fwrite,
    sys_fflush,
    sys_fclose
    };



