/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mcush.h"

#if MCUSH_VFS

mcush_vfs_volume_t vfs_vol_tab[MCUSH_VFS_VOLUME_NUM];
mcush_vfs_file_descriptor_t vfs_fd_tab[MCUSH_VFS_FILE_DESCRIPTOR_NUM];


int mcush_mount( const char *mount_point, const mcush_vfs_driver_t *driver )
{
    int i;
    if( *mount_point == '/' )
        mount_point++;
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
            if( driver->mount() )
            {
                vfs_vol_tab[i].mount_point = mount_point;
                vfs_vol_tab[i].driver = driver;
                return 1;
            } 
        }
    }
    return 0;
}

int mcush_umount( const char *mount_point )
{
    int i;
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
    while( *pathname && isalnum((int)*pathname) )
        *mount_point++ = *pathname++;
    *mount_point = 0;
    return 1;
}


int get_file_name( const char *pathname, char *file_name )
{
    if( *pathname++ != '/' )
        return 0;
    while( *pathname && isalnum((int)*pathname) )
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
    *vol->driver->errno = vol->driver->info( total, used );
    return *vol->driver->errno ? 0 : 1;
}


int mcush_size( const char *pathname, int *size )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    char file_name[32];
    if( ! get_file_name( pathname, file_name ) )
        return 0;
    if( !vol )
        return 0;
    *vol->driver->errno = vol->driver->size( file_name, size );
    return *vol->driver->errno ? 0 : 1;
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
    *vol->driver->errno = vol->driver->remove( file_name );
    return *vol->driver->errno ? 0 : 1;
}


int mcush_rename( const char *old_pathname, const char *new_name )
{
    mcush_vfs_volume_t *vol = get_vol(old_pathname);
    char file_name[32];
    if( ! get_file_name( old_pathname, file_name ) )
        return 0;
    if( ! vol )
        return 0;
    *vol->driver->errno = vol->driver->rename( file_name, new_name );
    return *vol->driver->errno ? 0 : 1;
}


int mcush_open( const char *pathname, const char *mode )
{
    mcush_vfs_volume_t *vol = get_vol(pathname);
    char file_name[32];
    int fd, i;
    if( ! get_file_name( pathname, file_name ) )
        return 0;
    if( ! vol )
        return 0;
    fd = vol->driver->open( file_name, mode );
    if( fd )
    { 
        for( i=0; i<MCUSH_VFS_FILE_DESCRIPTOR_NUM; i++ )
        {
            if( ! vfs_fd_tab[i].handle ) 
            {
                vfs_fd_tab[i].handle = fd;
                vfs_fd_tab[i].driver = vol->driver;
                return i+1;
            }
        }
        *vol->driver->errno = MCUSH_VFS_RESOURCE_LIMIT;
    }
    else
        *vol->driver->errno = MCUSH_VFS_FAIL_TO_OPEN_FILE;
    return 0;
}


int mcush_seek( int fd, int offset, int where )
{
    return 0;
}


int mcush_read( int fd, void *buf, int len )
{
    fd -= 1;
    return vfs_fd_tab[fd].driver->read( vfs_fd_tab[fd].handle, buf, len );
}


int mcush_write( int fd, void *buf, int len )
{
    fd -= 1;
    return vfs_fd_tab[fd].driver->write( vfs_fd_tab[fd].handle, buf, len );
}


int mcush_flush( int fd )
{
    fd -= 1;
    *vfs_fd_tab[fd].driver->errno = vfs_fd_tab[fd].driver->flush( vfs_fd_tab[fd].handle );
    return 1;
}


int mcush_close( int fd )
{
    fd -= 1;
    *vfs_fd_tab[fd].driver->errno = vfs_fd_tab[fd].driver->close( vfs_fd_tab[fd].handle );
    vfs_fd_tab[fd].driver = 0;
    vfs_fd_tab[fd].handle = 0;
    return 1;
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



const char *mcush_basename( const char *fname )
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
