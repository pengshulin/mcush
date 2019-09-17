/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


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
static uint32_t i2c_addr=0, i2c_addr_def=0;
static uint32_t i2c_delay_us=5;
static uint8_t i2c_lsb_mode=0;

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
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    hal_delay_us( i2c_delay_us );
    hal_gpio_set( i2c_port_scl, i2c_pin_scl );
    ret = hal_gpio_get( i2c_port_sda, i2c_pin_sda );
    hal_delay_us( i2c_delay_us );
    hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
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
    uint8_t mask=i2c_lsb_mode ? 0x01 : 0x80;
    uint8_t ret=0;
    int i;
    hal_gpio_set( i2c_port_sda, i2c_pin_sda );
    for( i=0; i<8; i++ )
    {     
        hal_gpio_set( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
        if( hal_gpio_get( i2c_port_sda, i2c_pin_sda ) )
            ret |= mask;
        hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
        if( i2c_lsb_mode )
            mask <<= 1;
        else
            mask >>= 1;
    } 
    i2c_write_ack( ack );
    return ret;
}

static int i2c_write_byte(uint8_t chr)
{
    uint8_t mask=i2c_lsb_mode ? 0x01 : 0x80;
    int i;
    for( i=0; i<8; i++ )
    {
        if( chr & mask )
            hal_gpio_set( i2c_port_sda, i2c_pin_sda );
        else
            hal_gpio_clr( i2c_port_sda, i2c_pin_sda );
        hal_delay_us( i2c_delay_us );
        hal_gpio_set( i2c_port_scl, i2c_pin_scl );
        hal_delay_us( i2c_delay_us );
        hal_gpio_clr( i2c_port_scl, i2c_pin_scl );
        if( i2c_lsb_mode )
            mask <<= 1;
        else
            mask >>= 1;
    }
    return i2c_read_ack(); 
}


int cmd_i2c( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, shell_str_delay, "delay_us", "default 5" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'a', shell_str_address, shell_str_address, "default 0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sda", "sda_pin", "default 0.0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "scl", "scl_pin", "default 0.1" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', shell_str_init, 0, "init pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'D', shell_str_deinit, 0, "deinit pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'l', "lsb", 0, "lsb first" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'n', "nostop", 0, "no stop bit" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'r', shell_str_read, "read_cycle", "default 0" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_value, 0, shell_str_data },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0, addr_set=0, nostop_set=0, lsb_set=0;
    int read_cycle=0;
    char *p;
    int i, dat, line_count;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
            {
                if( parse_int(opt.value, (int*)&i2c_addr) )
                    addr_set = 1;
                else
                    goto err_addr;
            }
            else if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
                parse_int(opt.value, (int*)&i2c_delay_us);
            else if( STRCMP( opt.spec->name, shell_str_read ) == 0 )
                parse_int(opt.value, (int*)&read_cycle);
            else if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "nostop" ) == 0 )
                nostop_set = 1;
            else if( strcmp( opt.spec->name, "lsb" ) == 0 )
                lsb_set = 1;
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
            else if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
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
        if( ! addr_set )
            goto err_addr;
        i2c_lsb_mode = lsb_set;
        i2c_addr_def = i2c_addr;
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
  
    /* send address */ 
    if( !addr_set )
        i2c_addr = i2c_addr_def; 
    i2c_start();
    if( ! i2c_write_byte( addr_set ? i2c_addr : (i2c_addr<<1) ) )
    {
        i2c_stop();
        shell_write_err("ACK");
        return 1;
    }
    /* write data */ 
    parser.idx++;
    while( parser.idx < argc )
    {
        if( ! parse_int(argv[parser.idx], &dat) )
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
        i2c_write_byte( addr_set ? i2c_addr : ((i2c_addr<<1)|1) );
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
    if( ! nostop_set )
        i2c_stop();
    return 0;
 
err_addr:
    shell_write_err( shell_str_address );
    return -1;
err_port:
    shell_write_err( shell_str_port_bit );
    return -1;
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
          0, shell_str_delay, "delay_us", "default 5" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sdi", "sdi_pin", "default 0.0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sdo", "sdo_pin", "default 0.1" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "sck", "sck_pin", "default 0.2" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, "cs", "cs_pin", "default 0.3" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', shell_str_init, 0, "init pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'D', shell_str_deinit, 0, "deinit pins" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', shell_str_read, 0, "print readout" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "cpol", 0, "clk polarity" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "cpha", 0, "clk phase" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          0, "lsb", 0, "lsb first" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_value, 0, shell_str_data },
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
            if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
                parse_int(opt.value, (int*)&spi_delay_us);
            else if( STRCMP( opt.spec->name, shell_str_read ) == 0 )
                read_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_width ) == 0 )
                parse_int(opt.value, (int*)&spi_width);
            else if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
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
            else if( STRCMP( opt.spec->name, shell_str_value ) == 0 )
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
        if( ! parse_int(argv[parser.idx], &dat_out) )
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


