/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


#if USE_CMD_FATFS || USE_CMD_SPIFFS
int do_test_file( const char *mount_point )
{
    char fname[32];
    char buf[256];
    int fd;
    int i, j;
    
    strcpy( fname, mount_point );
    strcat( fname, "/test.dat" );
    fd = mcush_open( fname, "w+" );
    if( fd <= 0 )
        return 0;
    strcpy( buf, "abcdefghijklmnopqrstuvwxyz\n" );
    i = strlen(buf);
    j = mcush_write( fd, buf, i );
    if( i == j )
    {
        strcpy( buf, "0123456789\n" );
        i = strlen(buf);
        j = mcush_write( fd, buf, i );
    }
    mcush_close( fd );
    return (i == j) ? 1 : 0;
}
#endif


#if USE_CMD_CAT
#if USE_CMD_CAT_WRITE
    #define _SUPPORT_WRITE  1
#else
    #define _SUPPORT_WRITE  0
#endif
#if USE_CMD_CAT_B64
    #define _SUPPORT_B64  1
#else
    #define _SUPPORT_B64  0
#endif
#if USE_CMD_CAT_DELAY
    #define _SUPPORT_DELAY  1
#else
    #define _SUPPORT_DELAY  0
#endif
#define CAT_BUF_RAW  100
#define CAT_BUF_B64  180
#if USE_CMD_CAT_B64
#define CAT_BUF_LEN  (CAT_BUF_RAW+CAT_BUF_B64)
#else
#define CAT_BUF_LEN  (CAT_BUF_RAW)
#endif
int cmd_cat( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
#if _SUPPORT_B64
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'b', "b64", 0, "base 64 code" },
#endif
#if _SUPPORT_WRITE
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'w', shell_str_write, 0, "write mode" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'a', shell_str_append, 0, "append mode" },
#endif
#if _SUPPORT_DELAY
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'd', shell_str_delay, shell_str_delay, "output delay in ms" },
#endif
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, shell_str_file_name },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char fname[32];
    char buf[CAT_BUF_LEN];
    int fd;
    char c;
    int size, bytes;
    int i;
#if _SUPPORT_B64
    int j;
    uint8_t b64=0;
    base64_encodestate state_en;
#endif
#if _SUPPORT_WRITE
#if _SUPPORT_B64
    base64_decodestate state_de;
    char *p, *p2;
#endif
    uint8_t write=0, append=0;
    void *input=0;
#endif
#if _SUPPORT_DELAY
    uint32_t delay=0;
#endif

    fname[0] = 0;
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
#if _SUPPORT_WRITE
            if( STRCMP( opt.spec->name, shell_str_write ) == 0 )
                write = 1;
            else if( STRCMP( opt.spec->name, shell_str_append ) == 0 )
                append = 1;
            else 
#endif
#if _SUPPORT_B64
                 if( strcmp( opt.spec->name, "b64" ) == 0 )
            {
                b64 = 1;
                base64_init_encodestate( &state_en );
#if _SUPPORT_WRITE
                base64_init_decodestate( &state_de );
#endif
            }
            else 
#endif
                if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
                strcpy( fname, (char*)opt.value );
#if _SUPPORT_DELAY
            else if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
            {
                if( ! parse_int(opt.value, (int*)&delay) )
                {
                    shell_write_err( shell_str_parameter );
                    return -1;
                }
            }
#endif
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( ! fname[0] )
        return -1;
       
