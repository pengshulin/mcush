/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"

#ifndef USE_CMD_HELP
    #define USE_CMD_HELP  1
#endif
#ifndef USE_CMD_SCPI_IDN
    #define USE_CMD_SCPI_IDN  1
#endif
#ifndef USE_CMD_SCPI_RST
    #define USE_CMD_SCPI_RST  1
#endif
#ifndef USE_CMD_RESET
    #define USE_CMD_RESET  1
#endif
#ifndef USE_CMD_GPIO
    #define USE_CMD_GPIO  1
#endif
#ifndef USE_CMD_LED
    #define USE_CMD_LED  1
#endif
#ifndef USE_CMD_DUMP
    #define USE_CMD_DUMP  1
#endif
#ifndef USE_CMD_WRITE
    #define USE_CMD_WRITE  1
#endif
#ifndef USE_CMD_MFILL
    #define USE_CMD_MFILL  1
#endif
#ifndef USE_CMD_WAIT
    #define USE_CMD_WAIT  1
#endif
#ifndef USE_CMD_ECHO
    #define USE_CMD_ECHO  1
#endif
#ifndef USE_CMD_MKBUF
    #define USE_CMD_MKBUF  1
#endif
#ifndef USE_CMD_WDG
    #define USE_CMD_WDG  1
#endif
#ifndef USE_CMD_UPTIME
    #define USE_CMD_UPTIME  1
#endif
#ifndef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  1
    #ifndef USE_CMD_SYSTEM_HEAP
        #define USE_CMD_SYSTEM_HEAP  0
    #endif
    #ifndef USE_CMD_SYSTEM_STACK
        #define USE_CMD_SYSTEM_STACK  1
    #endif
#endif
#ifndef USE_CMD_MAPI
    #define USE_CMD_MAPI  1
#endif
#ifndef USE_CMD_BEEP
    #define USE_CMD_BEEP  0
#endif
#ifndef USE_CMD_SGPIO
    #define USE_CMD_SGPIO  0
#endif



#if defined(MCUSH_NON_OS)
    #undef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  0
#endif


int cmd_help( int argc, char *argv[] );
int cmd_scpi_idn( int argc, char *argv[] );
int cmd_scpi_rst( int argc, char *argv[] );
int cmd_reset( int argc, char *argv[] );
int cmd_gpio( int argc, char *argv[] );
int cmd_led( int argc, char *argv[] );
int cmd_dump( int argc, char *argv[] );
int cmd_write( int argc, char *argv[] );
int cmd_mfill( int argc, char *argv[] );
int cmd_wait( int argc, char *argv[] );
int cmd_echo( int argc, char *argv[] );
int cmd_wdg( int argc, char *argv[] );
int cmd_uptime( int argc, char *argv[] );
int cmd_system( int argc, char *argv[] );
int cmd_mapi( int argc, char *argv[] );
int cmd_beep( int argc, char *argv[] );
int cmd_sgpio( int argc, char *argv[] );
int cmd_mkbuf( int argc, char *argv[] );





