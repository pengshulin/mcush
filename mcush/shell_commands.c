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
#ifndef USE_CMD_REBOOT
    #define USE_CMD_REBOOT  1
#endif
#ifndef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
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
        #define USE_CMD_SYSTEM_HEAP  1
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
#ifndef USE_CMD_POWER
    #define USE_CMD_POWER  0
#endif
#ifndef USE_CMD_I2C
    #define USE_CMD_I2C  1
#endif
#ifndef USE_CMD_SPI
    #define USE_CMD_SPI  1
#endif
#ifndef USE_CMD_COUNTER
    #define USE_CMD_COUNTER  0
#endif
#ifndef USE_CMD_RTC
    #define USE_CMD_RTC  0
#endif



#if MCUSH_SPIFFS
    #ifndef USE_CMD_SPIFFS
        #define USE_CMD_SPIFFS  1
    #endif
#else
    #ifdef USE_CMD_SPIFFS
        #undef USE_CMD_SPIFFS
    #endif
    #define USE_CMD_SPIFFS  0
#endif

#if MCUSH_VFS
    #ifndef USE_CMD_CAT
        #define USE_CMD_CAT  1
    #endif
    #ifndef USE_CMD_RM
        #define USE_CMD_RM  1
    #endif
    #ifndef USE_CMD_RENAME
        #define USE_CMD_RENAME  1
    #endif
    #ifndef USE_CMD_CP
        #define USE_CMD_CP  1
    #endif
    #ifndef USE_CMD_LS
        #define USE_CMD_LS  1
    #endif
    #ifndef USE_CMD_LOAD
        #define USE_CMD_LOAD  1
    #endif
#else
    #ifdef USE_CMD_CAT
        #undef USE_CMD_CAT
    #endif
    #define USE_CMD_CAT  0
    #ifdef USE_CMD_RM
        #undef USE_CMD_RM
    #endif
    #define USE_CMD_RM  0
    #ifdef USE_CMD_RENAME
        #undef USE_CMD_RENAME
    #endif
    #define USE_CMD_RENAME  0
    #ifdef USE_CMD_CP
        #undef USE_CMD_CP
    #endif
    #define USE_CMD_CP  0
    #ifdef USE_CMD_LS
        #undef USE_CMD_LS
    #endif
    #define USE_CMD_LS  0
    #ifdef USE_CMD_LOAD
        #undef USE_CMD_LOAD
    #endif
    #define USE_CMD_LOAD  0
    
    #undef USE_CMD_UPGRADE
    #define USE_CMD_UPGRADE  0
#endif


int cmd_help( int argc, char *argv[] );
int cmd_scpi_idn( int argc, char *argv[] );
int cmd_scpi_rst( int argc, char *argv[] );
int cmd_reboot( int argc, char *argv[] );
int cmd_upgrade( int argc, char *argv[] );
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
int cmd_power( int argc, char *argv[] );
int cmd_i2c( int argc, char *argv[] );
int cmd_spi( int argc, char *argv[] );
int cmd_counter( int argc, char *argv[] );
int cmd_rtc( int argc, char *argv[] );
int cmd_spiffs( int argc, char *argv[] );
int cmd_cat( int argc, char *argv[] );
int cmd_rm( int argc, char *argv[] );
int cmd_rename( int argc, char *argv[] );
int cmd_copy( int argc, char *argv[] );
int cmd_list( int argc, char *argv[] );
int cmd_load( int argc, char *argv[] );



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
#if USE_CMD_REBOOT
{   CMD_HIDDEN, 0,  "reboot",  cmd_reboot, 
    "reboot device",
    "reboot" },
#endif
#if USE_CMD_UPGRADE
{   CMD_HIDDEN, 0,  "upgrade",  cmd_upgrade, 
    "upgrade firmware and restart",
    "upgrade -f filename" },
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
#if USE_CMD_POWER
{   0, 0,  "power",  cmd_power, 
    "set power",
    "power [on|off]" },
#endif
#if USE_CMD_I2C
{   0, 0,  "i2c",  cmd_i2c, 
    "i2c emulated",
    "i2c [-i|r] <data>" },
#endif
#if USE_CMD_SPI
{   0, 0,  "spi",  cmd_spi, 
    "spi emulated",
    "spi [-i|r] <data>" },
#endif
#if USE_CMD_COUNTER
{   0, 'c',  "counter",  cmd_counter, 
    "input trigger counter",
    "counter [-i|r]" },
#endif
#if USE_CMD_RTC
{   0, 0,  "rtc",  cmd_rtc, 
    "rtc",
    "rtc -s [val]" },
#endif
#if USE_CMD_BEEP
{   0,  'b',  "beep",  cmd_beep, 
    "beep control",
    "beep [-f <freq>] [<ms>]"  },
#endif
#if USE_CMD_SPIFFS
{   0, 's', "spiffs",  cmd_spiffs, 
    "spiffs test",
    "spiffs"  },
