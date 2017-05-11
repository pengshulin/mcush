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
    MCUSH_VFS_FILE_NOT_EXIST,
    MCUSH_VFS_FAIL_TO_CREATE_FILE,
    MCUSH_VFS_FAIL_TO_OPEN_FILE,

} mcush_vfs_error_t;

typedef struct {
    int (*mcush_driver_finfo)( int *total, int *used );
    int (*mcush_driver_fsize)( const char *name, int *size );
    int (*mcush_driver_fsearch)( const char *fname );
    int (*mcush_driver_fremove)( const char *fname );
    int (*mcush_driver_frename)( const char *foldname, const char *fnewname );
    int (*mcush_driver_fopen)( const char *fname, const char *mode );
    int (*mcush_driver_fseek)( int fd, int offset, int where );
    int (*mcush_driver_fread)( int fd, void *buf, int len );
    int (*mcush_driver_fwrite)( int fd, void *buf, int len );
    int (*mcush_driver_fflush)( int fd );
    int (*mcush_driver_fclose)( int fd );

} mcush_vfs_driver_t;


typedef struct {
    const char *mount_point;
    mcush_vfs_driver_t *driver;
} mcush_vfs_volume_t;


typedef struct {
    const char *fname;
    const char *mode;
    mcush_vfs_driver_t *driver;
} mcush_vfs_file_descriptor_t;


int mcush_fmount( const char *mount_point, mcush_vfs_driver_t *driver );
int mcush_fumount( const char *mount_point );
int mcush_finfo( const char *name, int *total, int *used ); 
int mcush_fsize( const char *fname );
int mcush_fsearch( const char *fname );
int mcush_fremove( const char *fname );
int mcush_frename( const char *foldname, const char *fnewname );
int mcush_fopen( const char *fname, const char *mode );
int mcush_fseek( int fd, int offset, int where );
int mcush_fread( int fd, void *buf, int len );
int mcush_fwrite( int fd, void *buf, int len );
int mcush_fflush( int fd );
int mcush_fclose( int fd );

const char *mcush_fbasename( const char *fname );


#endif
