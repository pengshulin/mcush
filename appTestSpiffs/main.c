#include "mcush.h"
#include "task_blink.h"
#include "spi_flash.h"
#include "spiffs.h"

#define SPIFLASH_CFG_PHYS_SZ        (1024*1024*8)
#define SPIFLASH_CFG_PHYS_ERASE_SZ  (65536)
#define SPIFLASH_CFG_PHYS_ADDR      (0)
#define SPIFLASH_CFG_LOG_BLOCK_SZ   (65536)
#define SPIFLASH_CFG_LOG_PAGE_SZ    (256)

spiffs _fs;
char _work_buf[2*SPIFLASH_CFG_LOG_PAGE_SZ];
char _fds[1024];
char _cache_buf[4096];


s32_t *SPI_FLASH_Read(u32_t addr, u32_t size, u8_t *dst)
{
    sFLASH_ReadBuffer(dst, addr, size);
    return SPIFFS_OK;
}


s32_t *SPI_FLASH_Write(u32_t addr, u32_t size, u8_t *src)
{
    sFLASH_WriteBuffer(src, addr, size);
    return SPIFFS_OK;
}


s32_t *SPI_FLASH_Erase(u32_t addr, u32_t size)
{
    sFLASH_EraseSector(addr);
    return SPIFFS_OK;
}


int cmd_spiffs( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "command", 'c', 0, "erase|read|write|mount|test|format|check|info", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *cmd=0;
    void *addr=(void*)-1;
    char buf[256];
    int i, j;
    int len;
    void *p;
    spiffs_config cfg;
    spiffs_file fd;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "command" ) == 0 )
                cmd = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd )
        return 1;

    if( strcmp( cmd, "erase" ) == 0 )
    {
        sFLASH_EraseBulk();
    }
    else if( strcmp( cmd, "read" ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        sFLASH_ReadBuffer( (void*)buf, (int)addr, 256 );
        for( i=0; i<16; i++ )
        {
            shell_printf( "%08X: ", (unsigned int)((int)addr+i*16) );
            for( j=0; j<16; j++ )
                shell_printf( "%02X ", *(unsigned char*)&buf[i*16+j] );
            shell_write_str( " |" );
            for( j=0; j<16; j++ )
            {
                if( isprint((int)(*(unsigned char*)&buf[i*16+j])) )
                    shell_write_char(*(unsigned char*)&buf[i*16+j]);
                else
                    shell_write_char('.');
            }
            shell_write_char( '|' );
            shell_write_str( "\r\n" );
        }
    } 
    else if( strcmp( cmd, "write" ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        if( shell_make_16bits_data_buffer( &p, &len ) ) 
        {
            len *= 2;
            sFLASH_WritePage( (uint8_t*)p, (int)addr, len > 256 ? 256 : len );
            free( p );
        }
        else
            return 1;
    } 
    else if( strcmp( cmd, "mount" ) == 0 )
    {
        if( SPIFFS_mounted(&_fs) )
            return 0;
        cfg.phys_size = SPIFLASH_CFG_PHYS_SZ;
        cfg.phys_addr = SPIFLASH_CFG_PHYS_ADDR;
        cfg.phys_erase_block = SPIFLASH_CFG_PHYS_ERASE_SZ;
        cfg.log_block_size = SPIFLASH_CFG_LOG_BLOCK_SZ;
        cfg.log_page_size = SPIFLASH_CFG_LOG_PAGE_SZ;
        cfg.hal_read_f = (spiffs_read)SPI_FLASH_Read;
        cfg.hal_write_f = (spiffs_write)SPI_FLASH_Write;
        cfg.hal_erase_f = (spiffs_erase)SPI_FLASH_Erase;

        sFLASH_Init();
        if( sFLASH_ReadID() != sFLASH_W25P64_ID )
        {
            set_errno( 100 ); 
            shell_write_line( "CHIPID err" );
            return 1;
        }

        i = SPIFFS_mount( &_fs, &cfg, (u8_t*)_work_buf, (u8_t*)_fds, 
                          sizeof(_fds), (void*)_cache_buf, sizeof(_cache_buf), 0 );

        if( i != SPIFFS_OK )
            return 1;
    }
    else if( strcmp( cmd, "test" ) == 0 )
    {
        if( ! SPIFFS_mounted(&_fs) )
            goto not_mounted;
        fd = SPIFFS_open( &_fs, "test.dat", SPIFFS_O_CREAT | SPIFFS_APPEND | SPIFFS_RDWR, 0 );
        if( fd < 0 )
            return 1;
        strcpy( buf, "abcdefghijklmnopqrstuvwxyz\n" );
        SPIFFS_write( &_fs, fd, buf, strlen(buf) );
        strcpy( buf, "01234567890\n" );
        SPIFFS_write( &_fs, fd, buf, strlen(buf) );
        SPIFFS_close( &_fs, fd );
    }
    else if( strcmp( cmd, "format" ) == 0 )
    {
        if( ! SPIFFS_mounted(&_fs) )
            goto not_mounted;
        i = SPIFFS_format( &_fs );
        return i ? 0 : 1;
    }
    else if( strcmp( cmd, "check" ) == 0 )
    {
        if( ! SPIFFS_mounted(&_fs) )
            goto not_mounted;
        i = SPIFFS_check( &_fs );
        shell_printf( "%d\n", i );
        return 0;
    }
    else if( strcmp( cmd, "info" ) == 0 )
    {
        if( ! SPIFFS_mounted(&_fs) )
            goto not_mounted;
        SPIFFS_info( &_fs, (u32_t*)&i, (u32_t*)&j );
        shell_printf( "total: %d  used: %d\n", i, j );
        return 0;
    }



    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}


int cmd_cat( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "file", 0, 0, "file name", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *fname=0;
    char buf[256];
    int i;
    spiffs_file fd;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
                fname = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname )
        return 1;
        
    if( ! SPIFFS_mounted(&_fs) )
        goto not_mounted;

    fd = SPIFFS_open( &_fs, fname, SPIFFS_O_RDONLY, 0 );
    if( fd < 0 )
        return 1;

    while( 1 )
    {    
        i = SPIFFS_read( &_fs, fd, buf, 256 );
        if( i==0 )
            break;
        shell_write( buf, i );
        if( i<256 )
        {
            shell_write_str( "\n" );
            break;
        }
    }
    SPIFFS_close( &_fs, fd );
    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}


