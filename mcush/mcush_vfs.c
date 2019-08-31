/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mcush.h"

#if MCUSH_VFS
#define FD_RESERVED  10
mcush_vfs_volume_t vfs_vol_tab[MCUSH_VFS_VOLUME_NUM];
mcush_vfs_file_descriptor_t vfs_fd_tab[MCUSH_VFS_FILE_DESCRIPTOR_NUM];

#if MCUSH_VFS_STATISTICS
mcush_vfs_statistics_t vfs_stat;
#endif

int mcush_mount( const char *mount_point, const mcush_vfs_driver_t *driver )
{
    int i;
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_mount++;
#endif
    if( *mount_point == '/' )
        mount_point++;
    /* check if the mount_point is used */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( vfs_vol_tab[i].mount_point && 
            (strcmp(vfs_vol_tab[i].mount_point, mount_point) == 0) )
        {
            return 1;
        }
    }
    /* insert into free space */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( ! vfs_vol_tab[i].mount_point )
        {
            if( driver->mount() )
            {
                vfs_vol_tab[i].mount_point = mount_point;
                vfs_vol_tab[i].driver = driver;
                return 1;
            } 
            else
                return 0;
        }
    }
    return 0;
}

int mcush_umount( const char *mount_point )
{
    int i;
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_umount++;
#endif
    if( *mount_point == '/' )
        mount_point++;
    /* check if the mount_point is used */
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( vfs_vol_tab[i].mount_point && 
            (strcmp(vfs_vol_tab[i].mount_point, mount_point) == 0) )
        {
            if( vfs_vol_tab[i].driver->umount() )
            {
                vfs_vol_tab[i].mount_point = 0;
                vfs_vol_tab[i].driver = 0;
                return 1;
            }
            else
                return 0;
        }
    }
    return 0;
}

    
int get_mount_point( const char *pathname, char *mount_point )
{
    if( *pathname++ != '/' )
        return 0;
    while( *pathname && isalnum((int)*pathname) && *pathname != '/' )
        *mount_point++ = *pathname++;
    *mount_point = 0;
    return 1;
}


int get_file_name( const char *pathname, char *file_name )
{
    if( *pathname++ != '/' )
        return 0;
    while( *pathname && isalnum((int)*pathname) && *pathname != '/' )
        pathname++;
    if( *pathname++ != '/' )
        return 0;
    while( *pathname )
        *file_name++ = *pathname++;
    *file_name = 0;
    return 1;
}


mcush_vfs_volume_t *get_vol( const char *name )
{
    int i;
    char mount_point[16];

    if( ! get_mount_point( name, mount_point ) )
        return 0;
    for( i=0; i<MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( vfs_vol_tab[i].mount_point && 
            (strcmp(vfs_vol_tab[i].mount_point, mount_point) == 0) )
        {
            return &vfs_vol_tab[i];
        }
    }
    return 0; 
}


int mcush_info( const char *pathname, int *total, int *used )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    if( !vol )
        return 0;
    return vol->driver->info( total, used );
}


int mcush_size( const char *pathname, int *size )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    char file_name[32];
    if( ! get_file_name( pathname, file_name ) )
        return 0;
    if( !vol )
        return 0;
    return vol->driver->size( file_name, size );
}


int mcush_search( const char *fname )
{
    return 0;
}


int mcush_remove( const char *pathname )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    char file_name[32];
    if( ! get_file_name( pathname, file_name ) )
        return 0;
    if( ! vol )
        return 0;
    return vol->driver->remove( file_name );
}


int mcush_rename( const char *old_pathname, const char *new_name )
{
    mcush_vfs_volume_t *vol = get_vol(old_pathname);
    char file_name[32];
    if( ! get_file_name( old_pathname, file_name ) )
        return 0;
    if( ! vol )
        return 0;
    return vol->driver->rename( file_name, new_name );
}


int mcush_open( const char *pathname, const char *mode )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    char file_name[32];
    int fd, i;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_open++;
#endif
    if( ! get_file_name( pathname, file_name ) )
        goto err;
    if( ! vol )
        goto err;
    portENTER_CRITICAL();
    for( i=0; i<MCUSH_VFS_FILE_DESCRIPTOR_NUM; i++ )
    {
        if( vfs_fd_tab[i].driver == NULL ) 
        {
            vfs_fd_tab[i].driver = vol->driver;
            break;
        }
    }
    portEXIT_CRITICAL();
    if( i >= MCUSH_VFS_FILE_DESCRIPTOR_NUM )
    {  
        *vol->driver->err = MCUSH_VFS_RESOURCE_LIMIT;
        goto err;
    }
    else
    {
        fd = vol->driver->open( file_name, mode );
        if( fd )
        { 
            vfs_fd_tab[i].handle = fd;
        }
        else
        {
            *vol->driver->err = MCUSH_VFS_FAIL_TO_OPEN_FILE;
            vfs_fd_tab[i].driver = NULL;
            goto err;
        }
    }
    return i+FD_RESERVED;
