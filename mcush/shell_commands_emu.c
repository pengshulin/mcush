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

#if USE_CMD_SPI4
static spi_cb_t *spi_cb[4];
#elif USE_CMD_SPI3
static spi_cb_t *spi_cb[3];
#elif USE_CMD_SPI2
static spi_cb_t *spi_cb[2];
#else
static spi_cb_t *spi_cb[1];
#endif


void emu_spi_deinit( int spi_index )
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( spi )
    {
        hal_gpio_set_input( spi->port_sdi, spi->pin_sdi_bit );
        hal_gpio_set_input( spi->port_sdo, spi->pin_sdo_bit );
        hal_gpio_set_input( spi->port_sck, spi->pin_sck_bit );
        hal_gpio_set_input( spi->port_cs, spi->pin_cs_bit );
        vPortFree( (void*)spi );
        spi_cb[spi_index] = 0;
    }
}


void emu_spi_init_structure( spi_cb_t *spi_init )
{
    spi_init->port_sdi = CMD_SPI_SDI_PORT;
    spi_init->port_sdo = CMD_SPI_SDO_PORT;
    spi_init->port_sck = CMD_SPI_SCK_PORT;
    spi_init->port_cs = CMD_SPI_CS_PORT;
    spi_init->pin_sdi = CMD_SPI_SDI_PIN;
    spi_init->pin_sdo = CMD_SPI_SDO_PIN;
    spi_init->pin_sck = CMD_SPI_SCK_PIN;
    spi_init->pin_cs = CMD_SPI_CS_PIN;
    spi_init->delay_us = 5;
    spi_init->width = 8;
    spi_init->cpol = 0;
    spi_init->cpha = 0;
    spi_init->lsb = 0;
}


int emu_spi_init( int spi_index, spi_cb_t *spi_init )
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( spi )
    {
        /* free previous spi pins (maybe different from the new pins),
           but do not free the memory */
        hal_gpio_set_input( spi->port_sdi, spi->pin_sdi_bit );
        hal_gpio_set_input( spi->port_sdo, spi->pin_sdo_bit );
        hal_gpio_set_input( spi->port_sck, spi->pin_sck_bit );
        hal_gpio_set_input( spi->port_cs, spi->pin_cs_bit );
    }
    else
    {
        spi = pvPortMalloc( sizeof(spi_cb_t) );
        if( spi == 0 )
            return 0;  /* fail */
        spi_cb[spi_index] = spi;
    }

    memcpy( spi, spi_init, sizeof(spi_cb_t) );
    spi->pin_sdi_bit = 1<<spi->pin_sdi;
    spi->pin_sdo_bit = 1<<spi->pin_sdo;
    spi->pin_sck_bit = 1<<spi->pin_sck;
    spi->pin_cs_bit = 1<<spi->pin_cs;

    hal_gpio_set_input( spi->port_sdi, spi->pin_sdi_bit );
    hal_gpio_set_output( spi->port_sdo, spi->pin_sdo_bit );
    hal_gpio_set_output( spi->port_sck, spi->pin_sck_bit );
    hal_gpio_set_output( spi->port_cs, spi->pin_cs_bit );
    hal_gpio_set( spi->port_cs, spi->pin_cs_bit );

    return 1;
}


static uint32_t emu_spi_write_single(spi_cb_t *spi, uint32_t dat)
{
    uint32_t ret=0;
    int i;
    int polar=spi->cpha^spi->cpol;

    for( i=0; i<spi->width; i++ )
    {
        /* sdo output */
        if( dat & 1<<(spi->lsb ? i : (spi->width-1-i)) )
            hal_gpio_set( spi->port_sdo, spi->pin_sdo_bit );
        else
            hal_gpio_clr( spi->port_sdo, spi->pin_sdo_bit );
        /* sck */
        if( polar )
            hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
        else
            hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
        /* delay 1 */
        hal_delay_us( spi->delay_us );
        /* sck */
        if( polar )
            hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
        else
            hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
        /* sdo */
        if( dat & 1<<(spi->lsb ? i : (spi->width-1-i)))
            hal_gpio_set( spi->port_sdo, spi->pin_sdo_bit );
        else
            hal_gpio_clr( spi->port_sdo, spi->pin_sdo_bit );
        /* sdi */
        if( hal_gpio_get( spi->port_sdi, spi->pin_sdi_bit ) )
            ret |= 1<<(spi->lsb ? i : (spi->width-1-i));
        /* delay 2 */
        hal_delay_us( spi->delay_us );
    }
    return ret;
}