#endif
#if USE_CMD_CAT
{   0, 0, "cat",  cmd_cat, 
    "print file contents",
    "cat <pathname>"  },
#endif
#if USE_CMD_RM
{   0, 0, "rm",  cmd_rm, 
    "remove file",
    "rm <pathname>"  },
#endif
#if USE_CMD_RENAME
{   0, 0, "rename",  cmd_rename, 
    "rename file",
    "rename <old_pathname> <new>"  },
#endif
#if USE_CMD_CP
{   0, 0, "cp",  cmd_copy, 
    "copy file",
    "cp <src> <dst>"  },
#endif
#if USE_CMD_LS
{   0, 'l', "ls",  cmd_list, 
    "list files",
    "ls"  },
#endif
#if USE_CMD_LOAD
{   0,  0,  "load",  cmd_load, 
    "load script",
    "load <pathname>" },
#endif

{   CMD_END  } };



#if USE_CMD_HELP
int cmd_help( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'a', "all", 0, "show all" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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

#if USE_CMD_SCPI_IDN_CUSTOM
int cmd_scpi_idn_custom( int argc, char *argv[] );
#endif
#if USE_CMD_SCPI_IDN
int cmd_scpi_idn( int argc, char *argv[] )
{
#if USE_CMD_SCPI_IDN_CUSTOM
    return cmd_scpi_idn_custom( argc, argv );
#else
    /* *idn? command ignore all arguments */
    char buf[64];

    shell_write_line( "mcush," MCUSH_VERSION_STRING );
    buf[0] = 0;
    if( hal_get_serial_number(buf) && strlen(buf) )
        shell_write_line( buf );

    return 0;
#endif
}
#endif


#if USE_CMD_SCPI_RST
int cmd_scpi_rst( int argc, char *argv[] )
{
    /* *rst command ignore all arguments */
    hal_reset();
    return 0;
}
#endif


#if USE_CMD_REBOOT
int cmd_reboot( int argc, char *argv[] )
{
#ifdef HAL_REBOOT_COUNTER
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', "count", 0, "print counter" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', "reset", 0, "reset counter" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t count_flag=0, reset_flag=0;
        
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "count" ) == 0 )
                count_flag = 1;
            if( strcmp( opt.spec->name, "reset" ) == 0 )
                reset_flag = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( count_flag || reset_flag )
    {
        if( count_flag )
            shell_printf( "%u\n", hal_reboot_counter_get() );
        if( reset_flag )
            hal_reboot_counter_reset();
    }
    else 
    {
        hal_reboot();
        while( 1 );
    }
#else
    /* reboot command ignore all arguments */
    hal_reboot();
    while( 1 );
#endif
    return 0;
}
#endif


#if USE_CMD_UPGRADE
int cmd_upgrade( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', "file", "upgrade file", "binary file name" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          's', "sign", "signature", "CRC signature" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char fname[32];
    
    fname[0] = 0;
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
                strcpy( fname, (char*)opt.value );
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( strlen(fname) == 0 )
        return 1;

    shell_printf("Loading firmware from %s...\n", fname);
    vTaskDelay(configTICK_RATE_HZ/5);  // 0.2s to confirm the uart output
    if( !hal_upgrade_prepare_swap( fname, 0 ) )
        return 1; 
    shell_printf("Upgrading...\n");
    vTaskDelay(configTICK_RATE_HZ/5);  // 0.2s to confirm the uart output
    hal_upgrade_run_stage2();
    //while( 1 );
    return 0;
}
#endif


#if USE_CMD_GPIO
int cmd_gpio( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'l', "loop", 0, "loop mode" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          0, "loop_delay", "loop delay value", "default 1000ms" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'p', "port", "port_bit_name", "port[.bit] name, eg 0[.0]" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'i', "input", "input_mode", "set input mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'o', "output", "output_mode", "set output mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          's', "set", "set_high_val", "set output high mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'c', "clr", "set_low_val", "set output low mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          't', "toggle", "toggle_val", "toggle output mask" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'n', "port_num", 0, "query number" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'U', "pullup", 0, "with pullup resister" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'D', "pulldown", 0, "with pulldown resister" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
        tick = xTaskGetTickCount();
        while( xTaskGetTickCount() < tick + loop_delay*configTICK_RATE_HZ/1000 )
        {
            if( shell_driver_read_char_blocked(&c, 10*configTICK_RATE_HZ/1000) != -1 )
            {
                if( c == 0x03 ) /* Ctrl-C for stop */
                    return 0;
            }
        }
        goto loop_start;
    }
 
    return 0;
port_error:
    shell_write_err( shell_str_port_bit );
    return 1;
parm_error:
    shell_write_err( shell_str_parameter );
    return 1;
}
#endif


