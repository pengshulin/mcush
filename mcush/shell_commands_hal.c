/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


#if USE_CMD_REBOOT
int cmd_reboot( int argc, char *argv[] )
{
#ifdef HAL_REBOOT_COUNTER
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', shell_str_count, 0, "print counter" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', shell_str_reset, 0, "reset counter" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t count_flag=0, reset_flag=0;
        
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_count ) == 0 )
                count_flag = 1;
            if( STRCMP( opt.spec->name, shell_str_reset ) == 0 )
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
          'f', shell_str_file, "upgrade file", "binary file name" },
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
            if( STRCMP( opt.spec->name, shell_str_file ) == 0 )
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
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED, 
          'l', shell_str_loop, "loop_delay_ms", "default 1000ms" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'p', shell_str_port, "port_bit_name", "port[.bit] name, eg 0[.0]" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'i', shell_str_input, "input_mode", "set input mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'o', shell_str_output, "output_mode", "set output mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          's', shell_str_set, "set_high_val", "set output high mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          'c', shell_str_clr, "set_low_val", "set output low mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED,
          't', shell_str_toggle, "toggle_val", "toggle output mask" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'n', shell_str_number, 0, shell_str_query },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'U', shell_str_pullup, 0, "with pullup resister" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'D', shell_str_pulldown, 0, "with pulldown resister" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    unsigned int loop=0, loop_delay=1000, loop_tick;
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
            if( STRCMP( opt.spec->name, shell_str_port ) == 0 )
                pport = opt.value;
            else if( STRCMP( opt.spec->name, shell_str_input ) == 0 )
            {
                input_set = 1;
                pinput = opt.value;
            }
            else if( STRCMP( opt.spec->name, shell_str_output ) == 0 )
            {
                output_set = 1;
                poutput = opt.value;
            }
            else if( STRCMP( opt.spec->name, shell_str_set ) == 0 )
            {
                set_set = 1;
                pset = opt.value;
            }
            else if( STRCMP( opt.spec->name, shell_str_clr ) == 0 )
            {
                clr_set = 1;
                pclr = opt.value;
            }
            else if( STRCMP( opt.spec->name, shell_str_toggle ) == 0 )
            {
                toggle_set = 1;
                ptoggle = opt.value;
            }
            else if( STRCMP( opt.spec->name, shell_str_number ) == 0 )
            {
                shell_printf( "%d\n", port_num );
                return 0;
            }
            else if( STRCMP( opt.spec->name, shell_str_pullup ) == 0 )
                pull = 1;
            else if( STRCMP( opt.spec->name, shell_str_pulldown ) == 0 )
                pull = 0;
            else if( STRCMP( opt.spec->name, shell_str_loop ) == 0 )
            {
                loop=1;
                parse_int(opt.value, (int*)&loop_delay);
            }
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
        if( input_set && !parse_int(pinput, (int*)&input_val) )
            goto parm_error;
        if( output_set && !parse_int(poutput, (int*)&output_val) )
            goto parm_error;
        if( set_set && !parse_int(pset, (int*)&set_val) )
            goto parm_error;
        if( clr_set && !parse_int(pclr, (int*)&clr_val) )
            goto parm_error;
        if( toggle_set && !parse_int(ptoggle, (int*)&toggle_val) )
            goto parm_error;
    }

    if( !(input_set || output_set || set_set || clr_set || toggle_set) )
        none_set = 1;

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
 
    LOOP_CHECK 
 
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
          's', shell_str_set, 0, shell_str_on },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', shell_str_toggle, 0, shell_str_invert },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'c', shell_str_clr, 0, shell_str_off },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', shell_str_index, "led_index", shell_str_index_from_0 },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'n', shell_str_number, 0, shell_str_query },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'T', shell_str_test, 0, "blink all" },
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
            if( STRCMP( opt.spec->name, shell_str_clr ) == 0 )
                cmd = 0;
            else if( STRCMP( opt.spec->name, shell_str_set ) == 0 )
                cmd = 1;
            else if( STRCMP( opt.spec->name, shell_str_toggle ) == 0 )
                cmd = 2;
            else if( STRCMP( opt.spec->name, shell_str_index ) == 0 )
                parse_int(opt.value, (int*)&index);
            else if( STRCMP( opt.spec->name, shell_str_number ) == 0 )
            {
                shell_printf( "%d\n", led_num );
                return 0;
            }
            else if( STRCMP( opt.spec->name, shell_str_test ) == 0 )
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
 
    if( cmd == 0 )
        hal_led_clr( index );
    else if( cmd == 1 )
        hal_led_set( index );
    else if( cmd == 2 )
        hal_led_toggle( index );
    else
        shell_write_line( hal_led_get(index) ? "1" : "0" );
    return 0;
}
#endif