#if _SUPPORT_WRITE
    if( write || append )
    {
        input = shell_read_multi_lines(0);
        if( !input )
            return 1;
        i = strlen(input);
        if( !i )
        {
            os_free(input);
            return 1;
        }
        
        fd = mcush_open( fname, append ? "a+" : "w+" );
        if( fd == 0 )
        {
            os_free(input);
            return 1;
        }
#if _SUPPORT_B64
        if( b64 )
        {
            p2 = (char*)input;
            while( 1 ) 
            {
                p = p2;
                i = 0;
                while( *p2 && *p2 != '\n')
                {
                    p2++;
                    i++;
                }
                if( !*p2 && !i )
                    break;
                if( *p2 == '\n' )
                    *p2++ = 0;
                if( !i )
                    break;
                j = base64_decode_block( (const char*)p, i, (char*)&buf, &state_de );
                if( j )
                {
                    if( j != mcush_write( fd, buf, j ) )
                    {
                        mcush_close(fd);
                        os_free(input);
                        return 1;
                    } 
                }
            }
        }
        else
#endif
        {
            if( i != mcush_write( fd, input, i ) )
            {
                mcush_close(fd);
                os_free(input);
                return 1;
            } 
        }
        mcush_close(fd);
        os_free(input);
    }
    else
#endif
    { 
        if( ! mcush_size( fname, &size ) )
            return 1;
        fd = mcush_open( fname, "r" );
        if( fd == 0 )
            return 1;
        bytes = 0;
        while( 1 )
        {    
#if _SUPPORT_B64
            i = mcush_read( fd, buf, b64 ? CAT_BUF_RAW : CAT_BUF_LEN );
#else
            i = mcush_read( fd, buf, CAT_BUF_LEN );
#endif
            if( i < 0 )
            {
                mcush_close(fd);
                shell_write_char( '\n' );
                return 1;
            }

            bytes += i;
            if( i==0  )
            {
#if _SUPPORT_B64
                if( b64 )
                {
                    j = base64_encode_blockend( buf, &state_en );
                    shell_write( buf, j );
                }
#endif
                break;  // end
            }
            else
            {
#if _SUPPORT_B64
                if( b64 )
                {
                    j = base64_encode_block( buf, i, &buf[CAT_BUF_RAW], &state_en );
                    shell_write( buf + CAT_BUF_RAW, j );
                }
                else
#endif
                    shell_write( buf, i );
#if _SUPPORT_B64
                if( i < (b64 ? CAT_BUF_RAW : CAT_BUF_LEN) )
#else
                if( i < CAT_BUF_LEN )
#endif
                {
#if _SUPPORT_B64
                    if( b64 )
                    {
                        j = base64_encode_blockend( buf, &state_en );
                        shell_write( buf, j );
                    }
#endif
                    break;  // end
                }
            }
            if( bytes>=size )
            {
#if _SUPPORT_B64
                if( b64 )
                {
                    j = base64_encode_blockend( buf, &state_en );
                    shell_write( buf, j );
                }
#endif
                break;  // end
            }

#if _SUPPORT_DELAY
            while( shell_driver_read_char_blocked(&c, OS_TICKS_MS(delay)) != -1 )
#else
            while( shell_driver_read_char_blocked(&c, 0) != -1 )
#endif
            {
                if( c == 0x03 ) /* Ctrl-C for stop */
                {
                    mcush_close(fd);
                    shell_write_char( '\n' );
                    return 0;
                }
            }
        }
        mcush_close(fd);
        shell_write_str( "\n" );
    }
    return 0;
}
#endif


#if USE_CMD_RM
int cmd_rm( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, shell_str_file_name },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
                fname = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname )
        return -1;
        
    return mcush_remove( fname ) ? 0 : 1;
}
#endif


#if USE_CMD_RENAME
int cmd_rename( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, "old -> new" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0, *fname2=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
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
        return -1;

    return mcush_rename( fname, fname2 ) ? 0 : 1;
}
#endif


