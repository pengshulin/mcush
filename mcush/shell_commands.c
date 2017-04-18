/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

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
#ifndef USE_CMD_WDG
    #define USE_CMD_WDG  1
#endif
#ifndef USE_CMD_UPTIME
    #define USE_CMD_UPTIME  1
#endif
#ifndef USE_CMD_SYSTEM
    #define USE_CMD_SYSTEM  1
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
int cmd_wdg( int argc, char *argv[] );
int cmd_uptime( int argc, char *argv[] );
int cmd_system( int argc, char *argv[] );





shell_cmd_t CMD_TAB[] = {
#if USE_CMD_HELP
{   "?", "help",  cmd_help, 
    "display command list",
    "help"  },
#endif
#if USE_CMD_SCPI_IDN
{   "",  "*idn?", cmd_scpi_idn,
    "show device info",
    "*idn?" },
#endif
#if USE_CMD_SCPI_RST
{   "",  "*rst",  cmd_scpi_rst, 
    "reset device",
    "*rst" },
#endif
#if USE_CMD_RESET
{   "r",  "reset",  cmd_reset, 
    "reset CPU core",
    "reset" },
#endif
#if USE_CMD_GPIO
{   "p",  "gpio",  cmd_gpio, 
    "control gpio",
    "gpio -p <port.bit> -i <val> -o <val> -s <val> -c <val> -t <val> -l" },
#endif
#if USE_CMD_LED
{   "",  "led",  cmd_led, 
    "control led",
    "led -s|c|t -i <idx>" },
#endif
#if USE_CMD_DUMP
{   "x",  "dump",  cmd_dump, 
    "dump memory",
    "dump -a <address> [-l <length>] [-w 1|2|4]" },
#endif
#if USE_CMD_WRITE
{   "w",  "write",  cmd_write, 
    "write memory",
    "write -a <address> [-w 1|2|4] dat1 dat2 ..." },
#endif
#if USE_CMD_MFILL
{   0,  "mfill",  cmd_mfill, 
    "fill memory",
    "mfill -b <address> [-l <length>] [-w 1|2|4] [-p <pattern>]" },
#endif
#if USE_CMD_WAIT
{   0,  "wait",  cmd_wait, 
    "wait in milliseconds",
    "wait <ms>" },
#endif
#if USE_CMD_WDG
{   0,  "wdg",  cmd_wdg, 
    "watchdog",
    "wdg enable|disable|clear|reset" },
#endif
#if USE_CMD_UPTIME
{   0,  "uptime",  cmd_uptime, 
    "show how long mcu has been running",
    "uptime" },
#endif
#if USE_CMD_SYSTEM
{   "sys",  "system",  cmd_system, 
    "show system info",
    "sys" },
#endif
{   0,  0,  0,  0  } };



#if USE_CMD_HELP
int cmd_help( int argc, char *argv[] )
{
    if( argc > 1 )
    {
        shell_write_line("no arguments needed");
        return -1;
    }

    return shell_print_help();
}
#endif


#if USE_CMD_SCPI_IDN
int cmd_scpi_idn( int argc, char *argv[] )
{
    shell_write_str( "mcush" );
    shell_write_str( "," );
    shell_write_str( MCUSH_VERSION_STRING );
    shell_write_str( "\r\n" );

    return 0;
}
#endif


#if USE_CMD_SCPI_RST
int cmd_scpi_rst( int argc, char *argv[] )
{
    return 0;
}
#endif


