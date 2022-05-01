/* POSIX File System (for test/simulation on linux) */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "mcush_vfs.h"

#if MCUSH_POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>


#define ROOT_DIR   ".fs"

os_mutex_handle_t mutex_posix;
static uint8_t _mounted;
static FILE *_fds[POSIX_FDS_NUM];


int mcush_posix_lock_check(void)
{
    if( mutex_posix != NULL )
        return 1;
    mutex_posix = os_mutex_create();
    return (mutex_posix != NULL) ? 1 : 0;
}


void mcush_posix_lock(void)
{
    if( mcush_posix_lock_check() && os_is_running() )
        os_mutex_get( mutex_posix,  -1 );
}


void mcush_posix_unlock(void)
{
    if( mcush_posix_lock_check() && os_is_running() )
        os_mutex_put( mutex_posix );
}


int mcush_posix_mounted( void )
{
    return _mounted;
}

  
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

int mcush_posix_mount( void )
{
    DIR *root;
    int ret;
    int i;

    if( _mounted )
        return 1;

    /* create (if not exist) local directory '.fs' for cache */
    if( (root = opendir(ROOT_DIR)) == NULL )
    {
        ret = mkdir(ROOT_DIR, MODE);
        if( ret != 0 )
        {
            printf("failed to created .fs directory (%d)\n", ret);
            return 0;
        }
        printf(".fs directory created\n");
    }
    else
        closedir(root);
    for( i=0; i<POSIX_FDS_NUM; i++ )
         _fds[i] = NULL;
    _mounted = 1;
    return 1;
}


int mcush_posix_umount( void )
{
    int i;

    if( _mounted )
    {
        for( i=0; i<POSIX_FDS_NUM; i++ )
        {
            if( _fds[i] != NULL )
            {
                fclose(_fds[i]);
                _fds[i] = NULL;
            }
        } 
    }
    _mounted = 0;
    return 1;
}


int mcush_posix_check( void )
{
    return 1;
}


int mcush_posix_remove( const char *path )
{
    char buf[128];
    printf("remove: %s\n", path);
    sprintf(buf, "%s/%s", ROOT_DIR, path);
    if( remove(buf) == 0 )
        return 1;
    return 0;
}


int mcush_posix_rename( const char *old, const char *newPath )
{
    char buf[128], buf2[128];
    printf("rename: %s to %s\n", old, newPath);
    sprintf(buf, "%s/%s", ROOT_DIR, old);
    sprintf(buf2, "%s/%s", ROOT_DIR, newPath);
    if( rename(buf, buf2) == 0 )
        return 1;
    return 0;
}


int mcush_posix_open( const char *path, const char *mode )
{
    char buf[128];
    FILE *file=NULL;
    int i;

    printf("open: %s (%s)\n", path, mode);
    sprintf(buf, "%s/%s", ROOT_DIR, path);

    for( i=0; i<POSIX_FDS_NUM; i++ )
    { 
        if( _fds[i] == NULL )
        {
            file = fopen( buf, mode );
            if( file == NULL )
                return 0;
            _fds[i] = file;
            return i+1;
        }
    }
    return 0;
}


int mcush_posix_read( int fh, char *buf, int len )
{
    if( fh <= 0 )
        return 0;
    fh -= 1;
    if( _fds[fh] == NULL ) 
        return -1;
    printf("read(%d): %d\n", fh, len);
    return fread( buf, 1, len, _fds[fh] );
}


int mcush_posix_write( int fh, char *buf, int len )
{
    if( fh <= 0 )
        return 0;
    fh -= 1;
    if( _fds[fh] == NULL ) 
        return -1;
    printf("write(%d): %d\n", fh, len);
    return fwrite( buf, 1, len, _fds[fh] );
}


int mcush_posix_seek( int fh, int offs, int where )
{
    if( fh <= 0 )
        return 0;
    fh -= 1;
    if( _fds[fh] == NULL ) 
        return 0;
    return (fseek( _fds[fh], offs, where ) == 0) ? 1 : 0;
}


