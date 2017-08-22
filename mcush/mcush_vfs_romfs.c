/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <string.h>

#if MCUSH_ROMFS

#define static

const char file_copyright[] = "MCUSH designed by Peng Shulin, all rights reserved.";
const char file_readme[] = "https://github.com/pengshulin/mcush";
const char file_demo[] = "echo 'blink leds'\nled -t -i0\nwait 200\nled -t -i0\n";
const romfs_file_t romfs_tab[] = {
    { "copyright", file_copyright, sizeof(file_copyright)-1 },
    { "readme", file_readme, sizeof(file_readme)-1 },
    { "demo", file_demo, sizeof(file_demo)-1 },
    { 0 } };

static romfs_file_desc_t _fds[ROMFS_FDS_NUM];


int mcush_romfs_mount( void )
{
    memset( (void*)&_fds, 0, ROMFS_FDS_NUM * sizeof(romfs_file_desc_t) );
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


int mcush_romfs_open( const char *pathname, const char *mode )
{
    const romfs_file_t *f = romfs_tab;
    int i;
    
    for( i=0; i<ROMFS_FDS_NUM; i++ )
    {
        if( ! _fds[i].file )
        {
            while( f->name )
            {
                if( strcmp(f->name, pathname)==0 )
                {
                    _fds[i].file = f;
                    _fds[i].pos = 0;
                    return i+1;
                }
                f++;
            }
        }
    }
    return 0;
}


int mcush_romfs_read( int fh, void *buf, int len )
{
    int i;

    fh -= 1;
    if( !_fds[fh].file ) 
        return -1;

    i = _fds[fh].file->len - _fds[fh].pos;
    if( i < 0 )
        i = 0;
    if( i > len )
        i = len;
    if( i )
        memcpy( buf, (const void*)&_fds[fh].file->contents[_fds[fh].pos], i ); 
    return i;
}


int mcush_romfs_write( int fh, void *buf, int len )
{
    return -1;
}


int mcush_romfs_seek( int fh, int offs, int where )
{
    int newpos;
    fh -= 1;
    if( !_fds[fh].file ) 
        return -1;

    if( where > 0 )
        newpos = _fds[fh].pos + offs;
    else if( where < 0 )
        newpos = _fds[fh].file->len - offs;
    else
        newpos = offs;
       
    if( newpos < 0 )
        return -1;
    else if( newpos > _fds[fh].file->len )
        return -1;
    _fds[fh].pos = newpos;
    return newpos;
}


int mcush_romfs_flush( int fh )
{
    return 0;
}


int mcush_romfs_close( int fh )
{
    _fds[fh-1].file = 0;
    return 1;
}


int mcush_romfs_size( const char *name, int *size )
{
    const romfs_file_t *f = romfs_tab;
    while( f->name )
    {
        if( strcmp(f->name, name) == 0 )
        {
            *size = f->len;
            return 1;
        }
        f++;
    }
    return 0;
}


int mcush_romfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    const romfs_file_t *f = romfs_tab;

    if( strcmp(pathname, "/") != 0 )
        return 0;

    while( f->name )
    {
        (*cb)( f->name, f->len, 0 );
        f++;
    }
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

