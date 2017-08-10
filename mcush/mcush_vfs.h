/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_VFS_H__
#define __MCUSH_VFS_H__


#ifndef MCUSH_VFS_VOLUME_NUM
    #define MCUSH_VFS_VOLUME_NUM  2
#endif

#ifndef MCUSH_VFS_FILE_DESCRIPTOR_NUM
    #define MCUSH_VFS_FILE_DESCRIPTOR_NUM  3
#endif


typedef enum {
    MCUSH_VFS_OK = 0,
    MCUSH_VFS_VOLUME_NOT_MOUNTED,
    MCUSH_VFS_VOLUME_ERROR,
    MCUSH_VFS_PATH_NAME_ERROR,
    MCUSH_VFS_FILE_NOT_EXIST,
    MCUSH_VFS_FAIL_TO_CREATE_FILE,
    MCUSH_VFS_FAIL_TO_OPEN_FILE,
    MCUSH_VFS_RESOURCE_LIMIT,
} mcush_vfs_error_t;

typedef struct {
    int *errno;
    int (*mount)( void );
    int (*umount)( void );
    int (*info)( int *total, int *used );
    int (*format)( void );
    int (*check)( void );
    int (*remove)( const char *name );
    int (*rename)( const char *old_name, const char *new_name );
    int (*open)( const char *name, const char *mode );
    int (*read)( int fd, void *buf, int len );
    int (*seek)( int fd, int offset, int where );
    int (*write)( int fd, void *buf, int len );
    int (*flush)( int fd );
    int (*close)( int fd );
    int (*size)( const char *name, int *size );
    int (*list)( const char *path, void (*cb)(const char *name, int size, int mode) );
} mcush_vfs_driver_t;


typedef struct {
    const char *mount_point;
    mcush_vfs_driver_t *driver;
} mcush_vfs_volume_t;


typedef struct {
    int handle;
    mcush_vfs_driver_t *driver;
} mcush_vfs_file_descriptor_t;


int mcush_mount( const char *mount_point, mcush_vfs_driver_t *driver );
int mcush_umount( const char *mount_point );
int mcush_format( const char *mount_point );
int mcush_info( const char *mount_point, int *total, int *used ); 
int mcush_size( const char *pathname, int *size );
int mcush_search( const char *pathname );
int mcush_remove( const char *pathname );
int mcush_rename( const char *old_pathname, const char *newname );
int mcush_open( const char *pathname, const char *mode );
int mcush_seek( int fd, int offset, int where );
int mcush_read( int fd, void *buf, int len );
int mcush_write( int fd, void *buf, int len );
int mcush_flush( int fd );
int mcush_close( int fd );

const char *mcush_basename( const char *pathname );


#endif
