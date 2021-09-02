/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


#if USE_CMD_MAPI
#if SUPPORT_NEWLIB
#include <malloc.h>
#endif
int cmd_mapi( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', shell_str_test, 0, "test heap memory" },
#if USE_CMD_MAPI_INFO
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', shell_str_info, 0, "print mallinfo" },
#endif
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'm', shell_str_malloc, 0, "allocate new memory" },
#if USE_CMD_MAPI_REALLOC
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', shell_str_realloc, 0, "re-allocate memory" },
#endif
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', shell_str_free, 0, "free memory" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'v', shell_str_verbose, 0, shell_str_verbose },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "memory length" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *addr=(void*)-1;
    int length=-1;
    uint8_t malloc_set=0, free_set=0, test_mode=0, verbose_mode=0;
#if USE_CMD_MAPI_INFO
    uint8_t info_set=0;
#endif
#if USE_CMD_MAPI_REALLOC
    uint8_t realloc_set=0;
#endif
    int i,j,k;
#if USE_CMD_MAPI_INFO
    struct mallinfo info;
#endif
#define MAPI_MLIST_LEN  64
    void *mlist[MAPI_MLIST_LEN];
   
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_length ) == 0 )
                parse_int(opt.value, (int*)&length);
            else if( STRCMP( opt.spec->name, shell_str_malloc ) == 0 )
                malloc_set = 1;
#if USE_CMD_MAPI_REALLOC
            else if( STRCMP( opt.spec->name, shell_str_realloc ) == 0 )
                realloc_set = 1;
#endif
            else if( STRCMP( opt.spec->name, shell_str_free ) == 0 )
                free_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_test ) == 0 )
                test_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_verbose ) == 0 )
                verbose_mode = 1;
#if USE_CMD_MAPI_INFO
            else if( STRCMP( opt.spec->name, shell_str_info ) == 0 )
                info_set = 1;
#endif
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

#if USE_CMD_MAPI_INFO
    if( info_set )
        goto print_mallinfo;
#endif

    if( test_mode )
    {
        for( i=0; i<MAPI_MLIST_LEN; i++ )
            mlist[i] = 0;
        j = MAPI_TEST_MALLOC_SIZE;
        //j = mallinfo().arena / 2;
        k = 0;
        i = 0;
        while( i < MAPI_MLIST_LEN )
        {
            mlist[i] = os_malloc( j );
            if( mlist[i] )
            {
                k += j;
                if( verbose_mode )
                    shell_printf( "[%d] %p %d\n", i+1, (unsigned int)mlist[i], j );
                i++;
            }
            else
            {
                j /= 2;
                if( !j )
                    break;
            }
        } 
        shell_printf( "%s: %d\n", shell_str_total, k );
        for( i=0; i<MAPI_MLIST_LEN; i++ )
        {
            if( mlist[i] )
                os_free((void*)mlist[i]);
        }
        return 0;
    }

#if USE_CMD_MAPI_REALLOC
    if( malloc_set || realloc_set )
#else
    if( malloc_set )
#endif
    {
        if( free_set )
            goto usage_error;
        if( (length==-1) || (length==0) )
        {
            shell_write_err( shell_str_length );
            return -1;
        }
#if USE_CMD_MAPI_REALLOC
        if( realloc_set )
        {
            if( !addr || ((int)addr == -1) ) 
            {
                shell_write_err( shell_str_address );
                return -1;
            }
            addr = pvPortRealloc( addr, length );
        }
        else
#endif
        {
            addr = os_malloc( length );
        }
        shell_printf( "%p\n", (unsigned int)addr );
    }
    else if( free_set )
    {
        if( (int)addr == -1 )
        {
            shell_write_err( shell_str_address );
            return -1;
        }
        os_free( addr );
    }
#if USE_CMD_MAPI_INFO
    else
    {
print_mallinfo:
        info = mallinfo();
        shell_printf( "arena:    %d\n", info.arena );    /* total space allocated from system */
        shell_printf( "ordblks:  %d\n", info.ordblks );  /* number of free chunks */
        shell_printf( "uordblks: %d\n", info.uordblks ); /* total allocated space */
        shell_printf( "fordblks: %d\n", info.fordblks ); /* total free space */
        shell_printf( "keepcost: %d\n", info.keepcost ); /* top-most, releasable space */
    }
#endif

    return 0;
usage_error:
    mcush_opt_usage_print( argv[0], opt_spec );
    return -1;
}
#endif


