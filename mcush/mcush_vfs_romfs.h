/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_ROMFS_H__
#define __MCUSH_VFS_ROMFS_H__


typedef struct {
    const char *name;
    const char *contents;
    const int len;
} romfs_file_t;


typedef struct {
    const romfs_file_t *file;
    int pos;
} romfs_file_desc_t;

#define ROMFS_FDS_NUM  3

//const romfs_file_t romfs_tab[]; 

int mcush_romfs_mount( void );
int mcush_romfs_mounted( void );
int mcush_romfs_info( int *total, int *used );
int mcush_romfs_format( void );
int mcush_romfs_check( void );
int mcush_romfs_remove( const char *path );
int mcush_romfs_rename( const char *old, const char *newPath );
int mcush_romfs_open( const char *pathname, const char *mode );
int mcush_romfs_read( int fh, char *buf, int len );
int mcush_romfs_seek( int fh, int offs, int where );
int mcush_romfs_write( int fh, char *buf, int len );
int mcush_romfs_flush( int fh );
int mcush_romfs_close( int fh );
int mcush_romfs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) );

extern const mcush_vfs_driver_t mcush_romfs_driver;

extern const romfs_file_t romfs_tab[];

#endif
