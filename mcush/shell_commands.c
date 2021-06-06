/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

extern int cmd_help( int argc, char *argv[] );
extern int cmd_scpi_idn( int argc, char *argv[] );
extern int cmd_scpi_rst( int argc, char *argv[] );
extern int cmd_reboot( int argc, char *argv[] );
extern int cmd_gpio( int argc, char *argv[] );
extern int cmd_led( int argc, char *argv[] );
extern int cmd_dump( int argc, char *argv[] );
extern int cmd_write( int argc, char *argv[] );
extern int cmd_mfill( int argc, char *argv[] );
extern int cmd_delay( int argc, char *argv[] );
extern int cmd_echo( int argc, char *argv[] );
extern int cmd_uptime( int argc, char *argv[] );
extern int cmd_system( int argc, char *argv[] );
extern int cmd_mapi( int argc, char *argv[] );
extern int cmd_beep( int argc, char *argv[] );
extern int cmd_sgpio( int argc, char *argv[] );
extern int cmd_mkbuf( int argc, char *argv[] );
extern int cmd_power( int argc, char *argv[] );
extern int cmd_i2c( int argc, char *argv[] );
extern int cmd_spi( int argc, char *argv[] );
extern int cmd_pulse( int argc, char *argv[] );
extern int cmd_ds1w( int argc, char *argv[] );
extern int cmd_pwm( int argc, char *argv[] );
extern int cmd_adc( int argc, char *argv[] );
extern int cmd_daq( int argc, char *argv[] );
extern int cmd_rtc( int argc, char *argv[] );
extern int cmd_can( int argc, char *argv[] );
extern int cmd_ws2812( int argc, char *argv[] );
extern int cmd_fcfs( int argc, char *argv[] );
extern int cmd_spiffs( int argc, char *argv[] );
extern int cmd_fatfs( int argc, char *argv[] );
extern int cmd_upgrade( int argc, char *argv[] );
extern int cmd_cat( int argc, char *argv[] );
extern int cmd_rm( int argc, char *argv[] );
extern int cmd_rename( int argc, char *argv[] );
extern int cmd_copy( int argc, char *argv[] );
extern int cmd_list( int argc, char *argv[] );
extern int cmd_load( int argc, char *argv[] );
extern int cmd_stop( int argc, char *argv[] );
extern int cmd_crc( int argc, char *argv[] );
extern int cmd_loop( int argc, char *argv[] );
extern int cmd_test( int argc, char *argv[] );