#if USE_CMD_LED
int cmd_led( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          's', "set", 0, "set on" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', "toggle", 0, "invert" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', "clr", 0, "set off" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', "index", "led_index", "idx from 0" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'n', "led_num", 0, "query number" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'T', "test", 0, "blink all" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
            if( (shell_driver_read_char_blocked(&c, 250*configTICK_RATE_HZ/1000) != -1 ) && (c == 0x03) )  /* Ctrl-C for quit */
                break;
            for( index=0; index<led_num; index++ )
            {
                if( test_on )
                    hal_led_set( index );
                else
                    hal_led_clr( index );
            }
            test_on ^= 1;
        }
        for( index=0; index<led_num; index++ )
            hal_led_clr( index );
        return 0;
    }

    if( (index == -1) || ((index < 0) || (index > (led_num-1))) )
    {
        shell_write_err( shell_str_index );
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, "address", "base address" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "default 16" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, shell_str_width, "1(default)|2|4" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', "compact", 0, "compact output" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', "float", 0, "float output (width=4)" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'C', "ascii", 0, "ascii output (width=1)" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "int", 0, "signed integer output" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', "uint", 0, "unsigned integer output" },
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

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, shell_str_length ) == 0 )
                shell_eval_int(opt.value, (int*)&length);
            else if( strcmp( opt.spec->name, shell_str_width ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, "compact" ) == 0 )
                compact_mode = 1;
            else if( strcmp( opt.spec->name, "ascii" ) == 0 )
                ascii_mode = 1;
            else if( strcmp( opt.spec->name, "int" ) == 0 )
            {
                integer_mode = 1;
                unsigned_mode = 0;
            }
            else if( strcmp( opt.spec->name, "uint" ) == 0 )
            {
                integer_mode = 1;
                unsigned_mode = 1;
            }
            else if( strcmp( opt.spec->name, "float" ) == 0 )
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
        switch(width)
        {
        case 1:
            for( i=0; i<16; i++ )
            {
                shell_printf( compact_mode ? "%02X" : "%02X ", *(unsigned char*)addr );
                addr = (void*)(((int)addr) + 1 );
                count += 1;
                if( count >= length )
                {
                    if( need_fill_line )
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
            break;
        case 2:
            for( i=0; i<8; i++ )
            {
                shell_printf( compact_mode ? "%04X" : "%04X ", *(unsigned short*)addr );
                addr = (void*)(((int)addr) + 2 );
                count += 2;
                if( count >= length )
                {
                    if( need_fill_line )
                    {
                        for( i++; i<8; i++ )
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
            break;
        case 4:
            for( i=0; i<4; i++ )
            {
                shell_printf( compact_mode ? "%08X" : "%08X ", *(unsigned int*)addr );
                addr = (void*)(((int)addr) + 4 );
                count += 4;
                if( count >= length )
                {
                    if( need_fill_line )
                    {
                        for( i++; i<4; i++ )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, "address", "base address" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, "bus width", "1(default)|2|4" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "data", 0, "data to be written" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *addr=(void*)-1;
    int width=1;
    int dat;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, shell_str_width ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, shell_str_data ) == 0 )
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
        if( ! shell_eval_int(argv[parser.idx], &dat) )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, "address", "base address" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "memory length" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, "bus width", "1(default)|2|4" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'p', "pattern", "pattern", "data to be written" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', "test", 0, "test mode" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, "pattern" ) == 0 )
                shell_eval_int(opt.value, (int*)&pattern);
            else if( strcmp( opt.spec->name, shell_str_length ) == 0 )
                shell_eval_int(opt.value, (int*)&length);
            else if( strcmp( opt.spec->name, shell_str_width ) == 0 )
                shell_eval_int(opt.value, (int*)&width);
            else if( strcmp( opt.spec->name, "test" ) == 0 )
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


#if USE_CMD_WAIT
int cmd_wait( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "time", 0, "default 1000ms" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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

    vTaskDelay( ms * configTICK_RATE_HZ / 1000  );
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "cmd", 0, "enable|disable|clear|reset" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
        portENTER_CRITICAL();
        while(1);
    }
    else
        return -1;
    
    return 0;
}
#endif


#if USE_CMD_UPTIME
int cmd_uptime( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "type", 0, "(t)ask|(q)ueue|(k)ern|heap|stack|trace" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
                if( mcushGetQueueInfo( xQueue, &qinfo ) )
                {
                    shell_printf( "%8s 0x%08X  %4d %4d %4d  0x%08X - 0x%08X (0x%08X)\n", name, (int)xQueue, 
                        qinfo.uxLength, qinfo.uxItemSize, qinfo.uxMessagesWaiting, 
                        (int)qinfo.pcHead, (int)qinfo.pcTail, ((int)qinfo.pcTail-(int)qinfo.pcHead) );
                }
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
        {
            shell_printf( "ReadyTaskLists[%d]:    0x%08X %s\n", i, (uint32_t)kinfo.pxReadyTaskLists[i], mcushGetTaskNamesFromTaskList(kinfo.pxReadyTaskLists[i], buf) );
        }
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
    else if( strcmp( type, "heap" ) == 0 )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', "test", 0, "test heap memory" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "fill", 0, "fill heap memory" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "info", 0, "print mallinfo" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'm', "malloc", 0, "allocate new memory" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', "realloc", 0, "re-allocate memory" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', "free", 0, "free memory" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, "address", "base address" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "memory length" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, shell_str_length ) == 0 )
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
            shell_write_err( shell_str_length );
            return -1;
        }
        if( realloc_set )
        {
            if( !addr || ((int)addr == -1) ) 
            {
                shell_write_err( shell_str_address );
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
            shell_write_err( shell_str_address );
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', shell_str_frequency, "frequency", "20~10000(default 4000)hz" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "ms", 0, "default 50ms" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int freq=-1, ms=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_frequency ) == 0 )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', "float", 0, "float mode" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    void *buf;
    int buf_len;
    uint8_t float_mode=0;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "float" ) == 0 )
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
                
    shell_printf( "0x%08X  %d\n", buf, buf_len );
    return 0;
}
#endif


#if USE_CMD_SGPIO
int cmd_sgpio( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
         'p', "port", "port_index", "index from 0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'o', "output", "output_mode", "set output mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', "input", "input_mode", "set input mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "input_len", "input_buf_len", "length of input buffer" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', shell_str_frequency, "frequency", "1~1000000hz" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'l', "loop", 0, "loop mode" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', "start", 0, "run" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          's', "stop", 0, "stop" }, 
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
           0, "info", 0, "print info" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
            else if( strcmp( opt.spec->name, shell_str_frequency ) == 0 )
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
        shell_write_err( "port" );
        return -1;
    }
   
    if( output == -1 )
        output = 0;
    if( input == -1 )
        input = 0;
    
    if( !output && !input )
    {
        shell_write_err( shell_str_io );
        return -1;
    }

    if( !( (freq_val >= SGPIO_FREQ_MIN) && (freq_val <= SGPIO_FREQ_MAX) ) )
    {
        shell_write_err( shell_str_frequency );
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
                shell_write_err( shell_str_length );
                return -1;
            }
        }
        buf_in = malloc( buf_len * 2 );
    }

    return hal_sgpio_setup( loop, port, output, input, buf_out, buf_in, buf_len, freq_val ) ? 0 : 1;
}
#endif


#if USE_CMD_POWER
int cmd_power( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "val", 0, "0|1|on|off" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int set=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "val" ) == 0 )
            {
                set=1;
                if( strcmp(opt.value, "0")==0 || strcmp(opt.value, "off")==0 )
                    hal_power_set( 0 );
                else if( strcmp(opt.value, "1")==0 || strcmp(opt.value, "on")==0 )
                    hal_power_set( 1 );
                else
                {
                    shell_write_err( shell_str_parameter );
                    return -1;
                }
            }
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( set == -1 )
    {
        shell_write_line( hal_is_power_set() ? "on" : "off" );
    } 
    return 0;
}
#endif


