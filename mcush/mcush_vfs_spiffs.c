/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_SPIFFS
#define static


static spiffs _fs;
static char _work_buf[2*SPIFLASH_CFG_LOG_PAGE_SZ];
static char _fds[1024];
static char _cache_buf[4096];


int mcush_spiffs_mounted( void )
{
    return SPIFFS_mounted(&_fs) ? 1 : 0;
}


int mcush_spiffs_mount( void )
{
    spiffs_config cfg;

    if( SPIFFS_mounted(&_fs) )
        return 1;
#if SPIFLASH_AUTO_DETECT
    cfg.phys_size = 0;
#else
    cfg.phys_size = SPIFLASH_CFG_PHYS_SZ;
#endif
    cfg.phys_addr = SPIFLASH_CFG_PHYS_ADDR;
    cfg.phys_erase_block = SPIFLASH_CFG_PHYS_ERASE_SZ;
    cfg.log_block_size = SPIFLASH_CFG_LOG_BLOCK_SZ;
    cfg.log_page_size = SPIFLASH_CFG_LOG_PAGE_SZ;
    cfg.hal_read_f = (spiffs_read)hal_spiffs_flash_read;
    cfg.hal_write_f = (spiffs_write)hal_spiffs_flash_write;
    cfg.hal_erase_f = (spiffs_erase)hal_spiffs_flash_erase;

    hal_spiffs_flash_init();
#if SPIFLASH_AUTO_DETECT
    switch( hal_spiffs_flash_read_id() )
    {
    case 0xEF4014: cfg.phys_size = 1*1024*1024; break;  // W25Q80
    case 0xEF4015: cfg.phys_size = 2*1024*1024; break;  // W25Q16
    case 0xEF4016: cfg.phys_size = 4*1024*1024; break;  // W25Q32
    case 0xEF4017: cfg.phys_size = 8*1024*1024; break;  // W25Q64
    case 0xEF4018: cfg.phys_size = 16*1024*1024; break;  // W25Q128
    case 0xEF4019: cfg.phys_size = 32*1024*1024; break;  // W25Q256
    default: return 0; break;  // unknown
    }
#else
    if( hal_spiffs_flash_read_id() != HAL_SPIFFS_CHIPID )
        return 0;
#endif

    SPIFFS_mount( &_fs, &cfg, (u8_t*)_work_buf, (u8_t*)_fds, 
                   sizeof(_fds), (void*)_cache_buf,
                   sizeof(_cache_buf), 0 );
    return SPIFFS_mounted(&_fs) ? 1 : 0;
}


int mcush_spiffs_umount( void )
{
    if( !SPIFFS_mounted(&_fs) )
        return 1;
    SPIFFS_unmount( &_fs );
    return SPIFFS_mounted(&_fs) ? 0 : 1;
}


int mcush_spiffs_check( void )
{
    return SPIFFS_check( &_fs );
}


int mcush_spiffs_remove( const char *path )
{
    int ret = SPIFFS_remove( &_fs, path );
    return ret < 0 ? 0 : 1;
}


int mcush_spiffs_rename( const char *old, const char *newPath )
{
    int ret = SPIFFS_rename( &_fs, old, newPath );
    return ret < 0 ? 0 : 1; 
}


spiffs_flags parse_mode_flags( const char *mode )
{
    spiffs_flags flags = 0;
    uint8_t r=0, w=0, c=0, a=0;
    while( mode && *mode )
    {
        if( *mode == 'r' )
            r = 1;
        else if( *mode == 'w' )
            w = 1;
        else if( *mode == '+' )
            c = 1;
        else if( *mode == 'a' )
            a = w = 1;
        mode++;
    }
    if( r && w )
        flags = SPIFFS_RDWR;
    else if( r && !w )
        flags = SPIFFS_RDONLY;
    else if( w && !r )
        flags = SPIFFS_WRONLY;
    if( a )
        flags |= SPIFFS_APPEND;
    else if( w )
        flags |= SPIFFS_TRUNC;
    if( c )
        flags |= SPIFFS_CREAT;
    return flags;
}


int mcush_spiffs_open( const char *path, const char *mode )
{
    int ret = SPIFFS_open( &_fs, path, parse_mode_flags(mode), 0 );
    return ret < 0 ? 0 : ret;
}


int mcush_spiffs_read( int fh, void *buf, int len )
{
    int ret = SPIFFS_read( &_fs, fh, buf, len );
    return ret < 0 ? 0 : ret;
}


int mcush_spiffs_write( int fh, void *buf, int len )
{
    int ret = SPIFFS_write( &_fs, fh, buf, len );
    return ret < 0 ? 0 : ret;
}


int mcush_spiffs_seek( int fh, int offs, int where )
{
    return SPIFFS_lseek( &_fs, fh, offs, where );
}


int mcush_spiffs_flush( int fh )
{
    int ret = SPIFFS_fflush( &_fs, fh );
    return ret < 0 ? 0 : ret;
}


int mcush_spiffs_close( int fh )
{
    int ret = SPIFFS_close( &_fs, fh );
    return ret < 0 ? 0 : ret;
}


int mcush_spiffs_format( void )
{
    return SPIFFS_format( &_fs );
}


int mcush_spiffs_size( const char *name, int *size )
{
    spiffs_DIR dir;
    struct spiffs_dirent dirent;
    int found=0;

    if( ! SPIFFS_opendir( &_fs, "/", &dir ) )
        return 0;
    while( SPIFFS_readdir( &dir, &dirent ) )
    {
        if( strcmp( (const char*)dirent.name, name ) == 0 )
        {
            *size = dirent.size;
            found = 1;
            break;
        }
    } 
    SPIFFS_closedir( &dir );
    return found;
}


int mcush_spiffs_info( int *total, int *used )
{
    return SPIFFS_info( &_fs, (u32_t*)total, (u32_t*)used );
}


int mcush_spiffs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    spiffs_DIR dir;
    struct spiffs_dirent dirent;

    if( ! SPIFFS_opendir( &_fs, "/", &dir ) )
        return 0;
    while( SPIFFS_readdir( &dir, &dirent ) )
    {
        (*cb)( (char*)dirent.name, dirent.size, 0 ); 
    } 
    SPIFFS_closedir( &dir );
    return 1;
}


static int mcush_spiffs_driver_errno;

const mcush_vfs_driver_t mcush_spiffs_driver = {
    &mcush_spiffs_driver_errno,
    mcush_spiffs_mount,
    mcush_spiffs_umount,
    mcush_spiffs_info,
    mcush_spiffs_format,
    mcush_spiffs_check,
    mcush_spiffs_remove,
    mcush_spiffs_rename,
    mcush_spiffs_open,
    mcush_spiffs_read,
    mcush_spiffs_seek,
    mcush_spiffs_write,
    mcush_spiffs_flush,
    mcush_spiffs_close,
    mcush_spiffs_size,
    mcush_spiffs_list,
};


#endif