const shell_cmd_t CMD_TAB[] = {
#if USE_CMD_HELP
{   0,  '?',  shell_str_help,  cmd_help, 
    "command list",
    "? [-a]"  },
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
{   CMD_HIDDEN, 0,  shell_str_reboot,  cmd_reboot, 
    "reboot device",
    shell_str_reboot },
#endif
#if USE_CMD_DUMP
{   CMD_HIDDEN,  'x',  "dump",  cmd_dump, 
    "dump memory",
    "x -b <address> [-l <length>] [-w 1|2|4]" },
#endif
#if USE_CMD_WRITE
{   CMD_HIDDEN,  'w',  "write",  cmd_write, 
    "write memory",
    "w -b <address> [-w 1|2|4] dat1...N" },
#endif
#if USE_CMD_MFILL
{   CMD_HIDDEN,  0,  "mfill",  cmd_mfill, 
    "fill/test memory",
    "mfill -b <address> [-l <length>] [-w 1|2|4] [-p <pattern>] [-t]" },
#endif
#if USE_CMD_MAPI
{   CMD_HIDDEN,  0,  "mapi",  cmd_mapi,
    "memory api",
    "mapi -t" },
#endif
#if USE_CMD_DELAY
{   CMD_HIDDEN,  0,  "delay",  cmd_delay, 
    "sleep ms",
    "delay <ms>" },
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
#if USE_CMD_UPTIME
{   CMD_HIDDEN,  0,  "uptime",  cmd_uptime, 
    "run time",
    "uptime" },
#endif
#if USE_CMD_SYSTEM
{   CMD_HIDDEN,  0,  "sys",  cmd_system, 
    "system info",
    "sys t|k" },
#endif
#if USE_CMD_GPIO
{   0,  0,  "gpio",  cmd_gpio, 
    "control gpio",
    "gpio -p <port.bit> [-i|o|s|c|t]" },
#endif
#if USE_CMD_SGPIO
{   0,  0,  "sgpio",  cmd_sgpio, 
    "control sgpio",
    "sgpio -p <port> -o <val> -i <val> -f <val>" },
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
#if USE_CMD_I2C2
{   0, 0,  "i2c2",  cmd_i2c, 
    "i2c emulated",
    "same as i2c" },
#if USE_CMD_I2C3
{   0, 0,  "i2c3",  cmd_i2c, 
    "i2c emulated",
    "same as i2c" },
#if USE_CMD_I2C4
{   0, 0,  "i2c4",  cmd_i2c, 
    "i2c emulated",
    "same as i2c" },
#endif
#endif
#endif
#endif
#if USE_CMD_SPI
{   0, 0,  "spi",  cmd_spi, 
    "spi emulated",
    "spi [-r] <data>" },
#if USE_CMD_SPI2
{   0, 0,  "spi2",  cmd_spi, 
    "spi emulated",
    "same as spi" },
#if USE_CMD_SPI3
{   0, 0,  "spi3",  cmd_spi, 
    "spi emulated",
    "same as spi" },
#if USE_CMD_SPI4
{   0, 0,  "spi4",  cmd_spi, 
    "spi emulated",
    "same as spi" },
#endif
#endif
#endif
#endif
#if USE_CMD_PULSE
{   0, 0,  "pulse",  cmd_pulse, 
    "pulse generator",
    "pulse <number>" },
#if USE_CMD_PULSE2
{   0, 0,  "pulse2",  cmd_pulse, 
    "pulse generator",
    "same as pulse" },
#if USE_CMD_PULSE3
{   0, 0,  "pulse3",  cmd_pulse, 
    "pulse generator",
    "same as pulse" },
#if USE_CMD_PULSE4
{   0, 0,  "pulse4",  cmd_pulse, 
    "pulse generator",
    "same as pulse" },
#endif
#endif
#endif
#endif
#if USE_CMD_DS1W
{   0, 0,  "ds1w",  cmd_ds1w, 
    "ds1w control",
    "ds1w [-w|w0|w1|r|z]" },
#if USE_CMD_DS1W2
{   0, 0,  "ds1w2",  cmd_ds1w, 
    "ds1w control",
    "same as ds1w" },
#if USE_CMD_DS1W3
{   0, 0,  "ds1w3",  cmd_ds1w, 
    "ds1w control",
    "same as ds1w" },
#if USE_CMD_DS1W4
{   0, 0,  "ds1w4",  cmd_ds1w, 
    "ds1w control",
    "same as ds1w" },
#endif
#endif
#endif
#endif
#if USE_CMD_WS2812
{   0, 'W',  "ws2812",  cmd_ws2812, 
    "ws2812 led strap",
    "ws2812 -w <dat1> <dat2> ..."  },
#endif
#if USE_CMD_PWM
{   0, 0,  "pwm",  cmd_pwm, 
    "control pwm",
    "pwm -i <idx> -v <val>" },
#endif
#if USE_CMD_ADC
{   0, 0,  "adc",  cmd_adc, 
    "adc measure",
    "adc -i <idx>" },
#endif
#if USE_CMD_DAQ
{   0, 0,  "daq",  cmd_daq, 
    "data acquire",
    "daq -c <cmd> -i <idx> -v <val>" },
#endif
#if USE_CMD_RTC
{   0, 0,  "rtc",  cmd_rtc, 
    "rtc",
    "rtc [-s <val>]" },
#endif
#if USE_CMD_CAN
{   0, 'c',  "can",  cmd_can, 
    "can bus control",
    "can -c <cmd> -i <idx> -v <val>" },
#endif
#if USE_CMD_BEEP
{   0,  'b',  "beep",  cmd_beep, 
    "beep control",
    "beep [-f <freq>] [<ms>]"  },
#endif
#if USE_CMD_FCFS
{   CMD_HIDDEN, 0, "fcfs",  cmd_fcfs, 
    "fcfs control",
    "fcfs"  },
#endif
#if USE_CMD_UPGRADE
{   CMD_HIDDEN, 'U', "upgrade",  cmd_upgrade, 
    "upgrade firmware",
    "upgrade"  },
#endif
#if USE_CMD_SPIFFS
{   0, 's', "spiffs",  cmd_spiffs, 
    "spiffs control",
    "spiffs"  },
#endif
#if USE_CMD_FATFS
{   0, 'f', "fatfs",  cmd_fatfs,
    "fatfs control",
    "fatfs"  },
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
{   CMD_HIDDEN,  0,  "load",  cmd_load, 
    "load script",
    "load <pathname>" },
#endif
#if USE_CMD_STOP
{   CMD_HIDDEN,  0,  "stop",  cmd_stop, 
    "stop script",
    "stop" },
#endif
#if USE_CMD_CRC
{   0, 0, "crc",  cmd_crc, 
    "file crc check",
    "crc <pathname>"  },
#endif
#if USE_CMD_LOOP
{   0, 'L', "loop",  cmd_loop, 
    "run command looply",
    "loop <cmd and args>"  },
#endif
#if USE_CMD_TEST
{   0, 'T', "test",  cmd_test, 
    "condition test and run command",
    "test [-np] <cmd and args>"  },
#endif
{   CMD_END  } };



