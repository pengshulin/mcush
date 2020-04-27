/* Flash Configuration File System */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <string.h>

#if MCUSH_FCFS

#ifndef FCFS_ADDR
#error "FCFS_ADDR not assigned!"
#endif

//#define static

/* flash layout:
 (FCFS_ADDR)  INDEX            FILE1          FILE2   
 index --> ------------ --> ---------- --> ----------  ...
          | magic_code |   | fname    |   | fname    |
          |------------|   |----------|   |----------|
          | uid        |   | contents |   | contents |
          |------------|    ----------     ----------
          | file1      |
          |------------|
          | file2      |
          |------------|
          | ...        |
          |------------|
          | NULL(end)  |
          |------------|
          | extra info |
           ------------
 */


static fcfs_file_desc_t _fds[FCFS_FDS_NUM];

int mcush_fcfs_mount( void )
{
    memset( (void*)&_fds, 0, FCFS_FDS_NUM * sizeof(fcfs_file_desc_t) );
    if( *(int*)FCFS_ADDR != FCFS_MAGIC_CODE )
        return 0;
    else
        return 1;
}


int mcush_fcfs_umount( void )
{
    return 1;
}


int mcush_fcfs_info( int *total, int *used )
{
    *total = *used = 0;
    return 0;
}


int mcush_fcfs_format( void )
{
    return hal_fcfs_erase( (int*)FCFS_ADDR ) ? 1 : 0;
}


int mcush_fcfs_check( void )
{
    return 0;
}


int mcush_fcfs_remove( const char *path )
{
    return 0;
}


int mcush_fcfs_rename( const char *old, const char *newPath )
{
    return 0;
}


int mcush_fcfs_open( const char *pathname, const char *mode )
{
    fcfs_file_t *f = (fcfs_file_t*)(FCFS_ADDR+4+FCFS_UID_LEN);
    int i, j;
    
    for( i=0; i<FCFS_FDS_NUM; i++ )
    {
        if( ! _fds[i].file )
        {
            while( (f->offset != 0) && (f->offset != 0xFFFF) )
            {
                if( strcmp((char*)(FCFS_ADDR+f->offset), pathname)==0 )
                {
                    _fds[i].file = f;
                    j = strlen((char*)(FCFS_ADDR+f->offset));
                    _fds[i].contents = (const char *)(FCFS_ADDR+f->offset+j+1);
                    _fds[i].pos = 0;
                    return i+1;
                }
                f++;
            }
        }
    }
    return 0;
}


int mcush_fcfs_read( int fh, void *buf, int len )
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
        memcpy( buf, (const void*)&_fds[fh].contents[_fds[fh].pos], i ); 
    return i;
}


int mcush_fcfs_write( int fh, void *buf, int len )
{
    return -1;
}


int mcush_fcfs_seek( int fh, int offs, int where )
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


int mcush_fcfs_flush( int fh )
{
    return 0;
}


int mcush_fcfs_close( int fh )
{
    _fds[fh-1].file = 0;
    return 1;
}


int mcush_fcfs_size( const char *name, int *size )
{
    fcfs_file_t *f = (fcfs_file_t *)(FCFS_ADDR+4+FCFS_UID_LEN);
    while( (f->offset != 0) && (f->offset != 0xFFFF) )
    {
        if( strcmp((char*)(FCFS_ADDR+f->offset), name) == 0 )
        {
            *size = f->len;
            return 1;
        }
        f++;
    }
    return 0;
}


int mcush_fcfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    fcfs_file_t *f = (fcfs_file_t *)(FCFS_ADDR+4+FCFS_UID_LEN);

    if( strcmp(pathname, "/") != 0 )
        return 0;

    while( (f->offset != 0) && (f->offset != 0xFFFF) )
    {
        (*cb)( (char*)(FCFS_ADDR+f->offset), f->len, 0 );
        f++;
    }
    return 1;
}





static int mcush_fcfs_driver_errno;

const mcush_vfs_driver_t mcush_fcfs_driver = {
    &mcush_fcfs_driver_errno,
    mcush_fcfs_mount,
    mcush_fcfs_umount,
    mcush_fcfs_info,
    mcush_fcfs_format,
    mcush_fcfs_check,
    mcush_fcfs_remove,
    mcush_fcfs_rename,
    mcush_fcfs_open,
    mcush_fcfs_read,
    mcush_fcfs_seek,
    mcush_fcfs_write,
    mcush_fcfs_flush,
    mcush_fcfs_close,
    mcush_fcfs_size,
    mcush_fcfs_list,
    };

#endif

