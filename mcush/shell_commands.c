/* commands */
#include "mcush.h"
   

int cmd_help( int argc, char *argv[] );
int cmd_scpi_idn( int argc, char *argv[] );
int cmd_scpi_rst( int argc, char *argv[] );
int cmd_reset( int argc, char *argv[] );
int cmd_gpio( int argc, char *argv[] );
int cmd_led( int argc, char *argv[] );
int cmd_dump( int argc, char *argv[] );
int cmd_write( int argc, char *argv[] );
int cmd_mfill( int argc, char *argv[] );





shell_cmd_t CMD_TAB[] = {
{   "?", "help",  cmd_help, 
    "display command list",
    "help"  },
{   "",  "*idn?", cmd_scpi_idn,
    "show device info",
    "*idn?" },
{   "",  "*rst",  cmd_scpi_rst, 
    "reset device",
    "*rst" },
{   "r",  "reset",  cmd_reset, 
    "reset CPU core",
    "reset" },
{   "p",  "gpio",  cmd_gpio, 
    "control gpio",
    "gpio -p <port.bit> -i <val> -o <val> -s <val> -c <val> -t <val>" },
{   "",  "led",  cmd_led, 
    "set led status",
    "led -s|c|t -i <idx>" },
{   "x",  "dump",  cmd_dump, 
    "dump memory",
    "dump -a <address> [-l <length>] [-w 1|2|4]" },
{   "w",  "write",  cmd_write, 
    "write memory",
    "write -a <address> [-w 1|2|4] dat1 dat2 ..." },
{   0,  "mfill",  cmd_mfill, 
    "fill memory",
    "mfill -b <address> [-l <length>] [-w 1|2|4] [-p <pattern>]" },
{   0,  0,  0,  0  } };



int cmd_help( int argc, char *argv[] )
{
    int i;
    if( argc > 1 )
    {
        shell_write_line("no arguments needed");
        return -1;
    }

    for( i=0; CMD_TAB[i].name; i++ )
    {
        shell_write_str( CMD_TAB[i].name );
        shell_write_str( "  " );
        shell_write_line( CMD_TAB[i].help );
        shell_write_str( "  " );
        shell_write_line( CMD_TAB[i].usage );
    }
    return 0;
}


int cmd_scpi_idn( int argc, char *argv[] )
{
    shell_write_str( "mcush" );
    shell_write_str( "," );
    shell_write_str( MCUSH_VERSION_STRING );
    shell_write_str( "\r\n" );

    return 0;
}


int cmd_scpi_rst( int argc, char *argv[] )
{
    return 0;
}

int cmd_reset( int argc, char *argv[] )
{
    portENTER_CRITICAL();
    hal_reset();
    while( 1 );
    //return 0;
}


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
    }; 

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
            if( shell_driver_read_char_blocked(&c, 1*configTICK_RATE_HZ/1000) != -1 )
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
    }; 

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


int cmd_dump( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "addr", 'b', "address", "base address", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "length", 'l', "length", "memory length", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_VALUE, "width", 'w', "width", "bus width 1|2|4, 1 for default", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    void *addr=(void*)-1;
    int length=16;
    int width=1;
    int count=0;
    int i;

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
            else if( strcmp( opt.spec->name, "length" ) == 0 )
                length = atoi(opt.value);
            else if( strcmp( opt.spec->name, "width" ) == 0 )
                width = atoi(opt.value);
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }; 

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

    while( count < length )
    {
        shell_printf( "%08X: ", (unsigned int)addr );
        switch(width)
        {
        case 1:
            for( i=0; i<16; i++ )
            {
                shell_printf( "%02X ", *(unsigned char*)addr );
                addr = (void*)(((int)addr) + 1 );
            }
            break;
        case 2:
            for( i=0; i<8; i++ )
            {
                shell_printf( "%04X ", *(unsigned short*)addr );
                addr = (void*)(((int)addr) + 2 );
            }
            break;
        case 4:
            for( i=0; i<4; i++ )
            {
                shell_printf( "%08X ", *(unsigned int*)addr );
                addr = (void*)(((int)addr) + 4 );
            }
            break;
        }
        count += 16;
        shell_write_str( "\r\n" );
    }
    return 0;
}


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
    }; 

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
    }; 

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

