/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mcush.h"

#if MCUSH_VFS

mcush_vfs_volume_t vfs_vol_tab[MCUSH_VFS_VOLUME_NUM];
mcush_vfs_file_descriptor_t vfs_fd_tab[MCUSH_VFS_FILE_DESCRIPTOR_NUM];


int mcush_fmount( const char *mount_point, mcush_vfs_driver_t *driver )
{
    int i;
    /* check if the mount_point is used */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( vfs_vol_tab[i].mount_point && 
            (strcmp(vfs_vol_tab[i].mount_point, mount_point) == 0) )
            return 1;
    }
    /* insert into free space */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( ! vfs_vol_tab[i].mount_point )
        {
            vfs_vol_tab[i].mount_point = mount_point;
            vfs_vol_tab[i].driver = driver;
            return 0;
        }
    }
    return 1;
}

int mcush_fumount( const char *mount_point )
{
    int i;
    /* check if the mount_point is used */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( vfs_vol_tab[i].mount_point && 
            (strcmp(vfs_vol_tab[i].mount_point, mount_point) == 0) )
        {
            /* umount it */ 
            return 0;
        }
    }
    return -1;
}

int mcush_finfo( const char *name, int *total, int *used )
{
    return 0;
}

int mcush_fsize( const char *fname )
{
    return 0;
}

int mcush_fsearch( const char *fname )
{
    return 0;
}

int mcush_fremove( const char *fname )
{
    return 0;
}

int mcush_frename( const char *foldname, const char *fnewname )
{
    return 0;
}

int mcush_fopen( const char *fname, const char *mode )
{
    return 0;
}

int mcush_fseek( int fd, int offset, int where )
{
    return 0;
}

int mcush_fread( int fd, void *buf, int len )
{
    return 0;
}

int mcush_fwrite( int fd, void *buf, int len )
{
    return 0;
}

int mcush_fflush( int fd )
{
    return 0;
}

int mcush_fclose( int fd )
{
    return 0;
}


const char *mcush_fbasename( const char *fname )
{
    return 0;
}


#endif

 
int fprintf(FILE *stream, const char *format, ...)
{
    va_list vargs;
    int charCnt;
    char str[256];

    va_start(vargs,format);
    if( stream == stdout || stream == stderr )
        charCnt = vsprintf( str, format, vargs);
    else
        charCnt = 0;
    va_end(vargs);
    return charCnt;
}


int fputc(int c, FILE *stream)
{
    if( stream == stdout || stream == stderr )
    {
        shell_write_char( c );
        return c;
    }
    else
        return EOF;
}


int fputs(const char *s, FILE *stream)
{
    int l;
    if( s && (stream == stdout || stream == stderr) )
    {
        l = strlen(s);
        shell_write(s, l);
        return l;
    }
    else
        return EOF;
}