#if USE_CMD_CP
int cmd_copy( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, "src -> dst" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0, *fname2=0;
    char buf[256];
    int i, j;
    int fd, fd2;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
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
        return -1;
    //if( strcmp(fname, fname2) == 0 )
    //    return 0;
       
    fd = mcush_open( fname, "r" );
    if( fd == 0 )
        return 1;
    fd2 = mcush_open( fname2, "w+" );
    if( fd2 == 0 )
    {
        mcush_close( fd );
        return 1;
    }

    while( 1 )
    {    
        i = mcush_read( fd, buf, 256 );
        if( i==0 )
            break;
        j = mcush_write( fd2, buf, i );
        if( i != j )
            break;
        if( i<256 )
            break;
    }

    mcush_close( fd );
    mcush_close( fd2 );
    return 0;
}
#endif


#if USE_CMD_LS
static char *cmd_list_match_file_name;
void cb_print_file(const char *name, int size, int mode)
{
    (void)mode;
    if( cmd_list_match_file_name && strcmp(cmd_list_match_file_name, name) != 0 )
        return;
    shell_printf("%6d  %s\n", size, name );
}

extern mcush_vfs_volume_t vfs_vol_tab[MCUSH_VFS_VOLUME_NUM];

int cmd_list( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_path, 0, shell_str_path_name },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *path=0;
    char mount_point[8], file_name[32];
    int i;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_path ) == 0 )
                path = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    file_name[0] = 0;
    if( path && ! get_mount_point( path, mount_point ) )
        return 1;
    get_file_name( path, file_name );
     
    for( i=0; i< MCUSH_VFS_VOLUME_NUM; i++ )
    {
        if( !vfs_vol_tab[i].mount_point )
            continue;
        if( !path || strcmp(vfs_vol_tab[i].mount_point, mount_point)==0 )
        {
            cmd_list_match_file_name = file_name[0] ? file_name : 0;
            strcpy( mount_point, "/" );
            strcat( mount_point, vfs_vol_tab[i].mount_point );
            shell_printf("/%s:\n", vfs_vol_tab[i].mount_point );
            mcush_list( mount_point, cb_print_file );
        }
    }
    return 0;
}
#endif


#if USE_CMD_LOAD
int cmd_load( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, shell_str_file_name },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char fname[32];
    int size;
    int fd;
    void *buf=0;
    int i;
    const char *addr;

    fname[0] = 0;
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
                strcpy( fname, (char*)opt.value );
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( fname[0] != '/' )
        return -1;
    if( ! isalnum((int)fname[1]) )
        return -1;
    if( fname[2] != '/' )
        return -1;
    if( ! isalnum((int)fname[3]) )
        return -1;

#if MCUSH_ROMFS
    if( fname[1] == 'r' )
    {
        if( mcush_romfs_get_raw_address( (const char*)&fname[3], &addr, &size ) )
        {
            shell_set_script( addr, 0 );
            return 0; 
        }
        else
            return 1;
    }
#endif

#if MCUSH_FCFS
    if( fname[1] == 'c' )
    {
        if( mcush_fcfs_get_raw_address( (const char*)&fname[3], &addr, &size ) )
        {
            shell_set_script( addr, 0 );
            return 0; 
        }
        else
            return 1;
    }
#endif

    mcush_size( fname, &size );
    if( size == 0 )
        return 0;

    buf = os_malloc( size + 1 );
    if( !buf )
    {
        shell_write_err( shell_str_script );
        return 1;
    }

    fd = mcush_open( fname, "r" );
    if( fd == 0 )
    {
        os_free(buf);
        return 1;
    }
    
    i = mcush_read( fd, buf, size );   
    mcush_close(fd);

    if( i != size )
    {
        shell_write_err( shell_str_script );
        os_free(buf);
        return 1;
    }
  
    ((char*)buf)[i] = 0; 
    shell_set_script( buf, 1 );
    return 0;
}
#endif


#if USE_CMD_CRC
int cmd_crc( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_file, 0, shell_str_file_name },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0;
    int size;
    uint32_t crc;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
                fname = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !fname )
        return -1;
        
    size = mcush_file_exists( fname );
    if( size == 0 )
        return 1;
    if( mcush_file_crc32(fname, &crc) == 0 )
        return 1;     
    shell_printf("0x%08X\n", crc);
    return 0;
}
#endif


