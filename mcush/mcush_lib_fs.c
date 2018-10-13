/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_VFS

int mcush_file_exists( const char *fname )
{
    int size;
    return mcush_size( fname, &size );
}


int mcush_file_load_string( const char *fname, char *str, int size_limit )
{
    int fd = mcush_open( fname, "r" );
    int r;

    if( fd == 0 )
        return 0;
    r = mcush_read( fd, str, size_limit-1 );
    mcush_close( fd );
    str[r] = 0;
    return r;
}


int mcush_file_write_string( const char *fname, char *str )
{
    int fd = mcush_open( fname, "w+" );
    int len = strlen(str);
    int r;

    if( fd == 0 )
        return 0;
    r = mcush_write( fd, str, len );
    mcush_close( fd );
    return r==len ? 1 : 0;
}


#define _READ_BUF_SIZE  128
int mcush_file_crc8( const char *fname )
{
    int fd = mcush_open( fname, "r" );
    uint8_t buf[_READ_BUF_SIZE];
    uint8_t crc=0;
    int r;
    int bytes=0;

    if( fd == 0 )
        return -1;
    while(1)
    { 
        r = mcush_read( fd, buf, _READ_BUF_SIZE );
        bytes += r;
        if( r > 0 )
            crc = _crc8( buf, r, crc, crc8_table ); 
        if( r != _READ_BUF_SIZE )
            break;
    }
    return bytes > 0 ? crc : -1;
}


int mcush_file_remove_retry( const char *fname, int retry_num )
{
    int retry;
    int size;
    int success=1;

    /* check if file exists */
    if( mcush_size( fname, &size ) )
    {
        /* try to remove file */
        for( retry=0; retry<retry_num; retry++ )
        {
            if( !mcush_remove( fname ) )
                break;
        }
        if( retry == retry_num )
            success = 0;
    }
    return success;
}


#endif