#if USE_CMD_HELP
int cmd_help( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'a', "all", 0, "show all" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', "check", shell_str_command, "check if command exists" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          's', "sname", shell_str_command, "check sname" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int show_hidden=0;
    char *check=0, *sname=0;
    uint8_t cmd_exists;
    char c;
 
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "all" ) == 0 )
                show_hidden = 1;
            else if( strcmp( opt.spec->name, "check" ) == 0 )
                check = (char*)opt.value;
            else if( strcmp( opt.spec->name, "sname" ) == 0 )
                sname = (char*)opt.value;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( check )
    {
        cmd_exists = shell_get_cmd_by_name( check ) ? 1 : 0;
        shell_write_line( cmd_exists ? shell_str_1 : shell_str_0 );
        return 0;
    }
    
    if( sname )
    {
        c = shell_get_short_name( sname );
        if( c == 0 )
            return 0;
        shell_write_char( c );
        shell_write_char( '\n' );
        return 0;
    }

    return shell_print_help(0, show_hidden);
}
#endif


#if USE_CMD_LOOP
int cmd_loop( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'l', shell_str_loop, "loop_delay_ms", "default 1000ms" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'n', shell_str_number, "cycle_limit", "cycle_limit" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_command, 0, "cmd with args" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    unsigned int loop=1, loop_delay=1000, loop_tick;
    char c;
    uint8_t cycle_set=0, cmd_set=0;
    int cycle_current=0, cycle_limit=-1;
    int (*cmd)(int argc, char *argv[]) = 0;
 
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_loop ) == 0 )
            {
                if( parse_int(opt.value, (int*)&loop_delay) == 0 )
                    return -1;
            }
            else if( STRCMP( opt.spec->name, shell_str_number ) == 0 )
            {
                if( parse_int(opt.value, (int*)&cycle_limit) == 0 )
                    return -1;
                if( cycle_limit <= 0 )
                    return -1;
                cycle_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_command ) == 0 )
            {
                cmd_set = 1;
                cmd = shell_get_cmd_by_name( argv[parser.idx] );
                argc -= parser.idx;
                argv += parser.idx;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    /* command not set, syntax error */ 
    if( ! cmd_set )
        return -1;
    /* command not found, exec error */ 
    if( cmd == 0 )
        return 1;
    /* cannot run "loop loop ...", syntax error */
    if( cmd == cmd_loop )
        return -1;

loop_start:
    if( cmd( argc, argv ) != 0 )
        return 1;

    if( cycle_set )
    {
        if( ++cycle_current >= cycle_limit )
            return 0;
    }
    
    LOOP_CHECK 
    return 0;
}
#endif