#if USE_CMD_RESET
int cmd_reset( int argc, char *argv[] )
{
    portENTER_CRITICAL();
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
        { MCUSH_OPT_VALUE, "loop_delay", 0, "loop delay value", "value in ms", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "port", 'p', "port_bit_name", "select port/bit", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "input", 'i', "input_mode", "set inpupt mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "output", 'o', "output_mode", "set output mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "set", 's', "set_high_val", "set output high", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "clr", 'c', "set_low_val", "set output low", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "toggle", 't', "toggle_val", "toggle output", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "port_num", 0, 0, "query port number", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "pullup", 0, 0, "input with pullup resister", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "pulldown", 0, 0, "input with pulldown resister", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int loop=0, loop_delay=-1;
    TickType_t tick;
    char c;
    int port=-1, bit=-1, bit_mode=-1, pull=-1;
    const char *pport=0, *pinput=0, *poutput=0, *pset=0, *pclr=0, *ptoggle=0;
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
            {
                if( opt.value )
                    loop_delay=atol(opt.value);
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( !pport )
        goto port_error;
    else
    {
        switch( sscanf( pport, "%d.%d", &port, &bit ) )
        {
        case 1: bit = -1;  bit_mode = 0;  break;
        case 2: bit_mode = 1;  break;
        //default:  goto port_error;  break;
        } 
        if( (port < 0) || (port > port_num-1) )
            goto port_error;
        if( bit_mode && ((bit < 0) || (bit > 31)) )
            goto port_error;
    }

    if( input_set && pinput && (1 != sscanf(pinput, "%i", &input_val)) )
        goto parm_error;
    if( output_set && poutput && (1 != sscanf(poutput, "%i", &output_val)) )
        goto parm_error;
    if( set_set && pset && (1 != sscanf(pset, "%i", &set_val)) )
        goto parm_error;
    if( clr_set && pclr && (1 != sscanf(pclr, "%i", &clr_val)) )
        goto parm_error;
    if( toggle_set && ptoggle && (1 != sscanf(ptoggle, "%i", &toggle_val)) )
        goto parm_error;

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
        tick = xTaskGetTickCount();
        while( xTaskGetTickCount() < tick + loop_delay*configTICK_RATE_HZ/1000 )
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
        { MCUSH_OPT_VALUE, "index", 'i', "led_index", "select led id", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int cmd=-1, index=-1;
    int led_num = hal_led_get_num();

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
                index = atoi(opt.value);
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( index == -1 )
    {
        shell_write_line( "index not assigned" );
        return -1;
    }
    else if( (index < 0) || (index > (led_num-1)) )
    {
        shell_printf( "index should between 0~%d\r\n", led_num-1 );
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
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "width", "bus width 1|2|4, 1 for default", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, "ascii", 'C', 0, "ascii output", MCUSH_OPT_USAGE_REQUIRED },
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
            {
                if( 1 != sscanf(opt.value, "%x", (unsigned int*)&addr) )
                    addr = (void*)-1;
            }
            else if( strcmp( opt.spec->name, "length" ) == 0 )
            {
                if( 1 != sscanf(opt.value, "%i", (int*)&length) )
                    length = 16;
            }
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                width = atoi(opt.value);
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
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "bus width", "bus width 1|2|4, 1 for default", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_LITERAL }, 
        { MCUSH_OPT_ARG, "data", 'd', "data", "data to be written", MCUSH_OPT_USAGE_REQUIRED },
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
            {
                if( 1 != sscanf(opt.value, "%i", (int*)&addr) )
                    addr = (void*)-1; 
            }
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                width = atoi(opt.value);
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
        if( 1 != sscanf(argv[parser.idx], "%i", &dat) )
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
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "bus width", "bus width 1|2|4, 1 for default", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "pattern", 'p', "pattern", "data to be written", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1;
    int length=-1;
    int pattern=-1;
    int width=1;
    int count=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "addr" ) == 0 )
            {
                if( 1 != sscanf(opt.value, "%i", (int*)&addr) )
                    addr = (void*)-1; 
            }
            else if( strcmp( opt.spec->name, "pattern" ) == 0 )
            {
                if( 1 != sscanf(opt.value, "%i", (int*)&pattern) )
                    pattern = -1; 
            }
            else if( strcmp( opt.spec->name, "length" ) == 0 )
            {
                if( 1 != sscanf(opt.value, "%i", (int*)&length) )
                    length = -1; 
            }
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                width = atoi(opt.value);
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
            *(char*)addr = (char)pattern;
            addr = (void*)(((int)addr) + 1 );
            break;
        case 2:
            *(short*)addr = (short)pattern;
            addr = (void*)(((int)addr) + 2 );
            break;
        case 4:
            *(int*)addr = (int)pattern;
            addr = (void*)(((int)addr) + 4 );
            break;
        }
        count += width;
    }
    return 0;
}
#endif