#if USE_CMD_WDG
int cmd_wdg( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, shell_str_command, 0, "enable|disable|clear|reset" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    const char *cmd=0;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_command ) == 0 )
                cmd = opt.value;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !cmd )
    {
        shell_write_line( hal_wdg_is_enable() ? shell_str_enable : shell_str_disable );
    }
    else if( strcmp( cmd, shell_str_enable ) == 0 )
    {
        hal_wdg_enable();
    }
    else if( strcmp( cmd, shell_str_disable ) == 0 )
    {
        hal_wdg_disable();
    }
    else if( strcmp( cmd, shell_str_clear ) == 0 )
    {
        hal_wdg_clear();
    }
    else if( strcmp( cmd, shell_str_reset ) == 0 )
    {
        portENTER_CRITICAL();
        while(1);
    }
    else
        return -1;
    
    return 0;
}
#endif


#if USE_CMD_BEEP
int cmd_beep( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', shell_str_frequency, shell_str_frequency, "20~10000(default 4000)hz" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, shell_str_ms, 0, "1~10000(default 50)ms" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int freq=-1, ms=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_frequency ) == 0 )
                parse_int(opt.value, (int*)&freq);
            else if( STRCMP( opt.spec->name, shell_str_ms ) == 0 )
                parse_int(opt.value, (int*)&ms);
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


#if USE_CMD_SGPIO
int cmd_sgpio( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
         'p', shell_str_port, "port_index", shell_str_index_from_0 },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'o', shell_str_output, "output_mode", "set output mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', shell_str_input, "input_mode", "set input mode mask" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "input_len", "input_buf_len", "length of input buffer" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', shell_str_frequency, shell_str_frequency, "1~1000000hz" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'l', shell_str_loop, 0, "loop mode" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', shell_str_start, 0, shell_str_run },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          's', shell_str_stop, 0, shell_str_stop }, 
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
           0, shell_str_info, 0, "print info" },
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
            if( STRCMP( opt.spec->name, shell_str_port ) == 0 )
                parse_int(opt.value, (int*)&port);
            else if( STRCMP( opt.spec->name, shell_str_frequency ) == 0 )
            {
                if( parse_float(opt.value, (float*)&freq_val) )
                    freq = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_loop ) == 0 )
                loop = 1;
            else if( STRCMP( opt.spec->name, shell_str_output ) == 0 )
                parse_int(opt.value, (int*)&output);
            else if( STRCMP( opt.spec->name, shell_str_input ) == 0 )
                parse_int(opt.value, (int*)&input);
            else if( STRCMP( opt.spec->name, shell_str_start ) == 0 )
                start = 1;
            else if( STRCMP( opt.spec->name, shell_str_stop ) == 0 )
                stop = 1;
            else if( STRCMP( opt.spec->name, shell_str_info ) == 0 )
                info = 1;
            else if( strcmp( opt.spec->name, "input_len" ) == 0 )
                parse_int(opt.value, (int*)&input_len);
 
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
            shell_printf( "%s  %s:%d  %s:0x%04X  %s:0x%04X  %s:%c  %s:%.1f\n", 
                cfg->run ? shell_str_run : shell_str_stop, 
                shell_str_port, cfg->port,
                shell_str_output, cfg->output_mode,
                shell_str_input, cfg->input_mode,
                shell_str_loop, cfg->loop_mode?'1':'0', 
                shell_str_frequency, cfg->freq );
            shell_printf( "buf_out:0x%08X  buf_in:0x%08X  %s:%d\n", 
                cfg->buf_out, cfg->buf_in, 
                shell_str_length, cfg->buf_len );
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
        shell_write_err( shell_str_port );
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
        buf_in = pvPortMalloc( buf_len * 2 );
    }

    return hal_sgpio_setup( loop, port, output, input, buf_out, buf_in, buf_len, freq_val ) ? 0 : 1;
}
#endif