const shell_cmd_t CMD_TAB[] = {
#if USE_CMD_HELP
{   0,  '?',  "help",  cmd_help, 
    "print command list",
    "help [-a]"  },
#endif
#if USE_CMD_SCPI_IDN
{   0,  0,  "*idn?", cmd_scpi_idn,
    "print device info",
    "*idn?" },
#endif
#if USE_CMD_SCPI_RST
{   0,  0,  "*rst",  cmd_scpi_rst, 
    "reset device",
    "*rst" },
#endif
#if USE_CMD_RESET
{   CMD_HIDDEN, 'r',  "reset",  cmd_reset, 
    "reset CPU core",
    "reset" },
#endif
#if USE_CMD_DUMP
{   CMD_HIDDEN,  'x',  "dump",  cmd_dump, 
    "dump memory",
    "dump -b <address> [-l <length>] [-w 1|2|4]" },
#endif
#if USE_CMD_WRITE
{   CMD_HIDDEN,  'w',  "write",  cmd_write, 
    "write memory",
    "write -b <address> [-w 1|2|4] dat1 dat2 ..." },
#endif
#if USE_CMD_MFILL
{   CMD_HIDDEN,  0,  "mfill",  cmd_mfill, 
    "fill/test memory",
    "mfill -b <address> [-l <length>] [-w 1|2|4] [-p <pattern>] [-t]" },
#endif
#if USE_CMD_MAPI
{   CMD_HIDDEN,  0,  "mapi",  cmd_mapi,
    "memory api",
    "mapi -m|-r|-f [-b <address>] [-l <length>]" },
#endif
#if USE_CMD_WAIT
{   CMD_HIDDEN,  0,  "wait",  cmd_wait, 
    "sleep ms",
    "wait <ms>" },
#endif
#if USE_CMD_ECHO
{   CMD_HIDDEN,  0,  "echo",  cmd_echo, 
    "print args",
    "echo <args>" },
#endif
#if USE_CMD_MKBUF
{   CMD_HIDDEN,  0,  "mkbuf",  cmd_mkbuf, 
    "make data buffer",
    "mkbuf" },
#endif
#if USE_CMD_WDG
{   CMD_HIDDEN,  0,  "wdg",  cmd_wdg, 
    "watchdog",
    "wdg enable|disable|clear|reset" },
#endif
#if USE_CMD_UPTIME
{   CMD_HIDDEN,  0,  "uptime",  cmd_uptime, 
    "print run time",
    "uptime" },
#endif
#if USE_CMD_SYSTEM
{   CMD_HIDDEN,  0,  "sys",  cmd_system, 
    "print sys info",
    "sys t|q|k" },
#endif
#if USE_CMD_GPIO
{   0,  0,  "gpio",  cmd_gpio, 
    "control gpio",
    "gpio -p <port.bit> [-i <val>] [-o <val>] [-s <val>] [-c <val>] [-t <val>] [-l]" },
#endif
#if USE_CMD_SGPIO
{   0,  0,  "sgpio",  cmd_sgpio, 
    "control sgpio",
    "sgpio -p <port> -o <val> -i <val> -f <val> [-l] [--start|stop]" },
#endif
#if USE_CMD_LED
{   0,  0,  "led",  cmd_led, 
    "control led",
    "led [-s|c|t] -i <idx>" },
#endif
#if USE_CMD_BEEP
{   0,  'b',  "beep",  cmd_beep, 
    "beep control",
    "beep [-f <freq>] [<ms>]"  },
#endif
{   CMD_END  } };



#if USE_CMD_HELP
int cmd_help( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "all", 'a', 0, "show hidden commands", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int show_hidden=0;
 
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "all" ) == 0 )
                show_hidden = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    return shell_print_help(0, show_hidden);
}
#endif


#if USE_CMD_SCPI_IDN
int cmd_scpi_idn( int argc, char *argv[] )
{
    /* *idn? command ignore all arguments */
    char buf[64];

    shell_write_line( "mcush," MCUSH_VERSION_STRING );
    buf[0] = 0;
    if( hal_get_serial_number(buf) && strlen(buf) )
        shell_write_line( buf );

    return 0;
}
#endif


#if USE_CMD_SCPI_RST
int cmd_scpi_rst( int argc, char *argv[] )
{
    /* *rst command ignore all arguments */
    return 0;
}
#endif


#if USE_CMD_RESET
int cmd_reset( int argc, char *argv[] )
{
    /* reset command ignore all arguments */
    hal_reset();
    while( 1 );
    //return 0;
}
#endif