err:
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_open_err++;
#endif
    return 0; 
}


int mcush_seek( int fd, int offset, int where )
{
    const mcush_vfs_driver_t *driver;
    int ret;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_seek++;
#endif
    if( fd == 0 )
        return 0;

    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
        goto err;

    driver = vfs_fd_tab[fd].driver;
    if( driver == NULL )
        goto err;
    ret = driver->seek( vfs_fd_tab[fd].handle, offset, where );
    return ret;
err:
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_seek_err++;
#endif
    return 0;
}


int mcush_read( int fd, void *buf, int len )
{
    int ret, unget=0;
    const mcush_vfs_driver_t *driver;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_read++;
#endif
    /* fd == 0: map to shell input */
    if( fd == 0 )
        return shell_read( buf, len );

    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
        goto err;

    if( len <= 0 )
        return 0;
    if( vfs_fd_tab[fd].unget_char )
    {
        *(char*)buf++ = vfs_fd_tab[fd].unget_char;
        vfs_fd_tab[fd].unget_char = 0;
        len--;
        unget = 1;
    }
    driver = vfs_fd_tab[fd].driver;
    if( driver == NULL )
        goto err;
    ret = driver->read( vfs_fd_tab[fd].handle, buf, len );
    if( ret < 0 )
        return unget ? 1 : -1;
    else
        return unget ? ret+1 : ret;
err:
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_read_err++;
#endif
    return -1;
}


int mcush_write( int fd, void *buf, int len )
{
    const mcush_vfs_driver_t *driver;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_write++;
#endif
    /* fd == 1/2: map to shell output */
    if( fd == 1 || fd == 2 )
    {
        shell_write( buf, len );
        return len;
    }
    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
        goto err;
    driver = vfs_fd_tab[fd].driver;
    if( driver )
        return driver->write( vfs_fd_tab[fd].handle, buf, len );
err:
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_write_err++;
#endif
    return -1; 
}


int mcush_flush( int fd )
{
    const mcush_vfs_driver_t *driver;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_flush++;
#endif
    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
    {
#if MCUSH_VFS_STATISTICS
        vfs_stat.count_flush_err++;
#endif
        return 0;
    }
    driver = vfs_fd_tab[fd].driver;
    if( driver )
        *driver->err = driver->flush( vfs_fd_tab[fd].handle );
    return 1;
}


int mcush_close( int fd )
{
    const mcush_vfs_driver_t *driver;

#if MCUSH_VFS_STATISTICS
    vfs_stat.count_close++;
#endif
    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
        goto err;
    driver = vfs_fd_tab[fd].driver;
    if( driver )
    {
        *driver->err = driver->close( vfs_fd_tab[fd].handle );
        portENTER_CRITICAL();
        vfs_fd_tab[fd].driver = NULL;
        vfs_fd_tab[fd].handle = 0;
        portEXIT_CRITICAL();
        return 1;
    }
err:
#if MCUSH_VFS_STATISTICS
    vfs_stat.count_close_err++;
#endif
    return 0;
}


int mcush_list( const char *path, void (*cb)(const char *name, int size, int mode) )
{
    mcush_vfs_volume_t *vol = get_vol(path);
    char mount_point[16];
    char file_name[32];

    if( ! vol )
        return 0;
    if( ! get_mount_point( path, mount_point ) )
        return 0;
    if( ! get_file_name( path, &file_name[1] ) )
        strcpy( file_name, "/" );
   
    return vol->driver->list( file_name, cb ); 
}


int mcush_getc( int fd )
{
    char c;
    return shell_read_char( &c );
}


int mcush_ungetc( int fd, char c )
{
    fd -= FD_RESERVED;
    if( (fd < 0) || (fd >= MCUSH_VFS_FILE_DESCRIPTOR_NUM) )
        return 0;
    if( vfs_fd_tab[fd].unget_char )
        return 0;
    vfs_fd_tab[fd].unget_char = c;
    return 1;
}


int mcush_putc( int fd, char c )
{
    if( mcush_write( fd, &c, 1 ) == 1 )
        return c;
    else
        return -1;
}


int mcush_puts( int fd, const char *buf )
{
    return mcush_write( fd, (void*)buf, strlen(buf) ); 
}


int mcush_printf( int fd, const char *fmt, ... )
{
    va_list ap;
    int n;
    char buf[256];

    va_start( ap, fmt );
    n = vsprintf( buf, fmt, ap );
    n = mcush_write( fd, buf, n );
    va_end( ap );
    return n;
}


const char *mcush_basename( const char *fname )
{
    return 0;
}


#endif

 
int fprintf(FILE *stream, const char *format, ...)
{
    va_list vargs;
    int charCnt;
    char str[1024];

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


/* TODO: check if 's' is defined as __nonnull__ */
int fputs(const char *s, FILE *stream)
{
    int l;

    if( stream == stdout || stream == stderr )
    {
        l = strlen(s);
        shell_write(s, l);
        return l;
    }
    else
        return EOF;
}