#if USE_CMD_POWER
int cmd_power( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, shell_str_value, 0, "0|1|on|off" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int set=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
            {
                set=1;
                if( strcmp(opt.value, shell_str_0)==0 || strcmp(opt.value, shell_str_off)==0 )
                    hal_power_set( 0 );
                else if( strcmp(opt.value, shell_str_1)==0 || strcmp(opt.value, shell_str_on)==0 )
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
        shell_write_line( hal_is_power_set() ? shell_str_on : shell_str_off );
    } 
    return 0;
}
#endif


#if USE_CMD_PWM
int cmd_pwm( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', shell_str_index, "pwm_index", shell_str_index_from_0 },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'v', shell_str_value, shell_str_value, "value param" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'f', shell_str_frequency, shell_str_frequency, "1~100000(default 1000)hz" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'r', shell_str_range, shell_str_range, "default 100" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'n', shell_str_number, 0, shell_str_query },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'I', shell_str_init, 0, shell_str_init },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'D', shell_str_deinit, 0, shell_str_deinit },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0, freq_set=0, index_set=0, value_set=0, range_set=0;
    int pwm_num = hal_pwm_get_num();
    int freq, range;
    int index=-1, value=-1;
 
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( STRCMP( opt.spec->name, shell_str_frequency ) == 0 )
            {
                parse_int(opt.value, (int*)&freq);
                if( freq < 0 || freq > 100000 )
                {
                    shell_write_err( shell_str_range );
                    return -1;
                }
                freq_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_range ) == 0 )
            {
                parse_int(opt.value, (int*)&range);
                if( range < 2 || range > 10000 )
                {
                    shell_write_err( shell_str_range );
                    return -1;
                }
                range_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_number ) == 0 )
            {
                shell_printf( "%d\n", pwm_num );
                return 0;
            }
            else if( STRCMP( opt.spec->name, shell_str_index ) == 0 )
            {
                parse_int(opt.value, (int*)&index);
                index_set = 1;
                if( (index < 0) || (index > (pwm_num-1)) )
                {
                    shell_write_err( shell_str_index );
                    return -1;
                }
            }
            else if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
            {
                parse_int(opt.value, (int*)&value);
                value_set = 1;
                if( (value < 0) || (value > 10000) ) 
                {
                    shell_write_err( shell_str_value );
                    return -1;
                }
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init )
    {
        hal_pwm_init( freq_set ? freq : 0, range_set ? range : 0, value_set ? value : -1 );
        return 0;
    }
    else if( deinit )
    {
        hal_pwm_deinit();
        return 0;
    }

    if( ! index_set )
        index = 0;

    if( value_set )
        hal_pwm_set( index, value );
    else
    {
        shell_write_err( shell_str_value );
        return -1;
    }

    return 0;
}
#endif