#if USE_CMD_FCFS
#include "mcush_vfs_fcfs.h"
int cmd_fcfs( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        /* TODO: offset option may be obselected in the future */
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'o', shell_str_offset, shell_str_offset, shell_str_offset },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', shell_str_command, "cmd_name", "info|erase|(p)rogram|offset" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_value, 0, shell_str_data },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *cmd=0;
    static uint32_t offset=0;
    int buf[32];
    int len;
    int i;
    fcfs_head_t *head=(fcfs_head_t*)FLASH_FCFS_ADDR_BASE;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_offset ) == 0 )
                parse_int(opt.value, (int*)&offset);
            else if( STRCMP( opt.spec->name, shell_str_command) == 0 )
                cmd = (char*)opt.value;
            else if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
            {
                parser.idx--;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd || strcmp( cmd, shell_str_info ) == 0 )
    {
        shell_printf( "%s: 0x%08X\n", shell_str_address, FLASH_FCFS_ADDR_BASE );
        shell_printf( "%s: %u\n", shell_str_size, FLASH_FCFS_SIZE );
        shell_printf( "uid: %s\n", hexlify((const char*)(head->uid), (char*)buf, sizeof(head->uid), 1) );
    }
    else if( (strcmp( cmd, shell_str_format ) == 0) ||
             (strcmp( cmd, shell_str_erase ) == 0) )
    {
        mcush_fcfs_umount();
        offset = 0;
        return hal_flash_erase((void*)FLASH_FCFS_ADDR_BASE, FLASH_FCFS_SIZE) ? 0 : 1;
    }
    else if( strcmp( cmd, "offset" ) == 0 )
    {
        if( ++parser.idx < argc )
        {
            if( ! parse_int(argv[parser.idx], (int*)&offset) )
            {
                shell_write_str( "data err: " );
                shell_write_line( argv[parser.idx] );
                return 1;
            }
        }
        else
            shell_printf( "0x%08X\n", offset );
    }
    else if( (strcmp( cmd, shell_str_program ) == 0) || 
             (strcmp( cmd, "p") == 0) )
    {
        parser.idx++;
        len = 0;
        while( parser.idx < argc )
        {
            if( ! parse_int(argv[parser.idx], (int*)&i) )
            {
                shell_write_str( "data err: " );
                shell_write_line( argv[parser.idx] );
                return 1;
            }
            buf[len] = i;
            parser.idx++;
            len += 1;
        }
        if( len == 0 )
            return -1;
        if( hal_flash_program( (void*)(FLASH_FCFS_ADDR_BASE+offset), buf, len*4 ) )
        {
            offset += len*4;
            return 0;
        }
        else
            return 1;
    } 
    else
    {
        shell_write_err( shell_str_command );
        return -1;
    }
    return 0;
}
#endif