#if USE_CMD_GPIO
int cmd_gpio( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "loop", 'l', 0, "loop mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "loop_delay", 0, "loop delay value", "loop delay value in ms, default 1000", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "port", 'p', "port_bit_name", "port[.bit] name, eg 0[.0]", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "input", 'i', "input_mode", "set input mode mask", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "output", 'o', "output_mode", "set output mode mask", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "set", 's', "set_high_val", "set output high mask", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "clr", 'c', "set_low_val", "set output low mask", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "toggle", 't', "toggle_val", "toggle output mask", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "port_num", 'n', 0, "query port number", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "pullup", 'U', 0, "input with pullup resister", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "pulldown", 'D', 0, "input with pulldown resister", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int loop=0, loop_delay=-1;
    uint32_t tick;
    char c;
    int port=-1, bit=-1, bit_mode=-1, pull=-1;
    const char *pport=0, *pinput=0, *poutput=0, *pset=0, *pclr=0, *ptoggle=0;
    char *pbit;
    uint8_t input_set=0, output_set=0, set_set=0, clr_set=0, toggle_set=0, none_set=0;
    int input_val=0, output_val=0, set_val=0, clr_val=0, toggle_val=0;
    int port_num = hal_gpio_get_port_num();

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "port" ) == 0 )
                pport = opt.value;
            else if( strcmp( opt.spec->name, "input" ) == 0 )
            {
                input_set = 1;
                pinput = opt.value;
            }
            else if( strcmp( opt.spec->name, "output" ) == 0 )
            {
                output_set = 1;
                poutput = opt.value;
            }
            else if( strcmp( opt.spec->name, "set" ) == 0 )
            {
                set_set = 1;
                pset = opt.value;
            }
            else if( strcmp( opt.spec->name, "clr" ) == 0 )
            {
                clr_set = 1;
                pclr = opt.value;
            }
            else if( strcmp( opt.spec->name, "toggle" ) == 0 )
            {
                toggle_set = 1;
                ptoggle = opt.value;
            }
            else if( strcmp( opt.spec->name, "port_num" ) == 0 )
            {
                shell_printf( "%d\n", port_num );
                return 0;
            }
            else if( strcmp( opt.spec->name, "pullup" ) == 0 )
                pull = 1;
            else if( strcmp( opt.spec->name, "pulldown" ) == 0 )
                pull = 0;
            else if( strcmp( opt.spec->name, "loop" ) == 0 )
                loop=1;
            else if( strcmp( opt.spec->name, "loop_delay" ) == 0 )
                shell_eval_int(opt.value, (int*)&loop_delay);
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( !pport )
        goto port_error;
    else
    {
        port = strtol( pport, &pbit, 10 );
        bit_mode = 0;
        if( pbit )
        {
            if( *pbit == '.' )
            {
                pbit += 1;
                if( *pbit )
                {
                    bit = strtol( pbit, &pbit, 10 );
                    if( !pbit )
                        bit_mode = 1;
                    else if( !*pbit )
                        bit_mode = 1;
                    else
                        goto port_error;
                }
                else
                    goto port_error;
            }
            else if( *pbit )
                goto port_error;
        } 
        if( (port < 0) || (port > port_num-1) )
            goto port_error;
        if( bit_mode && ((bit < 0) || (bit > 31)) )
            goto port_error;
    }

    if( !bit_mode )
    {
        if( input_set && !shell_eval_int(pinput, (int*)&input_val) )
            goto parm_error;
        if( output_set && !shell_eval_int(poutput, (int*)&output_val) )
            goto parm_error;
        if( set_set && !shell_eval_int(pset, (int*)&set_val) )
            goto parm_error;
        if( clr_set && !shell_eval_int(pclr, (int*)&clr_val) )
            goto parm_error;
        if( toggle_set && !shell_eval_int(ptoggle, (int*)&toggle_val) )
            goto parm_error;
    }

    if( !(input_set || output_set || set_set || clr_set || toggle_set) )
        none_set = 1;

    if( loop_delay <= 0 )
        loop_delay = 1000;  /* default: 1 sec */
loop_start:

    if( bit_mode )
    {
        if( input_set )
        {
            if( pull == -1 )
                hal_gpio_set_input( port, 1<<bit );
            else
                hal_gpio_set_input_pull( port, 1<<bit, pull );
        }
        if( output_set )
            hal_gpio_set_output( port, 1<<bit );
        if( set_set )
            hal_gpio_set( port, 1<<bit );
        if( clr_set )
            hal_gpio_clr( port, 1<<bit );
        if( toggle_set )
            hal_gpio_toggle( port, 1<<bit );
        if( none_set )
            shell_write_str( hal_gpio_get( port, 1<<bit ) ? "1\n" : "0\n" );
    }
    else
    {
        if( input_set )
        {
            if( pull == -1 )
                hal_gpio_set_input( port, pinput ? input_val : 0xFFFFFFFF );
            else
                hal_gpio_set_input_pull( port, pinput ? input_val : 0xFFFFFFFF, pull );
        }
        if( output_set )
            hal_gpio_set_output( port, poutput ? output_val : 0xFFFFFFFF );
        if( set_set )
            hal_gpio_set( port, pset ? set_val : 0xFFFFFFFF );
        if( clr_set )
            hal_gpio_clr( port, pclr ? clr_val : 0xFFFFFFFF );
        if( toggle_set )
            hal_gpio_toggle( port, ptoggle ? toggle_val : 0xFFFFFFFF );
        if( none_set )
        {
            shell_write_hex( hal_gpio_get( port, 0xFFFFFFFF ) );
            shell_write_str( "\n" );
        }
    }
 
    if( loop )
    {
#if defined(MCUSH_NON_OS)
        tick = get_sys_tick_count();
        while( get_sys_tick_count() < tick + loop_delay*configTICK_RATE_HZ/1000 )
#else
        tick = xTaskGetTickCount();
        while( xTaskGetTickCount() < tick + loop_delay*configTICK_RATE_HZ/1000 )
#endif
        {
            if( shell_driver_read_char_blocked(&c, 10*configTICK_RATE_HZ/1000) != -1 )
                if( c == 0x03 ) /* Ctrl-C for stop */
                    return 0;
        }
        goto loop_start;
    }
 
    return 0;
port_error:
    shell_write_line("port/bit error");
    return 1;
parm_error:
    shell_write_line("parm error");
    return 1;
}
#endif