#if USE_CMD_ADC
int cmd_adc( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'l', shell_str_loop, "loop_delay_ms", "default 1000ms" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'i', shell_str_index, "channel_index", "select channel" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'I', shell_str_init, 0, shell_str_init },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'D', shell_str_deinit, 0, shell_str_deinit },
        { MCUSH_OPT_NONE } };
    unsigned int loop=0, loop_delay=1000, loop_tick;
    uint8_t init=0, deinit=0, index_set=0;
    int adc_num = hal_adc_get_num();
    int index;
    char c;
    int i;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( STRCMP( opt.spec->name, shell_str_index ) == 0 )
            {
                if( opt.value )
                {
                    parse_int( opt.value, &index ); 
                    if( index < 0 || index >= adc_num )
                    {
                        shell_write_err( shell_str_index );
                        return -1;
                    }
                    index_set = 1;
                }
            }
            else if( STRCMP( opt.spec->name, shell_str_loop ) == 0 )
            {
                loop=1;
                parse_int(opt.value, (int*)&loop_delay);
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }; 

    if( init )
    {
        hal_adc_init();
        return 0;
    }
    else if( deinit )
    {
        hal_adc_deinit();
        return 0;
    }

loop_start:

    if( index_set )
    {
        shell_printf( "%.2f", hal_adc_get(index)+0.005 );
        shell_write_char( '\n' );
    } 
    else
    {
        for( i=0; i<adc_num; i++ )
        {
            shell_printf( "%.2f", hal_adc_get(i)+0.005 );
            shell_write_char( i < (adc_num-1) ? ',' : '\n' );
        }
    }

    LOOP_CHECK 
    return 0;
}
#endif

 
#if USE_CMD_RTC
int cmd_rtc( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          's', shell_str_set, 0, "set rtc" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_setting, 0, "format: YYYY-MM-DD HH:MM:SS" },
        { MCUSH_OPT_NONE } };
    int8_t set=-1;
    struct tm t;
     
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_set ) == 0 )
                set = 1;
            else if( STRCMP( opt.spec->name, shell_str_setting ) == 0 )
                break;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( set == 1 )
    {
        if( (parser.idx >= argc) || !parse_date(argv[parser.idx], &t.tm_year, &t.tm_mon, &t.tm_mday) )
        {
            shell_write_err( shell_str_data );
            return 1;
        }
        t.tm_wday = 0;
 
        parser.idx++;     
        if( (parser.idx >= argc) || !parse_time(argv[parser.idx], &t.tm_hour, &t.tm_min, &t.tm_sec) )
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


#if USE_CMD_CAN
#define DEFAULT_CAN_READ_TIMEOUT_MS  100
#define DEFAULT_CAN_READ_MAX_LINE    1000
#ifndef CAN_READ_MAX_LINE
    #if HAL_CAN_QUEUE_RX_LEN < DEFAULT_CAN_READ_MAX_LINE
        #define CAN_READ_MAX_LINE   DEFAULT_CAN_READ_MAX_LINE
    #else
        #define CAN_READ_MAX_LINE   HAL_CAN_QUEUE_RX_LEN 
    #endif
#endif

static void print_can_message( can_message_t *msg )
{
    shell_printf( msg->ext ? "%08X %c" : "%03X %c", msg->id, msg->remote ? 'R' : 'D' );
    if( msg->len )
    {
        shell_write_char( ' ' );
        shell_write_mem( msg->data, msg->len );
    }
    shell_write_char( '\n' );
}


int cmd_can( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', "cmd", shell_str_command, "info|(de)init|reset|baudrate|read|write|filter" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'i', "idx", shell_str_index, "index param" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'v', "val", shell_str_value, "value param" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'e', "ext", "extended", "extended" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'r', "rtr", "remote", "remote" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_data, 0, "data args" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    const char *cmd=0;
    int index, value;
    uint8_t index_set=0, value_set=0, ext_set=0, rtr_set=0;
    can_message_t msg;
    int arg_idx=-1;
    int dat;
    can_filter_t filter;
    int i, j, k, l;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "cmd" ) == 0 )
                cmd = opt.value;
            else if( strcmp( opt.spec->name, "idx" ) == 0 )
            {
                if( parse_int( opt.value, &index ) == 0 )
                {
                    shell_write_err(shell_str_index);
                    return 1;
                }
                index_set = 1;
            }
            else if( strcmp( opt.spec->name, "val" ) == 0 )
            {
                if( parse_int( opt.value, &value ) == 0 )
                {
                    shell_write_err(shell_str_value);
                    return 1;
                }
                value_set = 1;
            } 
            else if( strcmp( opt.spec->name, "ext" ) == 0 )
                ext_set = 1;
            else if( strcmp( opt.spec->name, "rtr" ) == 0 )
                rtr_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_data ) == 0 )
            {
                arg_idx = parser.idx;
                break;
            }
            else
                STOP_AT_INVALID_ARGUMENT 
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( cmd==NULL || strcmp(cmd, shell_str_info) == 0 )
    {
        shell_printf( "baudrate: %d\n", hal_can_get_baudrate() );
        k = hal_can_get_counter( (uint32_t*)&i, (uint32_t*)&j );
        shell_printf( "tx_count: %u\n", k ? (uint32_t)i : 0 );
        shell_printf( "rx_count: %u\n", k ? (uint32_t)j : 0 );
        shell_printf( "error: 0x%X\n", hal_can_get_error() );
    }
    else if( strcmp(cmd, "init") == 0 )
    {
        if( hal_can_init() == 0 )
            return 1;
    }
    else if( strcmp(cmd, "deinit") == 0 )
    {
        hal_can_deinit();
    }
    else if( strcmp(cmd, "reset") == 0 )
    {
        hal_can_reset();
    }
    else if( strcmp(cmd, "baudrate") == 0 )
    {
        if( !value_set )
            return 1;
        return hal_can_set_baudrate( value ) ? 0 : 1;
    }
    else if( strcmp(cmd, "filter") == 0 )
    {
        if( arg_idx > 0 )
        {
            /* set single filter as following arguments assigned */
            /* format: <filter_id> <enabled>  <can_id> <mask> */
            /* eg: can -c filter 5 0            (disable #5)   */
            /* eg: can -c filter 0 1 0x01 0x1F  (enable #0 as 0x1/0x1F) */
            /* eg: can -e -r -c filter 1 1 0x01 0x1F  (enable #0 as 0x1/0x1F ext/rtr set) */
            memset( (void*)&filter, 0, sizeof(can_filter_t) );
            if( arg_idx + 2 > argc )
                return 1;
            if( ! parse_int(argv[arg_idx++], &i) )
                return 1;
            if( ! parse_int(argv[arg_idx++], &j) )
                return 1;
            if( j )
            {
                if( arg_idx + 2 > argc )
                    return 1;
                if( ! parse_int(argv[arg_idx++], &k) )
                    return 1;
                if( ! parse_int(argv[arg_idx++], &l) )
                    return 1;
                if( ext_set )
                {
                    filter.ext = 1;
                    filter.ext_id = k;
                    filter.ext_id_mask = l;
                }
                else
                {
                    filter.std_id = k;
                    filter.std_id_mask = l;
                }
                filter.remote = rtr_set ? 1 : 0;
            }
            hal_can_filter_set( i, &filter, j );
        }
        else
        {
            /* no args assigned, list all filters */
            index = 0;
            while( 1 )
            {
                if( hal_can_filter_get( index, &filter, &i ) == 0 )
                    break;
                if( i )
                {
                    shell_printf( "#%d: ", index );
                    if( filter.ext )
                        shell_printf( "%08X(%08X)", filter.ext_id, filter.ext_id_mask );
                    else
                        shell_printf( "%03X(%03X)", filter.std_id, filter.std_id_mask );
                    if( filter.remote )
                        shell_write_str( " R" );
                    shell_write_char( '\n' );
                }
                index += 1;
            }
        }
    }
    else if( strcmp(cmd, "reset_input") == 0 )
    {
        while( hal_can_read( &msg, 0 ) )
        {
        } 
    }
    else if( strcmp(cmd, "read") == 0 )
    {
        /* timeout, may be modified by '-v value' */
        if( value_set )
            value = value*configTICK_RATE_HZ/1000;
        else
            value = DEFAULT_CAN_READ_TIMEOUT_MS*configTICK_RATE_HZ/1000;
        /* max number of output lines */
        if( index_set )
            i = index;
        else
            i = DEFAULT_CAN_READ_MAX_LINE;
        while( i && hal_can_read( &msg, value ) )
        {
            print_can_message( &msg );
            i--;
        }
    }
    else if( strcmp(cmd, "write") == 0 )
    {
        msg.id = index_set ? index : 0;
        msg.ext = ext_set;
        msg.remote = rtr_set;
        msg.len = 0;
        while( (arg_idx > 0) && (arg_idx < argc) && (msg.len < 8) )
        {
            if( ! parse_int(argv[arg_idx], &dat) )
            {
                shell_write_str( "data err: " );
                shell_write_line( argv[arg_idx] );
                return 1;
            }
            arg_idx++;
            msg.data[msg.len++] = dat;
        }
        if( hal_can_write( &msg ) == 0 )
            return 1;
    }
    else
    {
        shell_write_line("invalid cmd");
        return -1;
    } 
    return 0;
}
#endif