#if USE_CMD_I2C
#ifndef CMD_I2C_SDA_PORT
#define CMD_I2C_SDA_PORT  0
#endif
#ifndef CMD_I2C_SDA_PIN
#define CMD_I2C_SDA_PIN  0
#endif
#ifndef CMD_I2C_SCL_PORT
#define CMD_I2C_SCL_PORT  0
#endif
#ifndef CMD_I2C_SCL_PIN
#define CMD_I2C_SCL_PIN  1
#endif
static uint8_t i2c_port_sda=CMD_I2C_SDA_PORT, i2c_port_scl=CMD_I2C_SCL_PORT;
static uint16_t i2c_pin_sda=1<<(CMD_I2C_SDA_PIN), i2c_pin_scl=1<<(CMD_I2C_SCL_PIN);
static uint32_t i2c_addr=0;
static uint32_t i2c_delay_us=5;

static void i2c_start(void)
{
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    hal_gpio_set( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    hal_gpio_clr( i2c_port_sda, i2c_pin_sda );
    hal_delay_us( i2c_delay_us );
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
}

static void i2c_stop(void)
{
    hal_gpio_clr( i2c_port_sda, i2c_pin_sda );
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    hal_gpio_set( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
}

static int i2c_read_ack(void)
{
    int ret;
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    hal_gpio_set( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    ret = hal_gpio_get( i2c_port_sda, i2c_pin_sda );
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    return ret ? 0 : 1;
}

static void i2c_write_ack(int ack)
{
    if( ack )
        hal_gpio_clr( i2c_port_sda, i2c_pin_sda );
    else
        hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    hal_delay_us( i2c_delay_us );
    hal_gpio_set( i2c_port_scl, i2c_pin_scl );
    hal_delay_us( i2c_delay_us );
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
}

static uint8_t i2c_read_byte(int ack)
{
    int i;
    uint8_t ret=0;
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    for( i=0; i<8; i++ )
    {     
        hal_gpio_set( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
        if( hal_gpio_get( i2c_port_sda, i2c_pin_sda ) )
            ret |= 1 << (8-1-i);
        hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
    } 
    i2c_write_ack( ack );
    return ret;
}

static int i2c_write_byte(uint8_t chr)
{
    int i;
    for( i=0; i<8; i++ )
    {     
        if( chr & 1<<(8-1-i) )
            hal_gpio_set( i2c_port_sda, i2c_pin_sda );
        else
            hal_gpio_clr( i2c_port_sda, i2c_pin_sda );
        hal_delay_us( i2c_delay_us );
        hal_gpio_set( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
        hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
    }
    return i2c_read_ack(); 
}


int cmd_i2c( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "delay", "delay_us", "default 5" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'a', shell_str_address, "address", "default 0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sda", "sda_pin", "default 0.0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "scl", "scl_pin", "default 0.1" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "init", 0, "init pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'd', "deinit", 0, "deinit pins" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'r', "read", "read_cycle", "default 1" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "val", 0, "data" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0;
    int read_cycle=0;
    char *p;
    int i, dat, line_count;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&i2c_addr);
            else if( strcmp( opt.spec->name, "delay" ) == 0 )
                shell_eval_int(opt.value, (int*)&i2c_delay_us);
            else if( strcmp( opt.spec->name, "read" ) == 0 )
                shell_eval_int(opt.value, (int*)&read_cycle);
            else if( strcmp( opt.spec->name, "init" ) == 0 )
                init = 1;
            else if( strcmp( opt.spec->name, "deinit" ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "scl" ) == 0 )
            {
                i2c_port_scl = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                i2c_pin_scl = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                i2c_pin_scl = 1 << i2c_pin_scl;
            }
            else if( strcmp( opt.spec->name, "sda" ) == 0 )
            {
                i2c_port_sda = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                i2c_pin_sda = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                i2c_pin_sda = 1 << i2c_pin_sda;
            }
            else if( strcmp( opt.spec->name, "val" ) == 0 )
            {
                parser.idx--;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init )
    {
        hal_gpio_set_output( i2c_port_scl, i2c_pin_scl );
        hal_gpio_set_output_open_drain( i2c_port_sda, i2c_pin_sda );
        i2c_stop();
        return 0;
    }
    else if( deinit )
    {
        hal_gpio_set_input( i2c_port_scl, i2c_pin_scl );
        hal_gpio_set_input( i2c_port_sda, i2c_pin_sda );
        return 0;
    }
    
    i2c_start();
    if( ! i2c_write_byte( i2c_addr<<1 ) )
    {
        i2c_stop();
        shell_write_line("err ACK");
        return 1;
    }
    /* write data */ 
    parser.idx++;
    while( parser.idx < argc )
    {
        if( ! shell_eval_int(argv[parser.idx], &dat) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            i2c_stop();
            return 1;
        }
        i2c_write_byte( dat );
        parser.idx++;
    }
    /* then read back */
    if( read_cycle )
    {
        i2c_start();
        i2c_write_byte( (i2c_addr<<1)|1 );
        /* read bytes */
        line_count = 0;
        for( i=read_cycle; i; i-- )
        {
            dat = i2c_read_byte( i>1 ? 1 : 0 ); 
            line_count += shell_printf( "0x%02X ", dat );
            if( line_count > 78 )
            {
                shell_write_char( '\n' );
                line_count =0;
            }
        }
        if( line_count )
            shell_write_char( '\n' );
    }
    i2c_stop();
    return 0;
    
err_port:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


#if USE_CMD_SPI
#ifndef CMD_SPI_SDI_PORT
#define CMD_SPI_SDI_PORT  0
#endif
#ifndef CMD_SPI_SDI_PIN
#define CMD_SPI_SDI_PIN  0
#endif
#ifndef CMD_SPI_SDO_PORT
#define CMD_SPI_SDO_PORT  0
#endif
#ifndef CMD_SPI_SDO_PIN
#define CMD_SPI_SDO_PIN  1
#endif
#ifndef CMD_SPI_SCK_PORT
#define CMD_SPI_SCK_PORT  0
#endif
#ifndef CMD_SPI_SCK_PIN
#define CMD_SPI_SCK_PIN  2
#endif
#ifndef CMD_SPI_CS_PORT
#define CMD_SPI_CS_PORT  0
#endif
#ifndef CMD_SPI_CS_PIN
#define CMD_SPI_CS_PIN  3
#endif
static uint8_t spi_port_sdi=CMD_SPI_SDI_PORT, spi_port_sdo=CMD_SPI_SDO_PORT, spi_port_sck=CMD_SPI_SCK_PORT, spi_port_cs=CMD_SPI_CS_PORT;
static uint16_t spi_pin_sdi=1<<(CMD_SPI_SDI_PIN), spi_pin_sdo=1<<(CMD_SPI_SDO_PIN), spi_pin_sck=1<<(CMD_SPI_SCK_PIN), spi_pin_cs=1<<(CMD_SPI_CS_PIN);
static uint32_t spi_delay_us=5;
static uint32_t spi_width=8;
static uint8_t spi_cpol=0, spi_cpha=0, spi_lsb=0;

static uint32_t spi_rw(uint32_t dat)
{
    uint32_t ret=0;
    int i;

    for( i=0; i<spi_width; i++ )
    {
        /* sdo output */
        if( dat & 1<<(spi_lsb ? i : (spi_width-1-i)) )
            hal_gpio_set( spi_port_sdo, spi_pin_sdo );
        else
            hal_gpio_clr( spi_port_sdo, spi_pin_sdo );
        /* sck */
        if( spi_cpha ^ spi_cpol )
            hal_gpio_set( spi_port_sck, spi_pin_sck );
        else
            hal_gpio_clr( spi_port_sck, spi_pin_sck );
        /* delay 1 */
        hal_delay_us( spi_delay_us );
        /* sck */
        if( spi_cpha ^ spi_cpol )
            hal_gpio_clr( spi_port_sck, spi_pin_sck );
        else
            hal_gpio_set( spi_port_sck, spi_pin_sck );
        /* sdo */
        if( dat & 1<<(spi_lsb ? i : (spi_width-1-i)) )
            hal_gpio_set( spi_port_sdo, spi_pin_sdo );
        else
            hal_gpio_clr( spi_port_sdo, spi_pin_sdo );
        /* sdi */
        if( hal_gpio_get( spi_port_sdi, spi_pin_sdi ) )
            ret |= 1<<(spi_lsb ? i : (spi_width-1-i));
        /* delay 2 */
        hal_delay_us( spi_delay_us );
    }
    return ret;    
}


int cmd_spi( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_width, "bits", "default 8" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "delay", "delay_us", "default 5" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sdi", "sdi_pin", "default 0.0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sdo", "sdo_pin", "default 0.1" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sck", "sck_pin", "default 0.2" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "cs", "cs_pin", "default 0.3" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "init", 0, "init pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'd', "deinit", 0, "deinit pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', "read", 0, "print readout" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "cpol", 0, "clk polarity" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "cpha", 0, "clk phase" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "lsb", 0, "lsb first" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "val", 0, "data" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0, read_mode=0, cpol=0, cpha=0, lsb=0;
    char *p;
    int dat_out, dat_in;
    int line_count;
 
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "delay" ) == 0 )
                shell_eval_int(opt.value, (int*)&spi_delay_us);
            else if( strcmp( opt.spec->name, "read" ) == 0 )
                read_mode = 1;
            else if( strcmp( opt.spec->name, shell_str_width ) == 0 )
                shell_eval_int(opt.value, (int*)&spi_width);
            else if( strcmp( opt.spec->name, "init" ) == 0 )
                init = 1;
            else if( strcmp( opt.spec->name, "deinit" ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "cpol" ) == 0 )
                cpol = 1;
            else if( strcmp( opt.spec->name, "cpha" ) == 0 )
                cpha = 1;
            else if( strcmp( opt.spec->name, "lsb" ) == 0 )
                lsb = 1;
            else if( strcmp( opt.spec->name, "sdi" ) == 0 )
            {
                spi_port_sdi = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_pin_sdi = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                spi_pin_sdi = 1 << spi_pin_sdi;
            }
            else if( strcmp( opt.spec->name, "sdo" ) == 0 )
            {
                spi_port_sdo = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_pin_sdo = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                spi_pin_sdo = 1 << spi_pin_sdo;
            }
            else if( strcmp( opt.spec->name, "sck" ) == 0 )
            {
                spi_port_sck = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_pin_sck = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                spi_pin_sck = 1 << spi_pin_sck;
            }
            else if( strcmp( opt.spec->name, "cs" ) == 0 )
            {
                spi_port_cs = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_pin_cs = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                spi_pin_cs = 1 << spi_pin_cs;
            }
            else if( strcmp( opt.spec->name, "val" ) == 0 )
            {
                parser.idx--;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( (spi_width < 2) || (spi_width > 32) )
    {
        shell_write_err( shell_str_width );
        return 1;
    }

    if( init )
    {
        spi_cpol = cpol;
        spi_cpha = cpha;
        spi_lsb = lsb;
        hal_gpio_set_input( spi_port_sdi, spi_pin_sdi );
        hal_gpio_set_output( spi_port_sdo, spi_pin_sdo );
        hal_gpio_set_output( spi_port_sck, spi_pin_sck );
        hal_gpio_set_output( spi_port_cs, spi_pin_cs );
        hal_gpio_set( spi_port_cs, spi_pin_cs );
        return 0;
    }
    else if( deinit )
    {
        hal_gpio_set_input( spi_port_sdi, spi_pin_sdi );
        hal_gpio_set_input( spi_port_sdo, spi_pin_sdo );
        hal_gpio_set_input( spi_port_sck, spi_pin_sck );
        hal_gpio_set_input( spi_port_cs, spi_pin_cs );
        return 0;
    }

    if( spi_cpol )
        hal_gpio_set( spi_port_sck, spi_pin_sck );
    else
        hal_gpio_clr( spi_port_sck, spi_pin_sck );
    hal_gpio_clr( spi_port_cs, spi_pin_cs );
    parser.idx++;
    line_count = 0;
    while( parser.idx < argc )
    {
        if( ! shell_eval_int(argv[parser.idx], &dat_out) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            hal_gpio_set( spi_port_cs, spi_pin_cs );
            return 1;
        }
        dat_in = spi_rw( dat_out );
        if( read_mode )
        {
            line_count += shell_printf( "0x%X ", dat_in );
            if( line_count > 78 )
            {
                shell_write_char( '\n' );
                line_count = 0;
            }
        }
        parser.idx++;
    }
    if( read_mode && line_count )
        shell_write_char( '\n' );
    hal_gpio_set( spi_port_cs, spi_pin_cs );
    return 0;
 
err_port:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


#if USE_CMD_COUNTER
static uint8_t counter_port=0;
static uint16_t counter_pin=1<<0;
static uint32_t counter_val=0;

int cmd_counter( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, 0, "pin", "input_pin", "default 0.0", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_SWITCH, 'i', "init", 0, "init pin", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, 0, "deinit", 0, "deinit pin", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_SWITCH, 'r', "reset", 0, "reset zero", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0, reset=0;
    char *p;
 
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "init" ) == 0 )
                init = 1;
            else if( strcmp( opt.spec->name, "deinit" ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "reset" ) == 0 )
                reset = 1;
            else if( strcmp( opt.spec->name, "pin" ) == 0 )
            {
                counter_port = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                counter_pin = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
                counter_pin = 1 << counter_pin;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init )
    {
        hal_gpio_set_input( counter_port, counter_pin );
        counter_val = 0;
        return 0;
    }
    else if( deinit )
    {
        hal_gpio_set_input( counter_port, counter_pin );
        return 0;
    }

    if( reset )
    {
        counter_val = 0;
        return 0;
    }

    shell_printf("%d\n", counter_val);
    return 0;
 
err_port:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


#if USE_CMD_RTC
int cmd_rtc( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          's', "set", 0, "set rtc" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "setting", 0, "format: YYYY-MM-DD HH:MM:SS" },
        { MCUSH_OPT_NONE } };
    int8_t set=-1;
    struct tm t;
     
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "set" ) == 0 )
                set = 1;
            else if( strcmp( opt.spec->name, "setting" ) == 0 )
                break;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( set == 1 )
    {
        if( (parser.idx >= argc) || \
            (3 != sscanf( argv[parser.idx], "%d-%d-%d", 
                      &t.tm_year, &t.tm_mon, &t.tm_mday )) )
        {
            shell_write_err( shell_str_data );
            return 1;
        }
        t.tm_wday = 0;
 
        parser.idx++;     
        if( (parser.idx >= argc) || \
            (3 != sscanf( argv[parser.idx], "%d:%d:%d", 
                      &t.tm_hour, &t.tm_min, &t.tm_sec )) )
        {
            shell_write_err( shell_str_time );
            return 1;
        }
 
        if( ! hal_rtc_set( &t ) )
        {
            shell_write_err( "rtc_set" );
            return 1;
        }
    }
    else
    {
        if( ! hal_rtc_get( &t ) )
        {
            shell_write_line( "not synced" );
            return 1;
        }
        shell_printf( "%d-%d-%d %02d:%02d:%02d\n", 
                      t.tm_year, t.tm_mon, t.tm_mday,
                      t.tm_hour, t.tm_min, t.tm_sec );
    }
    
    return 0;
}
#endif