#if USE_CMD_DELAY
int cmd_delay( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_time, 0, "default 1000ms" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int ms=1000;
    os_tick_t t0=os_tick(), dt=0, total_ticks;
    char c;
    
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_time ) == 0 )
                if( parse_int(opt.value, (int*)&ms) == 0 )
                    return -1;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    total_ticks = OS_TICKS_MS(ms);
    while( shell_driver_read_char_blocked(&c, total_ticks - dt) != -1 )
    {
        if( c == 0x03 ) /* Ctrl-C for stop */
        {
            if( shell_is_script_mode() )
                shell_set_script( NULL, 0, 0 );  /* abort current script */
            return 0;
        }
        dt = os_tick() - t0;
    }
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


#if USE_CMD_TEST
#ifndef USE_CMD_TEST_ERRNO
    #define USE_CMD_TEST_ERRNO  1
    extern int get_errno(void);
#endif
int cmd_test( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'p', shell_str_pin, shell_str_pin, "port.bit high status" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'f', shell_str_file, shell_str_file, "file exist" },
#if USE_CMD_TEST_ERRNO
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'e', shell_str_error, shell_str_error, "errno match" },
#endif
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'n', shell_str_not, shell_str_not, shell_str_not },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_command, 0, "cmd with args" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int8_t bit_mode=-1;
    uint8_t cmd_set=0, not_set=0;
    int (*cmd)(int argc, char *argv[]) = 0;
    int port=-1, bit=-1;
#if USE_CMD_TEST_ERRNO
    uint8_t errno_set=0;
    int errno;
#endif
    const char *pport=0;
    char *pbit;
    int port_num = hal_gpio_get_port_num();
 
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_pin ) == 0 )
                pport = opt.value;
            else if( STRCMP( opt.spec->name, shell_str_not ) == 0 )
                not_set = 1;
#if USE_CMD_TEST_ERRNO
            else if( STRCMP( opt.spec->name, shell_str_error ) == 0 )
            {
                if( parse_int(opt.value, (int*)&errno) == 0 )
                    return -1;
                errno_set = 1;
            }
#endif
            else if( STRCMP( opt.spec->name, shell_str_command ) == 0 )
            {
                cmd_set = 1;
                cmd = shell_get_cmd_by_name( argv[parser.idx] );
                argc -= parser.idx;
                argv += parser.idx;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    /* command not set, syntax error */ 
    if( ! cmd_set )
        return -1;
    /* command not found, exec error */ 
    if( cmd == 0 )
        return 1;
 
    if( pport )
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

        /* check test condition */
        if( hal_gpio_get( port, 1<<bit ) == 0 )
        {
            if( not_set == 0 )
                return 0; /* unmatched, ignore command */
        }
        else
        {
            if( not_set )
                return 0; /* unmatched, ignore command */
        }
    }

#if USE_CMD_TEST_ERRNO
    if( errno_set )
    {
        if( get_errno() != errno )
        {
            if( not_set == 0 )
                return 0; /* unmatched, ignore command */
        }
        else
        {
            if( not_set )
                return 0; /* unmatched, ignore command */
        }
    }
#endif

    if( cmd( argc, argv ) != 0 )
        return 1;

    return 0;
port_error:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