#if USE_CMD_WS2812
#ifndef HAL_WS2812_PORT
#define HAL_WS2812_PORT  0
#endif
#ifndef HAL_WS2812_PIN
#define HAL_WS2812_PIN  0
#endif
#define LENGTH_MAX  1000
static int ws2812_length;
static int ws2812_group_length;
static int *ws2812_buf;  /* memory needs: 4*length bytes */


int ws2812_init( int length, int group_length, int port, int pin )
{
    if( (length <= 0) || (length > LENGTH_MAX) || (group_length <= 0) )
        return 0;

    if( ws2812_buf )
    {
        vPortFree( ws2812_buf );
        ws2812_length = 0;
    }
    ws2812_buf = (int*)pvPortMalloc( 4*length ); 
    if( ws2812_buf == NULL )
    {
        return 0;
    }
    ws2812_length = length;
    ws2812_group_length = group_length;
    memset( ws2812_buf, 0, 4*length );
    if( hal_ws2812_init( port, pin ) )
        return 1;
    else
    {
        vPortFree( ws2812_buf );
        return 0;
    }
}


void ws2812_deinit(void)
{
    if( ws2812_buf )
    {
        vPortFree( ws2812_buf );
        ws2812_buf = 0;
        ws2812_length = 0;
        hal_ws2812_deinit();
    }
}