#if USE_CMD_LED
int cmd_led( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "set", 's', 0, "set led", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "toggle", 't', 0, "toggle led", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "clr", 'c', 0, "clear led", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "index", 'i', "led_index", "select led id", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_SWITCH, "led_num", 'n', 0, "query led number", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "test", 'T', 0, "test mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int led_num = hal_led_get_num();
    int cmd=-1, index=-1, test_mode=-1;
    char c, test_on;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "clr" ) == 0 )
                cmd = 0;
            else if( strcmp( opt.spec->name, "set" ) == 0 )
                cmd = 1;
            else if( strcmp( opt.spec->name, "toggle" ) == 0 )
                cmd = 2;
            else if( strcmp( opt.spec->name, "index" ) == 0 )
                shell_eval_int(opt.value, (int*)&index);
            else if( strcmp( opt.spec->name, "led_num" ) == 0 )
            {
                shell_printf( "%d\n", led_num );
                return 0;
            }
            else if( strcmp( opt.spec->name, "test" ) == 0 )
                test_mode = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( test_mode == 1 )
    {
        for( index=0; index<led_num; index++ )
            hal_led_set( index );
        test_on = 0;
        while( 1 )
        {
            if( (shell_driver_read_char_blocked(&c, 250*configTICK_RATE_HZ/1000) != -1 ) \
                && (c == 0x03) )  /* Ctrl-C for quit */
                break;
            for( index=0; index<led_num; index++ )
                if( test_on )
                    hal_led_set( index );
                else
                    hal_led_clr( index );
            test_on ^= 1;
        }
        for( index=0; index<led_num; index++ )
            hal_led_clr( index );
        return 0;
    }

    if( index == -1 )
    {
        shell_write_line( "index not assigned" );
        return -1;
    }
    else if( (index < 0) || (index > (led_num-1)) )
    {
        shell_write_str( "index: " );
        if( led_num > 1 )
            shell_printf( "0~%d\r\n", led_num-1 );
        else
            shell_write_line( "0" );
        return -1;
    }
 
    switch( cmd )
    {
    case 0: hal_led_clr( index ); break;
    case 1: hal_led_set( index ); break;
    case 2: hal_led_toggle( index ); break;
    default: shell_write_line( hal_led_get(index) ? "1" : "0" ); break;
    }
    return 0;
}
#endif


#if USE_CMD_DUMP
int cmd_dump( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length (default 16)", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "width", "bus width 1(default)|2|4", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_SWITCH, "ascii", 'C', 0, "ascii output (width=1)", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "compact", 'c', 0, "compact output", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1, *addr2;
    int length=16;
    int width=1;
    int count=0, count2;
    int i;
    char c;
    uint8_t ascii_mode=0;
    uint8_t compact_mode=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "length" ) == 0 )
                shell_eval_int(opt.value, (int*)&length);
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, "ascii" ) == 0 )
                ascii_mode = 1;
            else if( strcmp( opt.spec->name, "compact" ) == 0 )
                compact_mode = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_line( "address not assigned" );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_line( "width should be 1,2,4" );
        return -1;
    }

    if( (width != 1) || compact_mode )
        ascii_mode = 0;
    while( count < length )
    {
        if( !compact_mode )
            shell_printf( "%08X: ", (unsigned int)addr );
        switch(width)
        {
        case 1:
            if( ascii_mode )
            {
                count2 = count;
                addr2 = addr;
            }
            for( i=0; i<16; i++ )
            {
                shell_printf( compact_mode ? "%02X" : "%02X ", *(unsigned char*)addr );
                addr = (void*)(((int)addr) + 1 );
                count += 1;
                if( count >= length )
                {
                    if( ascii_mode )
                    {
                        for( i++; i<16; i++ )
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
            break;
        case 2:
            for( i=0; i<8; i++ )
            {
                shell_printf( compact_mode ? "%04X" : "%04X ", *(unsigned short*)addr );
                addr = (void*)(((int)addr) + 2 );
                count += 2;
                if( count >= length )
                    break;
            }
            break;
        case 4:
            for( i=0; i<4; i++ )
            {
                shell_printf( compact_mode ? "%08X" : "%08X ", *(unsigned int*)addr );
                addr = (void*)(((int)addr) + 4 );
                count += 4;
                if( count >= length )
                    break;
            }
            break;
        }
        shell_write_str( "\r\n" );
        /* check if Ctrl-C is pressed */
        if( shell_driver_read_char_blocked(&c, 0) != -1 )
            if( c == 0x03 ) /* Ctrl-C for stop */
                return 0;
    }
    return 0;
}
#endif


#if USE_CMD_WRITE
int cmd_write( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "bus width", "bus width 1(default)|2|4", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_ARG, "data", 0, 0, "data to be written", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1;
    int width=1;
    int dat;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, "data" ) == 0 )
                break;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_line( "address not assigned" );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_line( "width should be 1,2,4" );
        return -1;
    }

    while( parser.idx < argc )
    {
        if( ! shell_eval_int(argv[parser.idx], &dat) )
        {
            shell_write_str( "data error: " );
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
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "bus width", "bus width 1(default)|2|4", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "pattern", 'p', "pattern", "data to be written", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_SWITCH, "test", 't', 0, "test mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1;
    int length=-1;
    int pattern=-1;
    int width=1;
    int count=0;
    int test_mode=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "pattern" ) == 0 )
                shell_eval_int(opt.value, (int*)&pattern);
            else if( strcmp( opt.spec->name, "length" ) == 0 )
                shell_eval_int(opt.value, (int*)&length);
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, "test" ) == 0 )
                test_mode = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( addr == (void*)-1 )
    {
        shell_write_line( "address not assigned" );
        return -1;
    }

    if( !( (width==1) || (width==2) || (width==4) ) )
    {
        shell_write_line( "width should be 1,2,4" );
        return -1;
    }

    if( pattern == -1 )
    {
        shell_write_line( "pattern not set" );
        return -1;
    }

    if( length == -1 )
    {
        shell_write_line( "length not set" );
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


#if USE_CMD_WAIT
int cmd_wait( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "time", 0, 0, "wait time in ms, default 1000", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int ms=1000;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "time" ) == 0 )
                shell_eval_int(opt.value, (int*)&ms);
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