int emu_spi_write(int spi_index, uint32_t *buf_out, uint32_t *buf_in, int bytes)
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( spi==0 )
        return 0;

    /* prepare and start CS */
    if( spi->cpol )
        hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
    else
        hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
    hal_gpio_clr( spi->port_cs, spi->pin_cs_bit );

    /* write and read */
    while( bytes-- )
    {
        *buf_in = emu_spi_write_single( spi, *buf_out );
        buf_in++;
        buf_out++;
    }

    /* end CS */
    hal_gpio_set( spi->port_cs, spi->pin_cs_bit );
        
    return 1;
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
    uint8_t init=0, deinit=0, read_mode=0;
    char *p;
    int spi_idx;
    spi_cb_t spi_init;
    //uint8_t spi_port_sdi=CMD_SPI_SDI_PORT, spi_port_sdo=CMD_SPI_SDO_PORT, spi_port_sck=CMD_SPI_SCK_PORT, spi_port_cs=CMD_SPI_CS_PORT;
    //uint16_t spi_pin_sdi=CMD_SPI_SDI_PIN, spi_pin_sdo=CMD_SPI_SDO_PIN, spi_pin_sck=CMD_SPI_SCK_PIN, spi_pin_cs=CMD_SPI_CS_PIN;
    //uint32_t spi_delay_us=5;
    //uint32_t spi_width=8;
    //uint8_t spi_cpol=0, spi_cpha=0, spi_lsb=0;
    uint32_t buf_out[256], buf_in[256];
    int bytes, i, line_count;

    /* check which command spi/spi2/spi3/spi4 is used */
    if( parse_int(argv[0]+3, (int*)&spi_idx) )
        spi_idx -= 1;
    else
        spi_idx = 0;
    emu_spi_init_structure( &spi_init );
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
                parse_int(opt.value, (int*)&spi_init.delay_us);
            else if( STRCMP( opt.spec->name, shell_str_read ) == 0 )
                read_mode = 1;
            else if( STRCMP( opt.spec->name, shell_str_width ) == 0 )
            {
                if( parse_int(opt.value, (int*)&spi_init.width) )
                {
                    if( (spi_init.width < 2) || (spi_init.width > 32) )
                    {
                        shell_write_err( shell_str_width );
                        return 1;
                    }
                }
            }
            else if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "cpol" ) == 0 )
                spi_init.cpol = 1;
            else if( strcmp( opt.spec->name, "cpha" ) == 0 )
                spi_init.cpha = 1;
            else if( strcmp( opt.spec->name, "lsb" ) == 0 )
                spi_init.lsb = 1;
            else if( strcmp( opt.spec->name, "sdi" ) == 0 )
            {
                spi_init.port_sdi = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_init.pin_sdi = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( strcmp( opt.spec->name, "sdo" ) == 0 )
            {
                spi_init.port_sdo = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_init.pin_sdo = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( strcmp( opt.spec->name, "sck" ) == 0 )
            {
                spi_init.port_sck = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_init.pin_sck = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( strcmp( opt.spec->name, "cs" ) == 0 )
            {
                spi_init.port_cs = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                spi_init.pin_cs = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
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
        return emu_spi_init( spi_idx, &spi_init ) ? 0 : 1;
    }
    else if( deinit )
    {
        emu_spi_deinit( spi_idx );
        return 0;
    }

    /* parse data params */
    parser.idx++;
    bytes = 0;
    while( parser.idx < argc )
    {
        if( ! parse_int(argv[parser.idx], (int*)&buf_out[bytes]) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            return 1;
        }
        parser.idx++;
        bytes += 1;
    }
    if( bytes == 0 )
        return -1;  /* no parms, nothing to do */

    /* write and read */
    if( !emu_spi_write( spi_idx, buf_out, buf_in, bytes ) )
        return 1;  /* execute failed, maybe not inited */

    /* output read */
    if( read_mode )
    {
        line_count = 0;
        for( i=0; i<bytes; i++ )
        {
            line_count += shell_printf( "0x%X ", buf_in[i] );
            if( line_count > 78 )
            {
                shell_write_char( '\n' );
                line_count = 0;
            }
        }
        if( line_count )
            shell_write_char( '\n' );
    }

    return 0;
 
err_port:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