int ws2812_write(int offset, int dat, int swap_rg)
{
    if( ws2812_buf == NULL )
        return 0; 

    if( offset >= ws2812_length )
        return 0; 
    
    /* swtich color between GRB and RGB */
    if( ! swap_rg )
        dat = (dat&0xFF) + ((dat>>8)&0xFF00) + ((dat<<8)&0xFF0000);
    *(ws2812_buf + offset) = dat;
    return 1;
}


int ws2812_push(int forward, int dat, int swap_rg)
{
    int i;

    if( ws2812_buf == NULL )
        return 0;
    
    /* swtich color between GRB and RGB */
    if( ! swap_rg )
        dat = (dat&0xFF) + ((dat>>8)&0xFF00) + ((dat<<8)&0xFF0000);
    if( forward )
    {
        for( i=ws2812_length-1; i>=1; i-- )
            *(ws2812_buf + i) = *(ws2812_buf + i - 1);
        *ws2812_buf = dat;
    }
    else
    {
        for( i=0; i<ws2812_length-1; i++ )
            *(ws2812_buf + i) = *(ws2812_buf + i + 1);
        *(ws2812_buf + ws2812_length - 1) = dat;
    }
    return 1;
}


static void ws2812_flush_pixel( int dat )
{
    int i;

    portENTER_CRITICAL(); 
    for( i=0; i<24; i++ )
    {
        if( dat & 0x00800000 )
            hal_ws2812_write1();
        else
            hal_ws2812_write0();
        dat <<= 1;
    }
    portEXIT_CRITICAL();
}