#if USE_CMD_SPIFFS
#include "mcush_vfs_spiffs.h"
#include "spi_flash.h"
int cmd_spiffs( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', shell_str_command, "cmd_name", "info|id|status|erase|read|write|[u|re]mount|test|format|check" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'C', shell_str_ascii, 0, shell_str_ascii },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_compact, 0, "compact output" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *cmd=0;
    void *addr=(void*)-1;
    char buf[256];
    int i, j;
    int len;
    void *p;
    uint8_t compact_mode=0, ascii_mode=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_compact ) == 0 )
                compact_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_ascii ) == 0 )
                ascii_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_command) == 0 )
                cmd = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd || strcmp( cmd, shell_str_info ) == 0 )
    {
        if( ! mcush_spiffs_mounted() )
            goto not_mounted;
        mcush_spiffs_info( &i, &j );
        shell_printf( "%s: %d\n%s: %d\n", shell_str_total, i, shell_str_used, j );
        return 0;
    }
    if( strcmp( cmd, shell_str_id ) == 0 )
    {
        shell_printf( "%X\n", (unsigned int)hal_spiffs_flash_read_id() );
    }
    else if( strcmp( cmd, shell_str_status ) == 0 )
    {
        shell_printf( "%X\n", (unsigned int)hal_spiffs_flash_read_status() );
    }
    else if( strcmp( cmd, shell_str_erase ) == 0 )
    {
        /* umount and erase bulk/sector and remount */
        if( ! mcush_spiffs_umount() )
            return 1;
        if( addr == (void*)-1 )
            sFLASH_EraseBulk();
        else
            sFLASH_EraseSector( (uint32_t)addr );
        if( ! mcush_spiffs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_read ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        sFLASH_ReadBuffer( (void*)buf, (int)addr, 256 );
        if( compact_mode )
            ascii_mode = 0;
        for( i=0; i<16; i++ )
        {
            if( !compact_mode )
                shell_printf( "%08X: ", (unsigned int)((int)addr+i*16) );
            for( j=0; j<16; j++ )
                shell_printf( compact_mode ? "%02X" : "%02X ", *(unsigned char*)&buf[i*16+j] );
            if( ascii_mode )
            {
                shell_write_str( " |" );
                for( j=0; j<16; j++ )
                {
                    if( isprint((int)(*(unsigned char*)&buf[i*16+j])) )
                        shell_write_char(*(unsigned char*)&buf[i*16+j]);
                    else
                        shell_write_char('.');
                }
                shell_write_char( '|' );
            }
            shell_write_str( "\r\n" );
        }
    } 
    else if( strcmp( cmd, shell_str_write ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        if( shell_make_16bits_data_buffer( &p, &len ) ) 
        {
            len *= 2;
            sFLASH_WritePage( (uint8_t*)p, (int)addr, len > 256 ? 256 : (uint16_t)len );
            os_free( p );
        }
        else
            return 1;
    } 
    else if( strcmp( cmd, shell_str_mount ) == 0 )
    {
        if( ! mcush_spiffs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_umount ) == 0 )
    {
        if( ! mcush_spiffs_umount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_remount ) == 0 )
    {
        if( ! mcush_spiffs_umount() || ! mcush_spiffs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_test ) == 0 )
    {
        if( ! mcush_spiffs_mounted() )
            goto not_mounted;
        return do_test_file( "/s" ) ? 0 : 1; 
    }
    else if( strcmp( cmd, shell_str_format ) == 0 )
    {
        if( mcush_spiffs_mounted() )
            mcush_spiffs_umount();
        i = mcush_spiffs_format();
        if( i )
            return 1;
        else if( ! mcush_spiffs_mount() )
            return 1;
    }
    else if( strcmp( cmd, "check" ) == 0 )
    {
        if( ! mcush_spiffs_mounted() )
            goto not_mounted;
        i = mcush_spiffs_check();
        shell_printf( "%d\n", i );
        return 0;
    }
    else
    {
        shell_write_err( shell_str_command );
        return -1;
    }
    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}
#endif


#if USE_CMD_FATFS
#include "mcush_vfs_fatfs.h"
#include "diskio.h"
extern SD_HandleTypeDef hsd;

int cmd_fatfs( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', shell_str_command, "cmd_name", "id|erase|read|write|[u|re]mount|test|format|check|info" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'C', shell_str_ascii, 0, shell_str_ascii },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_compact, 0, "compact output" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *cmd=0;
    void *addr=(void*)-1;
    uint8_t compact_mode=0, ascii_mode=0;
    int i, j;
    unsigned char buf[512];
 
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_compact ) == 0 )
                compact_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_ascii ) == 0 )
                ascii_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_command) == 0 )
                cmd = (char*)opt.value;   
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd || strcmp( cmd, shell_str_info ) == 0 )
    {
        if( ! mcush_fatfs_mounted() )
            goto not_mounted;
        /* Card Identification Number */
        /*                  MID  OID  PNM      PRV  PSN      MDT  CRC    */  
        shell_printf( "CID: %02X-%04X-%02X%08X-%02X-%06X%02X-%04X-%02X\n", 
                        (hsd.CID[0]>>24)&0xFF, (hsd.CID[0]>>8)&0xFFFF, hsd.CID[0]&0xFF, hsd.CID[1],
                        (hsd.CID[2]>>24)&0xFF, hsd.CID[2]&0xFFFFFF, (hsd.CID[3]>>24)&0xFF,
                        (hsd.CID[3]>>8)&0xFFFF, (hsd.CID[3])&0xFF );
        /* Card Specific Data */
        shell_printf( "CSD: %08X%08X%08X%08X\n", hsd.CSD[0], hsd.CSD[1], hsd.CSD[2], hsd.CSD[3] );
        /* CardInfo */
        shell_printf( "Type: %d\n", hsd.SdCard.CardType );
        shell_printf( "Version: %d\n", hsd.SdCard.CardVersion );
        shell_printf( "Class: %d\n", hsd.SdCard.Class );
        shell_printf( "RelCardAdd: 0x%08X\n", hsd.SdCard.RelCardAdd );
        shell_printf( "BlockNbr: %d\n", hsd.SdCard.BlockNbr );
        shell_printf( "BlockSize: %d\n", hsd.SdCard.BlockSize );
        shell_printf( "LogBlockNbr: %d\n", hsd.SdCard.LogBlockNbr );
        shell_printf( "LogBlockSize: %d\n", hsd.SdCard.LogBlockSize );

        return 0;
    }
    if( strcmp( cmd, shell_str_read ) == 0 )
    {
        if( addr == (void*)-1 )
            addr = 0;
        if( disk_read( 0, buf, ((uint32_t)addr)/512, 1 ) != RES_OK )
            return 1;
        
        if( compact_mode )
            ascii_mode = 0;
        for( i=0; i<32; i++ )
        {
            if( !compact_mode )
                shell_printf( "%08X: ", (unsigned int)((int)addr+i*16) );
            for( j=0; j<16; j++ )
                shell_printf( compact_mode ? "%02X" : "%02X ", *(unsigned char*)&buf[i*16+j] );
            if( ascii_mode )
            {
                shell_write_str( " |" );
                for( j=0; j<16; j++ )
                {
                    if( isprint((int)(*(unsigned char*)&buf[i*16+j])) )
                        shell_write_char(*(unsigned char*)&buf[i*16+j]);
                    else
                        shell_write_char('.');
                }
                shell_write_char( '|' );
            }
            shell_write_str( "\r\n" );
        }
    }
    else if( strcmp( cmd, shell_str_write ) == 0 )
    {
 
    } 
    else if( strcmp( cmd, shell_str_erase ) == 0 )
    {
        //if( addr == (void*)-1 )
        //    sFLASH_EraseBulk();
        //else
        //    sFLASH_EraseSector( (uint32_t)addr );
    }
    else if( strcmp( cmd, shell_str_mount ) == 0 )
    {
        if( ! mcush_fatfs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_umount ) == 0 )
    {
        if( ! mcush_fatfs_umount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_remount ) == 0 )
    {
        if( ! mcush_fatfs_umount() || ! mcush_fatfs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_format ) == 0 )
    {
        if( mcush_fatfs_mounted() )
            mcush_fatfs_umount();
        i = mcush_fatfs_format();
        if( i )
            return 1;
        else if( ! mcush_fatfs_mount() )
            return 1;
    }
    else if( strcmp( cmd, shell_str_test ) == 0 )
    {
        return do_test_file( "/f" ) ? 0 : 1; 
    }

    return 0;

not_mounted:
    shell_write_line( "not mounted" );
    return 1;
}
#endif
