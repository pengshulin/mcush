/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


extern int cmd_help( int argc, char *argv[] );
extern int cmd_scpi_idn( int argc, char *argv[] );
extern int cmd_scpi_rst( int argc, char *argv[] );
extern int cmd_reboot( int argc, char *argv[] );
extern int cmd_upgrade( int argc, char *argv[] );
extern int cmd_gpio( int argc, char *argv[] );
extern int cmd_led( int argc, char *argv[] );
extern int cmd_dump( int argc, char *argv[] );
extern int cmd_write( int argc, char *argv[] );
extern int cmd_mfill( int argc, char *argv[] );
extern int cmd_wait( int argc, char *argv[] );
extern int cmd_echo( int argc, char *argv[] );
extern int cmd_wdg( int argc, char *argv[] );
extern int cmd_uptime( int argc, char *argv[] );
extern int cmd_system( int argc, char *argv[] );
extern int cmd_mapi( int argc, char *argv[] );
extern int cmd_beep( int argc, char *argv[] );
extern int cmd_sgpio( int argc, char *argv[] );
extern int cmd_mkbuf( int argc, char *argv[] );
extern int cmd_power( int argc, char *argv[] );
extern int cmd_i2c( int argc, char *argv[] );
extern int cmd_spi( int argc, char *argv[] );
extern int cmd_pwm( int argc, char *argv[] );
extern int cmd_adc( int argc, char *argv[] );
extern int cmd_counter( int argc, char *argv[] );
extern int cmd_rtc( int argc, char *argv[] );
extern int cmd_spiffs( int argc, char *argv[] );
extern int cmd_fatfs( int argc, char *argv[] );
extern int cmd_cat( int argc, char *argv[] );
extern int cmd_rm( int argc, char *argv[] );
extern int cmd_rename( int argc, char *argv[] );
extern int cmd_copy( int argc, char *argv[] );
extern int cmd_list( int argc, char *argv[] );
extern int cmd_load( int argc, char *argv[] );
extern int cmd_crc( int argc, char *argv[] );
extern int cmd_loop( int argc, char *argv[] );



const shell_cmd_t CMD_TAB[] = {
#if USE_CMD_HELP
{   0,  '?',  shell_str_help,  cmd_help, 
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
{   CMD_HIDDEN, 0,  shell_str_reboot,  cmd_reboot, 
    "reboot device",
    shell_str_reboot },
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
    "spi [-I|r] <data>" },
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
{   0,  0,  "load",  cmd_load, 
    "load script",
    "load <pathname>" },
#endif
#if USE_CMD_CRC
{   0, 0, "crc",  cmd_crc, 
    "file crc check",
    "crc <pathname>"  },
#endif
#if USE_CMD_LOOP
{   0, 0, "loop",  cmd_loop, 
    "run command looply",
    "loop <cmd and args>"  },
#endif
{   CMD_END  } };



#if USE_CMD_HELP
int cmd_help( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'a', "all", 0, "show all" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', "check", shell_str_command, "check if command exists" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int show_hidden=0;
    char *check=0;
    uint8_t cmd_exists;
 
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "all" ) == 0 )
                show_hidden = 1;
            else if( strcmp( opt.spec->name, "check" ) == 0 )
                check = (char*)opt.value;
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

    return shell_print_help(0, show_hidden);
}
#endif


#if USE_CMD_LOOP
int cmd_loop( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
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
    uint8_t cycle_set=0;
    int cycle_current, cycle_limit=-1;
    int (*cmd)(int argc, char *argv[]) = 0;
 
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_loop ) == 0 )
            {
                parse_int(opt.value, (int*)&loop_delay);
            }
            else if( STRCMP( opt.spec->name, shell_str_number ) == 0 )
            {
                parse_int(opt.value, (int*)&cycle_limit);
                if( cycle_limit > 0 )
                {
                    cycle_current = 0;
                    cycle_set = 1;
                }
                else
                    return -1;
            }
            else if( STRCMP( opt.spec->name, shell_str_command ) == 0 )
            {
                cmd = shell_get_cmd_by_name( argv[parser.idx] );
                argc -= parser.idx;
                argv += parser.idx;
                break;
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    /* command not set/found */ 
    if( cmd == 0 )
        return 1;

    /* cannot run "loop loop ..." */
    if( cmd == cmd_loop )
        return 1;

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


#if USE_CMD_WAIT
int cmd_wait( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_time, 0, "default 1000ms" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int ms=1000;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_time ) == 0 )
                parse_int(opt.value, (int*)&ms);
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