int cmd_rm( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "file", 0, 0, "file name", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *fname=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
                fname = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname )
        return 1;
        
    if( ! SPIFFS_mounted(&_fs) )
        goto not_mounted;

    return SPIFFS_remove( &_fs, fname ) < 0 ? 1 : 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}


int cmd_rename( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "file", 0, 0, "file name", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *fname=0, *fname2=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
            {
                fname = (char*)opt.value;
                if( parser.idx + 1 < argc )
                    fname2 = argv[parser.idx+1];
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname || !fname2 )
        return 1;
    if( strcmp(fname, fname2) == 0 )
        return 0;
       
    if( ! SPIFFS_mounted(&_fs) )
        goto not_mounted;

    return SPIFFS_rename( &_fs, fname, fname2 ) < 0 ? 1 : 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}


int cmd_copy( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "file", 0, 0, "file name", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *fname=0, *fname2=0;
    char buf[256];
    int i;
    spiffs_file fd, fd2;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
            {
                fname = (char*)opt.value;
                if( parser.idx + 1 < argc )
                    fname2 = argv[parser.idx+1];
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname || !fname2 )
        return 1;
    if( strcmp(fname, fname2) == 0 )
        return 0;
       
    if( ! SPIFFS_mounted(&_fs) )
        goto not_mounted;
    
    fd = SPIFFS_open( &_fs, fname, SPIFFS_O_RDONLY, 0 );
    if( fd < 0 )
        return 1;
    fd2 = SPIFFS_open( &_fs, fname2, SPIFFS_O_WRONLY | SPIFFS_O_CREAT, 0 );
    if( fd2 < 0 )
        return 1;

    while( 1 )
    {    
        i = SPIFFS_read( &_fs, fd, buf, 256 );
        if( i==0 )
            break;
        SPIFFS_write( &_fs, fd2, buf, i );
        if( i<256 )
            break;
    }

    SPIFFS_close( &_fs, fd );
    SPIFFS_close( &_fs, fd2 );
    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}


int cmd_list( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        //{ MCUSH_OPT_ARG, "file", 0, 0, "file name", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    //char *fname=0;
    spiffs_DIR dir;
    struct spiffs_dirent dirent;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            //if( strcmp( opt.spec->name, "file" ) == 0 )
            //    fname = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    //if( !fname )
    //    return 1;
        
    if( ! SPIFFS_mounted(&_fs) )
        goto not_mounted;

    if( ! SPIFFS_opendir( &_fs, "/", &dir ) )
        return 1;
    while( SPIFFS_readdir( &dir, &dirent ) )
    {
        shell_printf("%5d  %s\n", dirent.size, dirent.name );
    } 
    SPIFFS_closedir( &dir );
    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}



const shell_cmd_t cmd_tab[] = {
{   0, 's', "spiffs",  cmd_spiffs, 
    "spiffs test",
    "spiffs"  },
{   0, 0, "cat",  cmd_cat, 
    "print file contents",
    "cat <fname>"  },
{   0, 0, "rm",  cmd_rm, 
    "remove file",
    "rm <fname>"  },
{   0, 0, "rename",  cmd_rename, 
    "rename file",
    "rename <old> <new>"  },
{   0, 0, "cp",  cmd_copy, 
    "copy file",
    "cp <src> <dst>"  },
{   0, 'l', "ls",  cmd_list, 
    "list files",
    "ls"  },
{   CMD_END  } };


int main(void)
{
    mcush_init();
    task_blink_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