int mcush_posix_flush( int fh )
{
    if( fh <= 0 )
        return 0;
    fh -= 1;
    if( _fds[fh] == NULL ) 
        return 0;
    return (fflush( _fds[fh] ) == 0) ? 1 : 0;
}


int mcush_posix_close( int fh )
{
    if( fh <= 0 )
        return 0;
    fh -= 1;
    if( _fds[fh] == NULL ) 
        return 0;
    fclose( _fds[fh] );
    _fds[fh] = NULL;
    return 1;
}


int mcush_posix_format( void )
{
    return 0;
}


int mcush_posix_size( const char *name, int *size )
{
    DIR *root = opendir(ROOT_DIR);
    struct dirent *ent=NULL;
    struct stat statbuf;
    char buf[128];
    int found=0;

    printf("size: %s\n", name);
    sprintf(buf, "%s/%s", ROOT_DIR, name);

    if( root == NULL )
        return 0;
    while( (ent = readdir(root)) != NULL )
    {
        if( strcmp(ent->d_name, name) != 0 )
            continue;
        sprintf( buf, "%s/%s", ROOT_DIR, ent->d_name );
        lstat( buf, &statbuf );
        if( S_ISDIR(statbuf.st_mode) )
            continue;
        *size = statbuf.st_size;
        found = 1;
    }
    closedir(root);
    return found;
}


int mcush_posix_info( int *total, int *used )
{
    DIR *root = opendir(ROOT_DIR);
    struct dirent *ent=NULL;
    struct stat statbuf;
    int count_total=0, count_used=0;
    char buf[128];

    if( root == NULL )
        return 0;
    while( (ent = readdir(root)) != NULL )
    {
        if( strcmp(ent->d_name, ".") == 0 )
            continue;
        if( strcmp(ent->d_name, "..") == 0 )
            continue;
        sprintf( buf, "%s/%s", ROOT_DIR, ent->d_name );
        lstat( buf, &statbuf );
        if( S_ISDIR(statbuf.st_mode) )
            continue;
        count_total += statbuf.st_blksize * statbuf.st_blocks;
        count_used += statbuf.st_size;
        //printf("%ld/%ld/%ld\n", statbuf.st_size, statbuf.st_blksize, statbuf.st_blocks );
    }
    closedir(root);
    *total = count_total;
    *used = count_used;
    return 1;
}


int mcush_posix_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    DIR *root = opendir(ROOT_DIR);
    struct dirent *ent=NULL;
    struct stat statbuf;
    char buf[128];

    (void)pathname;
    if( root == NULL )
        return 0;
    printf("posix list\n");
    while( (ent = readdir(root)) != NULL )
    {
        if( strcmp(ent->d_name, ".") == 0 )
            continue;
        if( strcmp(ent->d_name, "..") == 0 )
            continue;
        sprintf( buf, "%s/%s", ROOT_DIR, ent->d_name );
        lstat( buf, &statbuf );
        if( S_ISDIR(statbuf.st_mode) )
            continue;
        printf("\t%s, %ld, %ld, %ld\n", ent->d_name, statbuf.st_size, statbuf.st_blksize, statbuf.st_blocks );
        (*cb)( (char*)ent->d_name, statbuf.st_size, 0 ); 
    }
    closedir(root);
    return 1;
}


static int mcush_posix_driver_errno;

const mcush_vfs_driver_t mcush_posix_driver = {
    &mcush_posix_driver_errno,
    mcush_posix_mount,
    mcush_posix_umount,
    mcush_posix_info,
    mcush_posix_format,
    mcush_posix_check,
    mcush_posix_remove,
    mcush_posix_rename,
    mcush_posix_open,
    mcush_posix_read,
    mcush_posix_seek,
    mcush_posix_write,
    mcush_posix_flush,
    mcush_posix_close,
    mcush_posix_size,
    mcush_posix_list,
};


#endif