#if USE_CMD_MKBUF
int cmd_mkbuf( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', shell_str_float, 0, "float mode" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *buf;
    int buf_len;
    uint8_t float_mode=0;

    mcush_opt_parser_init( &parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_float ) == 0 )
                float_mode = 1;
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( float_mode )
    {
        if( !shell_make_float_data_buffer( &buf, &buf_len ) )
            return 1;
    }
    else
    {
        if( !shell_make_16bits_data_buffer( &buf, &buf_len ) )
            return 1;
    }
                
    shell_printf( "%s: %p\n%s: %d\n", shell_str_address, buf, shell_str_length, buf_len );
    return 0;
}
#endif


#if USE_CMD_DUMP
int cmd_dump( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "default 16" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, shell_str_width, "1(default)|2|4" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', shell_str_compact, 0, "compact output" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', shell_str_float, 0, "float output (width=4)" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'C', shell_str_ascii, 0, "ascii output (width=1)" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', shell_str_int, 0, "signed integer output" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', shell_str_uint, 0, "unsigned integer output" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *addr=(void*)-1, *addr2;
    int length=16;
    int width=1;
    int count=0, count2;
    int i;
    char c;
    uint8_t compact_mode=0, ascii_mode=0, integer_mode=0, float_mode=0, need_fill_line=0, unsigned_mode=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_length ) == 0 )
            {
                if( parse_int(opt.value, (int*)&length) == 0 )
                    return -1;
            }
            else if( STRCMP( opt.spec->name, shell_str_width ) == 0 )
            {
                if( parse_int(opt.value, (int*)&width) == 0 )
                    return -1;
            }
            else if( STRCMP( opt.spec->name, shell_str_compact ) == 0 )
                compact_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_ascii ) == 0 )
                ascii_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_int ) == 0 )
            {
                integer_mode = 1;
                unsigned_mode = 0;
            }
            else if( STRCMP( opt.spec->name, shell_str_uint ) == 0 )
            {
                integer_mode = 1;
                unsigned_mode = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_float ) == 0 )
                float_mode = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_err( shell_str_address );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_err( shell_str_width );
        return -1;
    }

    if( compact_mode )
    {
        ascii_mode = 0;
        float_mode = 0;
        integer_mode = 0;
        need_fill_line = 0;
    }
    else
    {
        if( ascii_mode && (width != 1) )
            ascii_mode = 0;
        if( float_mode && (width != 4) )
            float_mode = 0;
        if( ascii_mode || float_mode || integer_mode )
            need_fill_line = 1;
    }
    while( count < length )
    {
        if( !compact_mode )
            shell_printf( "%08X: ", (unsigned int)addr );
        count2 = count;
        addr2 = addr;
        if( width == 1 )
        {
            for( i=0; i<16; i++ )
            {
                shell_printf( compact_mode ? "%02X" : "%02X ", *(unsigned char*)addr );
                addr = (void*)(((int)addr) + 1 );
                count += 1;
                if( count >= length )
                {
                    if( need_fill_line )
                    {
                        while( ++i<16 )
                            shell_write_str( "   " ); 
                    }
                    break;
                }
            }
            if( ascii_mode )
            {
                shell_write_str( " |" );
                for( i=0; i<16; i++ )
                {
                    if( isprint((int)(*(char*)addr2)) )
                        shell_write_char(*(char*)addr2);
                    else
                        shell_write_char('.');
                    addr2 = (void*)(((int)addr2) + 1 );
                    count2 += 1;
                    if( count2 >= length )
                        break;
                }
                shell_write_char( '|' );
            }
            else if( integer_mode )
            {
                shell_write_str( " |" );
                for( i=0; i<16; i++ )
                {
                    if( unsigned_mode )
                        shell_printf("%u", *(uint8_t*)addr2);
                    else
                        shell_printf("%d", *(int8_t*)addr2);
                    addr2 = (void*)(((int)addr2) + 1 );
                    count2 += 1;
                    if( count2 >= length )
                        break;
                    if( i < 15 )
                        shell_write_char(' ');
                }
                shell_write_char( '|' );
            }
        }
        else if( width == 2 )
        {
            for( i=0; i<8; i++ )
            {
                shell_printf( compact_mode ? "%04X" : "%04X ", *(unsigned short*)addr );
                addr = (void*)(((int)addr) + 2 );
                count += 2;
                if( count >= length )
                {
                    if( need_fill_line )
                    {
                        while( ++i<8 )
                            shell_write_str( "     " ); 
                    }
                    break;
                }
            }
            if( integer_mode )
            {
                shell_write_str( " |" );
                for( i=0; i<8; i++ )
                {
                    if( unsigned_mode )
                        shell_printf("%u", *(uint16_t*)addr2);
                    else
                        shell_printf("%d", *(int16_t*)addr2);
                    addr2 = (void*)(((int)addr2) + 2 );
                    count2 += 2;
                    if( count2 >= length )
                        break;
                    if( i < 7 )
                        shell_write_char(' ');
                }
                shell_write_char( '|' );
            }
        }
        else if( width == 4 )
        {
            for( i=0; i<4; i++ )
            {
                shell_printf( compact_mode ? "%08X" : "%08X ", *(unsigned int*)addr );
                addr = (void*)(((int)addr) + 4 );
                count += 4;
                if( count >= length )
                {
                    if( need_fill_line )
                    {
                        while( ++i<4 )
                            shell_write_str( "         " ); 
                    }
                    break;
                }
            }
            if( float_mode || integer_mode )
            {
                shell_write_str( " |" );
                for( i=0; i<4; i++ )
                {
                    if( float_mode )
                        shell_printf("%e", *(float*)addr2);
                    else if( unsigned_mode )
                        shell_printf("%u", *(uint32_t*)addr2);
                    else
                        shell_printf("%d", *(int32_t*)addr2);
                    addr2 = (void*)(((int)addr2) + 4 );
                    count2 += 4;
                    if( count2 >= length )
                        break;
                    if( i < 3 )
                        shell_write_char(' ');
                }
                shell_write_char( '|' );
            }
        }
        shell_write_str( "\r\n" );
        /* check if Ctrl-C is pressed */
        while( shell_driver_read_char_blocked(&c, 0) != -1 )
        {
            if( c == 0x03 ) /* Ctrl-C for stop */
                return 0;
        }
    }
    return 0;
}
#endif