#if USE_CMD_SPIFFS
#include "mcush_vfs_spiffs.h"
#include "spi_flash.h"
int cmd_spiffs( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'b', shell_str_address, "address", "base address" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', "command", "cmd_name", "erase|read|write|mount|umount|test|format|check|info" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *cmd=0;
    void *addr=(void*)-1;
    char buf[256];
    int i, j;
    int len;
    void *p;
    int fd;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_address ) == 0 )
                shell_eval_int(opt.value, (int*)&addr);
            else if( strcmp( opt.spec->name, shell_str_command) == 0 )
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
        shell_printf( "total: %d  used: %d\n", i, j );
        return 0;
    }
    if( strcmp( cmd, shell_str_erase ) == 0 )
    {
        sFLASH_EraseBulk();
    }
    else if( strcmp( cmd, shell_str_read ) == 0 )
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
    else if( strcmp( cmd, shell_str_write ) == 0 )
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
        if( ! mcush_spiffs_mount() )
            return 1;
    }
    else if( strcmp( cmd, "umount" ) == 0 )
    {
        if( ! mcush_spiffs_umount() )
            return 1;
    }
    else if( strcmp( cmd, "test" ) == 0 )
    {
        if( ! mcush_spiffs_mounted() )
            goto not_mounted;
        fd = mcush_spiffs_open( "test.dat", "rwa+" );
        if( fd < 0 )
            return 1;
        strcpy( buf, "abcdefghijklmnopqrstuvwxyz\n" );
        mcush_spiffs_write( fd, buf, strlen(buf) );
        strcpy( buf, "01234567890\n" );
        mcush_spiffs_write( fd, buf, strlen(buf) );
        mcush_spiffs_close( fd );
    }
    else if( strcmp( cmd, shell_str_format ) == 0 )
    {
        if( mcush_spiffs_mounted() )
            mcush_spiffs_umount();
        //    goto not_mounted;
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


#if USE_CMD_CAT
#define CAT_BUF_RAW  100
#define CAT_BUF_B64  180
#define CAT_BUF_LEN  (CAT_BUF_RAW+CAT_BUF_B64)
int cmd_cat( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'b', "b64", 0, "base 64 code" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'w', "write", 0, "write mode" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'a', "append", 0, "append mode" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "file", 0, "file name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t write=0, append=0, b64=0;
    char fname[32];
    char buf[CAT_BUF_LEN];
    int i, j;
    int fd;
    void *input=0;
    char *p, *p2;
    base64_encodestate state_en;
    base64_decodestate state_de;
    char c;

    fname[0] = 0;
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "write" ) == 0 )
                write = 1;
            if( strcmp( opt.spec->name, "append" ) == 0 )
                append = 1;
            if( strcmp( opt.spec->name, "b64" ) == 0 )
            {
                b64 = 1;
                base64_init_encodestate( &state_en );
                base64_init_decodestate( &state_de );
            }
            if( strcmp( opt.spec->name, "file" ) == 0 )
                strcpy( fname, (char*)opt.value );
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( ! fname[0] )
        return -1;
       
    if( write || append )
    {
        input = shell_read_multi_lines(0);
        if( !input )
            return 0;
        i = strlen(input);
        if( !i )
            return 0;
        
        fd = mcush_open( fname, append ? "a+" : "w+" );
        if( fd == 0 )
            return 1;
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
                        free(input);
                        mcush_close(fd);
                        return 1;
                    } 
                }
            }
        }
        else
        {
            if( i != mcush_write( fd, input, i ) )
            {
                free(input);
                mcush_close(fd);
                return 1;
            } 
        }
        free(input);
        mcush_close(fd);
    }
    else
    { 
        fd = mcush_open( fname, "r" );
        if( fd == 0 )
            return 1;

        while( 1 )
        {    
            i = mcush_read( fd, buf, b64 ? CAT_BUF_RAW : CAT_BUF_LEN );
            if( i==0 )
                break;
            if( b64 )
            {
                j = base64_encode_block( buf, i, &buf[CAT_BUF_RAW], &state_en );
                shell_write( buf + CAT_BUF_RAW, j );
            }
            else
                shell_write( buf, i );
            while( shell_driver_read_char_blocked(&c, 0) != -1 )
            {
                if( c == 0x03 ) /* Ctrl-C for stop */
                {
                    shell_write_char( '\n' );
                    mcush_close(fd);
                    return 0;
                }
            }
            if( i < (b64 ? CAT_BUF_RAW : CAT_BUF_LEN) )
            {
                if( b64 )
                {
                    j = base64_encode_blockend( buf, &state_en );
                    shell_write( buf, j );
                }
                break;
            }
        }
        shell_write_str( "\n" );
        mcush_close(fd);
    }
    return 0;
}
#endif