#if defined(MCUSH_NON_OS)
    hal_delay_ms( ms );
#else 
    vTaskDelay( ms * configTICK_RATE_HZ / 1000  );
#endif
    return 0;
}
#endif


#if USE_CMD_ECHO
int cmd_echo( int argc, char *argv[] )
{
    int i;

    for( i=1; i<argc; i++ )
    {
        shell_write_str( argv[i] );
        if( (i+1) < argc )
            shell_write_str( " " );
        else
            shell_write_str( "\n" );
    }
    return 0;
}
#endif


#if USE_CMD_WDG
int cmd_wdg( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "cmd", 0, 0, "enable|disable|clear|reset", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    const char *cmd=0;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "cmd" ) == 0 )
                cmd = opt.value;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd )
    {
        shell_write_line( hal_wdg_is_enable() ? "enable" : "disable" );
    }
    else if( strcmp( cmd, "enable" ) == 0 )
    {
        hal_wdg_enable();
    }
    else if( strcmp( cmd, "disable" ) == 0 )
    {
        hal_wdg_disable();
    }
    else if( strcmp( cmd, "clear" ) == 0 )
    {
        hal_wdg_clear();
    }
    else if( strcmp( cmd, "reset" ) == 0 )
    {
#if !defined( MCUSH_NON_OS )
        portENTER_CRITICAL();
#endif
        while(1);
    }
    else
        return -1;
    
    return 0;
}
#endif


#if USE_CMD_UPTIME
char *get_uptime_str(char *buf, int ms)
{
#if defined(MCUSH_NON_OS)
    unsigned int t = get_sys_tick_count();
#else
    unsigned int t = xTaskGetTickCount();
#endif
    unsigned int s = t / configTICK_RATE_HZ;
    if( ms )
    {
        t = t - s * configTICK_RATE_HZ;
        t = t * 1000 / configTICK_RATE_HZ; 
        sprintf(buf, "%u:%02u:%02u.%03u", s/3600, (s/60)%60, s%60, t);
    }
    else
        sprintf(buf, "%u:%02u:%02u", s/3600, (s/60)%60, s%60);
    return buf;
}


int cmd_uptime( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_NONE } };
    char buf[16];

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( !opt.spec )
            STOP_AT_INVALID_ARGUMENT 
    }

    shell_write_line( get_uptime_str(buf, 1) );
    return 0;
}
#endif