#if USE_CMD_WAIT
int cmd_wait( int argc, char *argv[] )
{
    int ms=-1;

    if( argc == 1 )
        ms = 1000;
    else if( argc == 2 )
    {
        ms = atoi(argv[1]);
        if( ms == 0 )
            ms = 1000;
    }
    else
        return -1; 
        
    vTaskDelay( ms * configTICK_RATE_HZ / 1000  );
    return 0;
}
#endif


#if USE_CMD_WDG
int cmd_wdg( int argc, char *argv[] )
{
    if( argc == 1 )
    {
        shell_write_line( hal_wdg_is_enable() ? "enable" : "disable" );
        return 0;
    }
    else if( argc > 2 )
        return -1;

    if( strcmp( argv[1], "enable" ) == 0 )
    {
        hal_wdg_enable();
    }
    else if( strcmp( argv[1], "disable" ) == 0 )
    {
        hal_wdg_disable();
    }
    else if( strcmp( argv[1], "clear" ) == 0 )
    {
        hal_wdg_clear();
    }
    else if( strcmp( argv[1], "reset" ) == 0 )
    {
        portENTER_CRITICAL();
        while(1);
    }
    else
        return -1;
    
    return 0;
}
#endif


char *get_uptime_str(char *buf, int ms)
{
    unsigned int t = xTaskGetTickCount();
    unsigned int s = t / configTICK_RATE_HZ;
    if( ms )
    {
        t = t - s * configTICK_RATE_HZ;
        t = t * 1000 / configTICK_RATE_HZ; 
        sprintf(buf, "%d:%02d:%02d.%03d", s/3600, (s/60)%60, s%60, t);
    }
    else
        sprintf(buf, "%d:%02d:%02d", s/3600, (s/60)%60, s%60);
    return buf;
}


#if USE_CMD_UPTIME
int cmd_uptime( int argc, char *argv[] )
{
    char buf[16];
    shell_write_line( get_uptime_str(buf, 1) );
    return 0;
}
#endif


#if USE_CMD_SYSTEM
int cmd_system( int argc, char *argv[] )
{
    QueueHandle_t xQueue;
    mcush_queue_info_t qinfo;
    mcush_task_info_t tinfo;
    mcush_kern_info_t kinfo;
    const char *name=0, *filter_name=0;
    int i;
    char buf[32];

    if( argc < 2 )
        goto usage_error;

    if( (strcmp( argv[1], "t" ) == 0) || (strcmp( argv[1], "task" ) == 0) )
    {
        if( argc > 2 )
            filter_name = argv[2];
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
    else if( (strcmp( argv[1], "q" ) == 0 ) || (strcmp( argv[1], "queue" ) == 0 ) )
    {
        if( argc > 2 )
            filter_name = argv[2];
        for( i=0; i<configQUEUE_REGISTRY_SIZE; i++ )
        {
            if( mcushGetQueueRegistered( i, &xQueue, &name ) )
            {
                if( filter_name && strcmp(filter_name, name) )
                    continue; 
                if( ! mcushGetQueueInfo( xQueue, &qinfo ) )
                    continue; 
                shell_printf( "%8s 0x%08X  %4d %4d %4d  0x%08X - 0x%08X (0x%08X)\n", name, (int)xQueue, 
                        qinfo.uxLength, qinfo.uxItemSize, qinfo.uxMessagesWaiting, 
                        (int)qinfo.pcHead, (int)qinfo.pcTail, ((int)qinfo.pcTail-(int)qinfo.pcHead) );
            }
        }
    }
    else if( (strcmp( argv[1], "k" ) == 0 ) || (strcmp( argv[1], "kern" ) == 0 ) )
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
    else
        goto usage_error;
    return 0;

usage_error:
    shell_write_line( "system t|task <task_name>" );
    shell_write_line( "system q|queue <queue_name>" );
    shell_write_line( "system k|kern" );
    return -1;
}
#endif