#if USE_CMD_RM
int cmd_rm( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "file", 0, "file name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, shell_str_file ) == 0 )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "file", 0, "old -> new" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
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
        return -1;

    return mcush_rename( fname, fname2 ) ? 0 : 1;
}
#endif


#if USE_CMD_CP
int cmd_copy( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "file", 0, "file name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *fname=0, *fname2=0;
    char buf[256];
    int i, j;
    int fd, fd2;

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
    if( cmd_list_match_file_name && strcmp(cmd_list_match_file_name, name) != 0 )
        return;
    shell_printf("%6d  %s\n", size, name );
}

extern mcush_vfs_volume_t vfs_vol_tab[MCUSH_VFS_VOLUME_NUM];

int cmd_list( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "path", 0, "path name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char *path=0;
    char mount_point[8], file_name[32];
    int i;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "path" ) == 0 )
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
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, "file", 0, "file name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char fname[32];
    int size;
    int fd;
    void *buf=0;
    int i;

    fname[0] = 0;
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "file" ) == 0 )
                strcpy( fname, (char*)opt.value );
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( ! fname[0] )
        return -1;

    mcush_size( fname, &size );
    if( !size )
        return 0;

    buf = malloc( size + 1 );
    if( !buf )
    {
        shell_write_err( shell_str_script );
        return 1;
    }

    fd = mcush_open( fname, "r" );
    if( fd == 0 )
        return 1;
    
    i = mcush_read( fd, buf, size );   
    mcush_close(fd);

    if( i != size )
    {
        shell_write_err( shell_str_script );
        return 1;
        free(buf);
    }
  
    ((char*)buf)[i] = 0; 
    shell_set_script( buf, 1 );
    return 0;
}
#endif