#if USE_CMD_SYSTEM
int cmd_system( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "type", 0, 0, "(t)ask|(q)ueue|(k)ern|heap|stack|trace", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    QueueHandle_t xQueue;
    mcush_queue_info_t qinfo;
    mcush_task_info_t tinfo;
    mcush_kern_info_t kinfo;
    const char *name=0;
    char buf[256];
    char *p;
    int i;
    const char *type=0;
    
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "type" ) == 0 )
                type = opt.value;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !type )
    {
        STOP_AT_INVALID_ARGUMENT 
    }
    else if( (strcmp( type, "t" ) == 0) || (strcmp( type, "task" ) == 0) )
    {
        for( i=0; i<MCUSH_TASK_REGISTRY_SIZE; i++ )
        {
            if( mcushGetTaskInfo( i, &tinfo ) )
            {
                 shell_printf( "%8s 0x%08X %d 0x%08X 0x%08X (free %d)\n",
                        tinfo.pcTaskName, (uint32_t)tinfo.pTaskHandle, tinfo.uxPriority,
                        tinfo.pxStack, tinfo.pxTopOfStack, tinfo.uxFreeStack );
            }
        } 
    }
    else if( (strcmp( type, "q" ) == 0 ) || (strcmp( type, "queue" ) == 0 ) )
    {
        for( i=0; i<configQUEUE_REGISTRY_SIZE; i++ )
        {
            if( mcushGetQueueRegistered( i, &xQueue, &name ) )
            {
                if( ! mcushGetQueueInfo( xQueue, &qinfo ) )
                    continue; 
                shell_printf( "%8s 0x%08X  %4d %4d %4d  0x%08X - 0x%08X (0x%08X)\n", name, (int)xQueue, 
                        qinfo.uxLength, qinfo.uxItemSize, qinfo.uxMessagesWaiting, 
                        (int)qinfo.pcHead, (int)qinfo.pcTail, ((int)qinfo.pcTail-(int)qinfo.pcHead) );
            }
        }
    }
    else if( (strcmp( type, "k" ) == 0 ) || (strcmp( type, "kern" ) == 0 ) )
    {
        mcushGetKernInfo(&kinfo);
        shell_printf( "CurrentNumberOfTasks: %d\n", kinfo.uxCurrentNumberOfTasks );
        shell_printf( "TopReadyPriority:     %d\n", kinfo.uxTopReadyPriority );
        shell_printf( "PendedTicks:          %d\n", kinfo.uxPendedTicks );
        shell_printf( "NumOfOverflows:       %d\n", kinfo.uxNumOfOverflows );
        shell_printf( "CurrentTCB:           0x%08X %s\n", (uint32_t)kinfo.pxCurrentTCB, mcushGetTaskNameFromTCB(kinfo.pxCurrentTCB) );
        for( i=0; i<configMAX_PRIORITIES; i++ )
            shell_printf( "ReadyTaskLists[%d]:    0x%08X %s\n", i, (uint32_t)kinfo.pxReadyTaskLists[i], mcushGetTaskNamesFromTaskList(kinfo.pxReadyTaskLists[i], buf) );
        shell_printf( "DelayedTaskList1:     0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList1, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList1, buf) );
        shell_printf( "DelayedTaskList2:     0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList2, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList2, buf) );
        shell_printf( "DelayedTaskList:      0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList, buf) );
        shell_printf( "OverflowDelayedTList: 0x%08X %s\n", (uint32_t)kinfo.pxOverflowDelayedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxOverflowDelayedTaskList, buf) );
        shell_printf( "PendingReadyList:     0x%08X %s\n", (uint32_t)kinfo.pxPendingReadyList, mcushGetTaskNamesFromTaskList(kinfo.pxPendingReadyList, buf) );
#if ( INCLUDE_vTaskSuspend == 1 )
        shell_printf( "SuspendedTaskList:    0x%08X %s\n", (uint32_t)kinfo.pxSuspendedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxSuspendedTaskList, buf) );
#endif
    }
#if USE_CMD_SYSTEM_HEAP
    else if( strcmp( info, "heap" ) == 0 )
    {
        extern char *heap_end, _sheap, _eheap;
        shell_printf( "S:0x%08X\nC:0x%08X\nE:0x%08X\n", &_sheap, heap_end, &_eheap ); 
    }
#endif
#if USE_CMD_SYSTEM_STACK
    else if( strcmp( type, "stack" ) == 0 )
    {
        extern char _sstack, _estack;
        for( p=&_sstack, i=0; p<&_estack && (*p == 0xA5); p++, i++ );
        shell_printf( "S:0x%08X\nE:0x%08X\nfree:%d\n", &_sstack, &_estack, i );
    }
#endif
#if configUSE_TRACE_FACILITY && configUSE_STATS_FORMATTING_FUNCTIONS
    else if( strcmp( type, "trace" ) == 0 )
    {
        vTaskList( buf );
        shell_write_str( buf ); 
    }
#endif
    else
        STOP_AT_INVALID_ARGUMENT 
    return 0;
}
#endif