void ws2812_flush(void)
{
    int i, j, *p;

    hal_ws2812_clr();
    hal_delay_us(60);
    for( i=0, p=ws2812_buf; i<ws2812_length; i++ )
    {
        for( j=0; j<ws2812_group_length; j++ )
            ws2812_flush_pixel( *p );
        p++;
    }
    hal_ws2812_clr();
}


int cmd_ws2812( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'l', shell_str_length, shell_str_length, "number of groups" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'G', shell_str_group, shell_str_group, "number of pixels per group" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'D', shell_str_deinit, 0, shell_str_deinit },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'w', shell_str_write, 0, shell_str_write },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'g', "grb", 0, "GRB instead of RGB" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'o', shell_str_offset, shell_str_offset, "data offset" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'p', shell_str_pin, shell_str_pin, "default 0.0" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', shell_str_init, 0, shell_str_init },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'f', "forward", 0, "push forward" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'b', "backward", 0, "push backward" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_data, 0, "data to be written" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init_set=0, deinit_set=0, offset_set=0, write_set=0, length_set=0, grb_set=0, group_set=0;
    uint8_t pushf_set=0, pushb_set=0;
    int port=HAL_WS2812_PORT, pin=HAL_WS2812_PIN;
    int length=0, offset=0, group=0;
    int dat;
    char *p2;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_write ) == 0 )
                write_set = 1;
            else if( strcmp( opt.spec->name, "grb" ) == 0 )
                grb_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_length ) == 0 )
            {
                if( parse_int(opt.value, &length) )
                    length_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_group ) == 0 )
            {
                if( parse_int(opt.value, &group) )
                    group_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_offset ) == 0 )
            {
                if( parse_int(opt.value, &offset) )
                    offset_set = 1;
            }
            else if( STRCMP( opt.spec->name, shell_str_data ) == 0 )
            {
                parser.idx--;
                break;
            }
            else if( STRCMP( opt.spec->name, shell_str_pin ) == 0 )
            {
                port = strtol( opt.value, &p2, 10 );
                if( !p2 || (*p2!='.') )
                    goto err_port;
                if( *(++p2) == 0 )
                    goto err_port;
                pin = strtol( p2, &p2, 10 );
                if( p2 && *p2 )
                    goto err_port;
            }
            else if( strcmp( opt.spec->name, "forward" ) == 0 )
                pushf_set = 1;
            else if( strcmp( opt.spec->name, "backward" ) == 0 )
                pushb_set = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init_set )
    {
        if( ! group_set ) 
            group = 1;
        if( ! length_set ) 
        {
            shell_write_err( shell_str_length );
            return -1;
        }
        if( ws2812_init( length, group, port, pin ) == 0 )
        {
            shell_write_err( shell_str_memory );
            return 1;
        }
        return 0;
    }
    
    if( deinit_set )
    {
        ws2812_deinit();
        return 0;
    } 

    /* update memory */
    parser.idx++;
    while( parser.idx < argc )
    {
        if( ! parse_int(argv[parser.idx], &dat) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            hal_ws2812_clr();
            return 1;
        }
        if( pushf_set )
            ws2812_push( 1, dat, grb_set );
        else if( pushb_set )
            ws2812_push( 0, dat, grb_set );
        else
            ws2812_write( offset++, dat, grb_set );
        parser.idx++;
    }

    /* write pixel by pixel */ 
    if( write_set )
    {
        ws2812_flush();
    } 

    return 0;
err_port:
    shell_write_err( shell_str_port_bit );
    return -1;
}
#endif
