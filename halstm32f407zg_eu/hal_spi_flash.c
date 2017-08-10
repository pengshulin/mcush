#include <stdarg.h>
#include "hal.h"
#include "spi_flash.h"
#include "FreeRTOS.h"
#include "task.h"

#define static

#define SPIFLASH_CFG_PHYS_SZ        (1024*1024*8)
#define SPIFLASH_CFG_PHYS_ERASE_SZ  (65536)
#define SPIFLASH_CFG_PHYS_ADDR      (0)
#define SPIFLASH_CFG_LOG_BLOCK_SZ   (65536)
#define SPIFLASH_CFG_LOG_PAGE_SZ    (256)

static spiffs _fs;
static char _work_buf[2*SPIFLASH_CFG_LOG_PAGE_SZ];
static char _fds[1024];
static char _cache_buf[4096];


//void hal_spiffs_debug( char *fmt, ... )
//{
//    va_list ap;
//    int n;
//    char buf[256];
//
//    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED )
//        return;
//    va_start( ap, fmt );
//    n = vsprintf( buf, fmt, ap );
//    shell_write( buf, n );
//    va_end( ap );
//}



s32_t *hal_spi_flash_read(u32_t addr, u32_t size, u8_t *dst)
{
    sFLASH_ReadBuffer(dst, addr, size);
    return SPIFFS_OK;
}


s32_t *hal_spi_flash_write(u32_t addr, u32_t size, u8_t *src)
{
    sFLASH_WriteBuffer(src, addr, size);
    return SPIFFS_OK;
}


s32_t *hal_spi_flash_erase(u32_t addr, u32_t size)
{
    sFLASH_EraseSector(addr);
    return SPIFFS_OK;
}

int hal_spiffs_mounted( void )
{
    return SPIFFS_mounted(&_fs) ? 1 : 0;
}


int hal_spiffs_mount( void )
{
    spiffs_config cfg;

    if( SPIFFS_mounted(&_fs) )
        return 1;
    cfg.phys_size = SPIFLASH_CFG_PHYS_SZ;
    cfg.phys_addr = SPIFLASH_CFG_PHYS_ADDR;
    cfg.phys_erase_block = SPIFLASH_CFG_PHYS_ERASE_SZ;
    cfg.log_block_size = SPIFLASH_CFG_LOG_BLOCK_SZ;
    cfg.log_page_size = SPIFLASH_CFG_LOG_PAGE_SZ;
    cfg.hal_read_f = (spiffs_read)hal_spi_flash_read;
    cfg.hal_write_f = (spiffs_write)hal_spi_flash_write;
    cfg.hal_erase_f = (spiffs_erase)hal_spi_flash_erase;

    sFLASH_Init();
    if( sFLASH_ReadID() != HAL_SPIFFS_CHIPID )
    {
        //set_errno( 100 ); 
        //shell_write_line( "CHIPID err" );
        return 0;
    }

    return (SPIFFS_OK == SPIFFS_mount( &_fs, &cfg, (u8_t*)_work_buf, (u8_t*)_fds, 
                        sizeof(_fds), (void*)_cache_buf, sizeof(_cache_buf), 0 ) ) \
            ? 1 : 0;
}


int hal_spiffs_umount( void )
{
    if( !SPIFFS_mounted(&_fs) )
        return 1;
    SPIFFS_unmount( &_fs );
    return SPIFFS_mounted(&_fs);
}


int hal_spiffs_check( void )
{
    return SPIFFS_check( &_fs );
}


int hal_spiffs_remove( const char *path )
{
    return SPIFFS_remove( &_fs, path ); 
}


int hal_spiffs_rename( const char *old, const char *newPath )
{
    return SPIFFS_rename( &_fs, old, newPath ); 
}


spiffs_flags parse_mode_flags( const char *mode )
{
    spiffs_flags flags = 0;
    uint8_t rd=0, wr=0, cr=0, a=0;
    while( mode && *mode )
    {
        if( *mode == 'r' )
            rd = 1;
        else if( *mode == 'w' )
            wr = 1;
        else if( *mode == '+' )
            cr = 1;
        else if( *mode == 'a' )
            a = 1;
        mode ++;
    }
    if( rd && wr )
        flags = SPIFFS_RDWR;
    else if( rd && !wr )
        flags = SPIFFS_RDONLY;
    else if( wr && !rd )
        flags = SPIFFS_WRONLY;
    if( a )
        flags |= SPIFFS_APPEND;
    if( cr )
        flags |= SPIFFS_CREAT;
    return flags;
}


int hal_spiffs_open( const char *path, const char *mode )
{
    return SPIFFS_open( &_fs, path, parse_mode_flags(mode), 0 );
}


int hal_spiffs_read( int fh, void *buf, int len )
{
    int ret = SPIFFS_read( &_fs, fh, buf, len );
    return ret < 0 ? 0 : ret;
}


int hal_spiffs_write( int fh, void *buf, int len )
{
    int ret = SPIFFS_write( &_fs, fh, buf, len );
    return ret < 0 ? 0 : ret;
}


int hal_spiffs_seek( int fh, int offs, int where )
{
    return SPIFFS_lseek( &_fs, fh, offs, where );
}


int hal_spiffs_flush( int fh )
{
    return SPIFFS_fflush( &_fs, fh );
}


int hal_spiffs_close( int fh )
{
    return SPIFFS_close( &_fs, fh );
}


int hal_spiffs_format( void )
{
    return SPIFFS_format( &_fs );
}


int hal_spiffs_size( const char *name, int *size )
{
    return 0;
}


int hal_spiffs_info( int *total, int *used )
{
    return SPIFFS_info( &_fs, (u32_t*)total, (u32_t*)used );
}


int hal_spiffs_list( const char *pathname, void (*cb)(const char *name, int size, int mode) )
{
    spiffs_DIR dir;
    struct spiffs_dirent dirent;

    if( ! SPIFFS_opendir( &_fs, "/", &dir ) )
        return 0;
    while( SPIFFS_readdir( &dir, &dirent ) )
    {
        (*cb)( dirent.name, dirent.size, 0 ); 
    } 
    SPIFFS_closedir( &dir );
    return 1;
}



static int hal_spiffs_driver_errno;
const mcush_vfs_driver_t hal_spiffs_driver = {
    &hal_spiffs_driver_errno,
    hal_spiffs_mount,
    hal_spiffs_umount,
    hal_spiffs_info,
    hal_spiffs_format,
    hal_spiffs_check,
    hal_spiffs_remove,
    hal_spiffs_rename,
    hal_spiffs_open,
    hal_spiffs_read,
    hal_spiffs_seek,
    hal_spiffs_write,
    hal_spiffs_flush,
    hal_spiffs_close,
    hal_spiffs_size,
    hal_spiffs_list,
};



