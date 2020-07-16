/* MCUSH designed by Peng Shulin, all rights reserved. */
#if MCUSH_VFS
#include "mcush.h"
#include <sys/fcntl.h>


char *parse_file_flag( int flag, char *buf )
{
    buf[0] = 0;

    switch( flag & 0x03 )
    {
    case O_RDONLY: strcpy(buf, "r");  break;
    case O_WRONLY: strcpy(buf, "w");  break;
    case O_RDWR:
    default:       strcpy(buf, "rw");  break;
    }
    if( flag & O_CREAT )
        strcat(buf, "+");
    if( flag & O_APPEND )
        strcat(buf, "a");

    return buf;
}


int mcush_file_exists( const char *fname )
{
    int size;
    return mcush_size( fname, &size );
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
        if( r < 0 )
            break;
        bytes += r;
        if( r > 0 )
            crc = _crc8( buf, r, crc, crc8_table ); 
        if( r != _READ_BUF_SIZE )
            break;
    }
    mcush_close( fd );
    return bytes > 0 ? crc : -1;
}


int mcush_file_crc32( const char *fname )
{
    int fd = mcush_open( fname, "r" );
    uint8_t buf[_READ_BUF_SIZE];
    uint32_t crc=0;
    int r;
    int bytes=0;

    if( fd == 0 )
        return -1;
    while(1)
    { 
        r = mcush_read( fd, buf, _READ_BUF_SIZE );
        if( r < 0 )
            break;
        bytes += r;
        if( r > 0 )
            crc = _crc32( buf, r, crc, crc32_table ); 
        if( r != _READ_BUF_SIZE )
            break;
    }
    mcush_close( fd );
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


int mcush_file_read_line( int fd, char *line )
{
    int byte=0;
    int i;
    char *p=line, c;

    if( p == 0 )
        return 0;
    while( 1 )
    {
        i = mcush_read( fd, &c, 1 );
        if( i <= 0 )            /* EOF */
            break;
        byte++;
        if( c == '\n' )         /* stop */
            break;
        else if( c == '\r' )    /* ignore */
            continue;
        else                    /* append */
            *p++ = c;
    }
    if( byte == 0 )
        return 0;
    *p = 0;
    return 1;
}


int mcush_file_read_lines( int fd, char *line1, char *line2, char *line3, char *line4 )
{
    if( (line1==0) || (mcush_file_read_line( fd, line1 )==0) )
        return 0;
    if( (line2==0) || (mcush_file_read_line( fd, line2 )==0) )
        return 1;
    if( (line3==0) || (mcush_file_read_line( fd, line3 )==0) )
        return 2;
    if( (line4==0) || (mcush_file_read_line( fd, line4 )==0) )
        return 3;
    return 4;
}


int mcush_file_load_string( const char *fname, char *str, int size_limit )
{
    int fd = mcush_open( fname, "r" );
    int r;

    if( fd == 0 )
        return 0;
    r = mcush_read( fd, str, size_limit-1 );
    mcush_close( fd );
    if( r < 0 )
        return 0;
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
    if( r < 0 )
        return 0;
    return r==len ? 1 : 0;
}


/* read stripped head line from file */
int mcush_file_load_line( const char *fname, char *str, int size_limit )
{
    char buf[512], *p;
    int ret=0;

    if( mcush_file_load_string( fname, buf, 512 ) )
    {
        p = lstrip(buf);
        strip_line( &p );
        if( *p )
        {
            strncpy( str, p, size_limit-1 );
            str[size_limit-1] = 0;
            ret = 1;
        }
    }
    return ret;
}


int mcush_file_write_line( const char *fname, char *str )
{
    str = strip(str);
    if( *str == 0 )
        return 0;
    return mcush_file_write_string( fname, str );
}


/* read integer from file */
int mcush_file_load_int( const char *fname, int *val )
{
    char buf[64], *p=buf;
    int ret=0;

    if( mcush_file_load_string( fname, buf, 64 ) )
    {
        strip_line( &p ); 
        ret = parse_int( (const char*)p, val );
    }
    return ret;
}


int mcush_file_write_int( const char *fname, int val )
{
    char buf[64];

    sprintf( buf, "%d", val );
    return mcush_file_write_string( fname, buf );
}


/* read float value from file */
int mcush_file_load_float( const char *fname, float *val )
{
    char buf[64], *p=buf;
    int ret=0;

    if( mcush_file_load_string( fname, buf, 64 ) )
    {
        strip_line( &p ); 
        ret = parse_float( (const char*)p, val );
    }
    return ret;
}


int mcush_file_write_float( const char *fname, float val )
{
    char buf[64];

    sprintf( buf, "%f", val );
    return mcush_file_write_string( fname, buf );
}


/* write memory */
int mcush_file_write_memory( const char *fname, void *memory, int bytes )
{
    int fd = mcush_open( fname, "w+" );
    int r;

    if( fd == 0 )
        return 0;
    r = mcush_write( fd, memory, bytes );
    mcush_close( fd );
    return (r==bytes) ? 1 : 0;
}


#endif