#if USE_CMD_MAPI
#include <malloc.h>
int cmd_mapi( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "test", 't', 0, "test heap memory", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "fill", 0, 0, "fill heap memory", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "info", 'i', 0, "print mallinfo", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "malloc", 'm', 0, "allocate new memory", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "realloc", 'r', 0, "re-allocate memory", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "free", 'f', 0, "free memory", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1;
    int length=-1;
    uint8_t malloc_set=0, realloc_set=0, free_set=0, test_mode=0, info_set=0, fill_mode=0;
    void *mlist=0;
    int *p;
    int i,j,k;
    struct mallinfo info;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "length" ) == 0 )
                shell_eval_int(opt.value, (int*)&length);
            else if( strcmp( opt.spec->name, "malloc" ) == 0 )
                malloc_set = 1;
            else if( strcmp( opt.spec->name, "realloc" ) == 0 )
                realloc_set = 1;
            else if( strcmp( opt.spec->name, "free" ) == 0 )
                free_set = 1;
            else if( strcmp( opt.spec->name, "test" ) == 0 )
                test_mode = 1;
            else if( strcmp( opt.spec->name, "fill" ) == 0 )
                fill_mode = 1;
            else if( strcmp( opt.spec->name, "info" ) == 0 )
                info_set = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( info_set )
        goto print_mallinfo;

#define MAPI_MLIST_LEN  64
#define MAPI_TEST_MALLOC_SIZE  32768
    if( test_mode || fill_mode )
    {
        mlist = malloc( 4*MAPI_MLIST_LEN );
        if( ! mlist )
            return 1;
        shell_printf( "[L] 0x%08X %d\n", (unsigned int)mlist, 4*MAPI_MLIST_LEN );
        memset( mlist, 0, 4*MAPI_MLIST_LEN );
        p = (int*)mlist;
        //j = MAPI_TEST_MALLOC_SIZE;
        j = mallinfo().arena / 2;
        k = 4*MAPI_MLIST_LEN;
        i = 0;
        while( i < MAPI_MLIST_LEN )
        {
            *p = (int)malloc( j );
            if( *p )
            {
                k += j;
                shell_printf( "[%d] 0x%08X %d\n", i+1, (unsigned int)*p, j );
                p++;
                i++;
            }
            else
            {
                j /= 2;
                if( !j )
                    break;
            }
        } 
        shell_printf( "Total: %d\n", k );
        p = (int*)mlist;
        if( !fill_mode )
        {
            for( i=0; i<MAPI_MLIST_LEN; i++, p++ )
            {
                if( *p )
                    free((void*)*p);
            }
            free( mlist );
        }
        return 0;
    }

    if( malloc_set || realloc_set )
    {
        if( free_set )
            goto usage_error;
        if( (length==-1) || (length==0) )
        {
            shell_write_line( "length error" );
            return -1;
        }
        if( realloc_set )
        {
            if( !addr || ((int)addr == -1) ) 
            {
                shell_write_line( "addr error" );
                return -1;
            }
            addr = realloc( addr, length );
        }
        else
        {
            addr = malloc( length );
        }
        shell_printf( "0x%08X\n", (unsigned int)addr );
    }
    else if( free_set )
    {
        if( (int)addr == -1 )
        {
            shell_write_line( "addr not set" );
            return -1;
        }
        free( addr );
    }
    else
    {
print_mallinfo:
        info = mallinfo();
        shell_printf( "arena:    %d\n", info.arena );    /* total space allocated from system */
        shell_printf( "ordblks:  %d\n", info.ordblks );  /* number of non-inuse chunks */
        shell_printf( "uordblks: %d\n", info.uordblks ); /* total allocated space */
        shell_printf( "fordblks: %d\n", info.fordblks ); /* total non-inuse space */
        shell_printf( "keepcost: %d\n", info.keepcost ); /* top-most, releasable (via malloc_trim) space */
    }

    return 0;
usage_error:
    mcush_opt_usage_print( argv[0], opt_spec );
    return -1;
}
#endif


#if USE_CMD_BEEP
int cmd_beep( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "freq", 'f', "frequency", "range 20~10000 (default 4000)", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_ARG, "ms", 0, 0, "duration (default 50ms)", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int freq=-1, ms=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "freq" ) == 0 )
                shell_eval_int(opt.value, (int*)&freq);
            else if( strcmp( opt.spec->name, "ms" ) == 0 )
                shell_eval_int(opt.value, (int*)&ms);
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( freq == -1 )
        freq = 4000;
    if( ms == -1 )
        ms = 50;

    if( !( (freq >= 20) && (freq <= 10000) ) )
        STOP_AT_INVALID_ARGUMENT  
    if( !( (ms > 0) && (ms <= 10000) ) )
        STOP_AT_INVALID_ARGUMENT  
   
    hal_beep_on( freq );
    vTaskDelay(ms*configTICK_RATE_HZ/1000);
    hal_beep_off();
    
    return 0;
}
#endif