#if USE_CMD_WRITE
int cmd_write( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, "bus width", "1(default)|2|4" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_data, 0, "data to be written" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *addr=(void*)-1;
    int width=1;
    int dat;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_width ) == 0 )
                parse_int(opt.value, (int*)&width);
            else if( STRCMP( opt.spec->name, shell_str_data ) == 0 )
            {
                parser.idx--;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_err( shell_str_address );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_err( shell_str_width );
        return -1;
    }
    parser.idx++;
    while( parser.idx < argc )
    {
        if( ! parse_int(argv[parser.idx], &dat) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            return 1;
        }
        switch(width)
        {
        case 1:
            *(char*)addr = (char)dat;
            addr = (void*)(((int)addr) + 1 );
            break;
        case 2:
            *(short*)addr = (short)dat;
            addr = (void*)(((int)addr) + 2 );
            break;
        case 4:
            *(int*)addr = (int)dat;
            addr = (void*)(((int)addr) + 4 );
            break;
        } 
        parser.idx++;
    }
    return 0;
}
#endif


#if USE_CMD_MFILL
int cmd_mfill( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, shell_str_address, shell_str_base_address },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "memory length" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, "bus width", "1(default)|2|4" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'p', shell_str_pattern, shell_str_pattern, "data to be written" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', shell_str_test, 0, shell_str_test_mode },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *addr=(void*)-1;
    int length=-1;
    int pattern=-1;
    int width=1;
    int count=0;
    int test_mode=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
                parse_int(opt.value, (int*)&addr);
            else if( STRCMP( opt.spec->name, shell_str_pattern ) == 0 )
                parse_int(opt.value, (int*)&pattern);
            else if( STRCMP( opt.spec->name, shell_str_length ) == 0 )
                parse_int(opt.value, (int*)&length);
            else if( STRCMP( opt.spec->name, shell_str_width ) == 0 )
                parse_int(opt.value, (int*)&width);
            else if( STRCMP( opt.spec->name, shell_str_test ) == 0 )
                test_mode = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_err( shell_str_address );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_err( shell_str_width );
        return -1;
    }

    if( pattern == -1 )
    {
        shell_write_err( shell_str_pattern );
        return -1;
    }

    if( length == -1 )
    {
        shell_write_err( shell_str_length );
        return -1;
    }

    while( count < length )
    {
        switch(width)
        {
        case 1:
            if( test_mode )
            {
                if( *(char*)addr != (char)pattern )
                    goto test_failed;
            }
            else
                *(char*)addr = (char)pattern;
            addr = (void*)(((int)addr) + 1 );
            break;
        case 2:
            if( test_mode )
            {
                if( *(short*)addr != (short)pattern )
                    goto test_failed;
            }
            else 
                *(short*)addr = (short)pattern;
            addr = (void*)(((int)addr) + 2 );
            break;
        case 4:
            if( test_mode )
            {
                if( *(int*)addr != (int)pattern )
                    goto test_failed;
            }
            else
                *(int*)addr = (int)pattern;
            addr = (void*)(((int)addr) + 4 );
            break;
        }
        count += width;
    }
    return 0;
test_failed:
    return 1;
}
#endif