#if USE_CMD_MKBUF
int cmd_mkbuf( int argc, char *argv[] )
{
    void *buf;
    int buf_len;
    //mcush_opt_parser parser;
    //mcush_opt opt;
    //const mcush_opt_spec opt_spec[] = {
    //    { MCUSH_OPT_NONE } };

    //mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    //while( mcush_opt_parser_next( &opt, &parser ) )
    //{
    //    if( opt.spec )
    //    {
    //        if( strcmp( opt.spec->name, "port" ) == 0 )
    //            shell_eval_int(opt.value, (int*)&port);
    //        else if( strcmp( opt.spec->name, "freq" ) == 0 )
    //        {
    //            if( shell_eval_float(opt.value, (float*)&freq_val) )
    //                freq = 1;
    //        }
    //    }
    //    else
    //         STOP_AT_INVALID_ARGUMENT  
    //}

    if( !shell_make_16bits_data_buffer( &buf, &buf_len ) )
        return 1;
                
    shell_printf( "0x%08X  %d\n", buf, buf_len );
    return 0;
}
#endif


#if USE_CMD_SGPIO
int cmd_sgpio( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "port", 'p', "port_index", "index from 0", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "output", 'o', "output_mode", "set output mode mask", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "input", 'i', "input_mode", "set input mode mask", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "input_len", 0, "input_buf_len", "length of input buffer", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "freq", 'f', "frequency", "1~1000000", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_SWITCH, "loop", 'l', 0, "loop mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "start", 'r', 0, "run", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "stop", 's', 0, "stop", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "info", 0, 0, "print info", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int port=-1, loop=0, output=-1, input=-1, freq=-1, buf_len=0, input_len=0;
    float freq_val=SGPIO_FREQ_DEF;
    char start=0, stop=0, info=0;
    void *buf_in=0, *buf_out=0;
    sgpio_cfg_t *cfg;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "port" ) == 0 )
                shell_eval_int(opt.value, (int*)&port);
            else if( strcmp( opt.spec->name, "freq" ) == 0 )
            {
                if( shell_eval_float(opt.value, (float*)&freq_val) )
                    freq = 1;
            }
            else if( strcmp( opt.spec->name, "loop" ) == 0 )
                loop = 1;
            else if( strcmp( opt.spec->name, "output" ) == 0 )
                shell_eval_int(opt.value, (int*)&output);
            else if( strcmp( opt.spec->name, "input" ) == 0 )
                shell_eval_int(opt.value, (int*)&input);
            else if( strcmp( opt.spec->name, "start" ) == 0 )
                start = 1;
            else if( strcmp( opt.spec->name, "stop" ) == 0 )
                stop = 1;
            else if( strcmp( opt.spec->name, "info" ) == 0 )
                info = 1;
            else if( strcmp( opt.spec->name, "input_len" ) == 0 )
                shell_eval_int(opt.value, (int*)&input_len);
 
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( start && stop )
        STOP_AT_INVALID_ARGUMENT

    if( info )
    {
        cfg = hal_sgpio_info();
        if( cfg->inited )
        {
            shell_printf( "%s  port:%d  out:0x%04x  in:0x%04x  loop:%c  freq:%.1f\n", 
                cfg->run ? "RUN" : "STOP",
                cfg->port, cfg->output_mode, cfg->input_mode, cfg->loop_mode?'1':'0', cfg->freq );
            shell_printf( "buf_out:0x%08x  buf_in:0x%08X  len:%d\n", 
                cfg->buf_out, cfg->buf_in, cfg->buf_len );
        } 
        else
            shell_write_line( "not initialized" );
        return 0;
    }

    if( start )
        return hal_sgpio_start() ? 0 : 1;
    else if( stop )
    {
        hal_sgpio_stop();
        return 0;
    }

    if( port == -1 )
    {
        shell_write_line( "port not set" );
        return -1;
    }
   
    if( output == -1 )
        output = 0;
    if( input == -1 )
        input = 0;
    
    if( !output && !input )
    {
        shell_write_line( "out/in not set" );
        return -1;
    }

    if( !( (freq_val >= SGPIO_FREQ_MIN) && (freq_val <= SGPIO_FREQ_MAX) ) )
    {
        shell_write_line( "freq range err" );
        return -1;
    }
   
    if( output )
    { 
        if( !shell_make_16bits_data_buffer( &buf_out, &buf_len ) )
            return 1;
    }
    
    if( input )
    {
        if( !output )
        {
            if( input_len )
                buf_len = input_len;
            else
            {
                shell_write_line( "input buf len err" );
                return -1;
            }
        }
        buf_in = malloc( buf_len * 2 );
    }

    return hal_sgpio_setup( loop, port, output, input, buf_out, buf_in, buf_len, freq_val ) ? 0 : 1;
}
#endif


