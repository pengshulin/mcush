/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"

/*************************************************************************/
/* Multiple emulated I2C controller                                      */
/*************************************************************************/

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

#if USE_CMD_I2C4
static i2c_cb_t *i2c_cb[4];
#elif USE_CMD_I2C3
static i2c_cb_t *i2c_cb[3];
#elif USE_CMD_I2C2
static i2c_cb_t *i2c_cb[2];
#else
static i2c_cb_t *i2c_cb[1];
#endif

/* NOTE: larger r/w buffers in task stack may overflow and cause unpredictable behavior */
#ifndef I2C_RW_BUF_LEN
    #define I2C_RW_BUF_LEN  256  /* 256*2*1=512 stack size */
    //#define I2C_RW_BUF_LEN  128  /* 128*2*1=256 stack size */
#endif


void emu_i2c_init_structure(i2c_cb_t *i2c_init)
{
    i2c_init->port_scl = CMD_I2C_SCL_PORT;
    i2c_init->port_sda = CMD_I2C_SDA_PORT;
    i2c_init->pin_scl = CMD_I2C_SCL_PIN;
    i2c_init->pin_sda = CMD_I2C_SDA_PIN;
    i2c_init->delay_us = 5;
    i2c_init->addr = 0;
    i2c_init->lsb = 0;
}


static void i2c_start(i2c_cb_t *i2c)
{
    hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_clr( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
}


static void i2c_stop(i2c_cb_t *i2c)
{
    hal_gpio_clr( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
}


static int i2c_read_ack(i2c_cb_t *i2c)
{
    int ret;

    hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
    ret = hal_gpio_get( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
    return ret ? 0 : 1;
}


static void i2c_write_ack(i2c_cb_t *i2c, int ack)
{
    if( ack )
        hal_gpio_clr( i2c->port_sda, i2c->pin_sda_bit );
    else
        hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
    hal_delay_us( i2c->delay_us );
    hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
}


static uint8_t i2c_read_byte(i2c_cb_t *i2c, int ack)
{
    uint8_t mask=i2c->lsb ? 0x01 : 0x80;
    uint8_t ret=0;
    int i;

    hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
    for( i=0; i<8; i++ )
    {     
        hal_delay_us( i2c->delay_us );
        hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
        if( hal_gpio_get( i2c->port_sda, i2c->pin_sda_bit ) )
            ret |= mask;
        hal_delay_us( i2c->delay_us );
        hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
        if( i2c->lsb )
            mask <<= 1;
        else
            mask >>= 1;
    } 
    i2c_write_ack( i2c, ack );
    return ret;
}


static int i2c_write_byte(i2c_cb_t *i2c, uint8_t chr)
{
    uint8_t mask=i2c->lsb ? 0x01 : 0x80;
    int i;

    for( i=0; i<8; i++ )
    {
        if( chr & mask )
            hal_gpio_set( i2c->port_sda, i2c->pin_sda_bit );
        else
            hal_gpio_clr( i2c->port_sda, i2c->pin_sda_bit );
        hal_delay_us( i2c->delay_us );
        hal_gpio_set( i2c->port_scl, i2c->pin_scl_bit );
        hal_delay_us( i2c->delay_us );
        hal_gpio_clr( i2c->port_scl, i2c->pin_scl_bit );
        if( i2c->lsb )
            mask <<= 1;
        else
            mask >>= 1;
    }
    return i2c_read_ack(i2c); 
}


void emu_i2c_deinit(int i2c_index)
{
    i2c_cb_t *i2c=i2c_cb[i2c_index];

    if( i2c==0 )
        return;
    hal_gpio_set_input( i2c->port_scl, i2c->pin_scl_bit );
    hal_gpio_set_input( i2c->port_sda, i2c->pin_sda_bit );
    vPortFree( (void*)i2c );
    i2c_cb[i2c_index] = 0;
}


int emu_i2c_init(int i2c_index, i2c_cb_t *i2c_init)
{
    i2c_cb_t *i2c=i2c_cb[i2c_index];
    
    if( i2c )
    {
        /* free previous i2c pins (maybe different from the new pins),
           but do not free the memory */
        hal_gpio_set_input( i2c->port_scl, i2c->pin_scl_bit );
        hal_gpio_set_input( i2c->port_sda, i2c->pin_sda_bit );
    }
    else
    {
        i2c = pvPortMalloc( sizeof(i2c_cb_t) );
        if( i2c == 0 )
            return 0;  /* fail */
        i2c_cb[i2c_index] = i2c;
    }
  
    memcpy( i2c, i2c_init, sizeof(i2c_cb_t) );
    i2c->pin_scl_bit = 1<<i2c->pin_scl;
    i2c->pin_sda_bit = 1<<i2c->pin_sda;
 
    hal_gpio_set_output( i2c->port_scl, i2c->pin_scl_bit );
    hal_gpio_set_output_open_drain( i2c->port_sda, i2c->pin_sda_bit );
    i2c_stop(i2c);
    
    return 1;
}


int emu_i2c_write(int i2c_index, uint8_t *buf_out, uint8_t *buf_in, int write_bytes, int read_bytes, int address, int no_stop )
{
    i2c_cb_t *i2c=i2c_cb[i2c_index];
    int i; 

    if( i2c==0 )
        return 0;

    /* no write/read request, test address response only */
    if( (write_bytes==0) && (write_bytes==0) )
    {
        i2c_start(i2c);
        if( ! i2c_write_byte( i2c, address ? address : (i2c->addr<<1) ) )
            goto err_ack;
        i2c_stop(i2c);
        return 1;
    }

    /* write first */ 
    if( write_bytes )
    {
        i2c_start(i2c);
        if( ! i2c_write_byte( i2c, address ? address : (i2c->addr<<1) ) )
            goto err_ack;
        /* write data */ 
        for( i=0; i<write_bytes; i++ )
        {
            i2c_write_byte(i2c, buf_out[i]);
        }
    }

    /* read back */
    if( read_bytes )
    {
        i2c_start(i2c);
        if( ! i2c_write_byte(i2c, address ? address : ((i2c->addr<<1)|1) ) )
            goto err_ack;
        /* read bytes */
        for( i=0; i<read_bytes; i++ )
        {
            buf_in[i] = i2c_read_byte( i2c, i<read_bytes-1 ? 1 : 0 ); 
        }
    }
    if( ! no_stop )
        i2c_stop(i2c);
    return 1;

err_ack:
    i2c_stop(i2c);
    return -1;
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
    uint8_t init=0, deinit=0, no_stop=0, addr_set=0;
    char *p;
    int i2c_index=0;
    i2c_cb_t i2c_init;
    int i, line_count;
    uint8_t buf_out[I2C_RW_BUF_LEN], buf_in[I2C_RW_BUF_LEN];
    int read_bytes=0, write_bytes;

    /* check which command i2c/i2c2/i2c3/i2c4 is used */
#if USE_CMD_I2C2
    if( parse_int(argv[0]+3, (int*)&i2c_index) )
        i2c_index -= 1;
#endif
    emu_i2c_init_structure( &i2c_init );
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_address ) == 0 )
            {
                if( parse_int(opt.value, (int*)&i2c_init.addr) )
                    addr_set = 1;
                else
                    goto err_addr;
            }
            else if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
                parse_int(opt.value, (int*)&i2c_init.delay_us);
            else if( STRCMP( opt.spec->name, shell_str_read ) == 0 )
                parse_int(opt.value, (int*)&read_bytes);
            else if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( strcmp( opt.spec->name, "nostop" ) == 0 )
                no_stop = 1;
            else if( strcmp( opt.spec->name, "lsb" ) == 0 )
                i2c_init.lsb = 1;
            else if( strcmp( opt.spec->name, "scl" ) == 0 )
            {
                i2c_init.port_scl = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                i2c_init.pin_scl = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( strcmp( opt.spec->name, "sda" ) == 0 )
            {
                i2c_init.port_sda = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                i2c_init.pin_sda = strtol( p, &p, 10 );
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
        return emu_i2c_init( i2c_index, &i2c_init ) ? 0 : 1;
    }
    else if( deinit )
    {
        emu_i2c_deinit( i2c_index );
        return 0;
    }
  
    /* parse data params */
    parser.idx++;
    write_bytes = 0;
    while( parser.idx < argc )
    {
        if( ! parse_int(argv[parser.idx], (int*)&i) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            return 1;
        }
        buf_out[write_bytes] = i;
        parser.idx++;
        write_bytes += 1;
    }
    
    i = emu_i2c_write( i2c_index, buf_out, buf_in, write_bytes, read_bytes, addr_set ? i2c_init.addr : 0, no_stop );
    if( i == 0 )
        return 1;
    if( i == -1 )
    {
        shell_write_err("ACK");
        return 2;
    }

    /* print read data */ 
    if( read_bytes )
    {
        line_count = 0;
        for( i=0; i<read_bytes; i++ )
        {
            line_count += shell_printf( "0x%02X ", buf_in[i] );
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
 
err_addr:
    shell_write_err( shell_str_address );
    return -1;
err_port:
    shell_write_err( shell_str_port_bit );
    return -1;
}
#endif


/*************************************************************************/
/* Multiple emulated SPI controller                                      */
/*************************************************************************/

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

#ifndef SPI_RW_BUF_LEN
    //#define SPI_RW_BUF_LEN  256  /* 256*2*4=2K stack size */
    #define SPI_RW_BUF_LEN  128  /* 128*2*4=1K stack size */
    //#define SPI_RW_BUF_LEN  64  /* 64*2*4=512 stack size */
#endif


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


void emu_spi_deinit( int spi_index )
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( spi==0 )
        return;
    hal_gpio_set_input( spi->port_sdi, spi->pin_sdi_bit );
    hal_gpio_set_input( spi->port_sdo, spi->pin_sdo_bit );
    hal_gpio_set_input( spi->port_sck, spi->pin_sck_bit );
    hal_gpio_set_input( spi->port_cs, spi->pin_cs_bit );
    vPortFree( (void*)spi );
    spi_cb[spi_index] = 0;
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
    if( spi->cpol )
        hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
    else
        hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
 
    return 1;
}


int emu_spi_update( int spi_index, spi_cb_t *update )
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( !spi )
        return 0;

    update->pin_sdi_bit = 1<<update->pin_sdi;
    update->pin_sdo_bit = 1<<update->pin_sdo;
    update->pin_sck_bit = 1<<update->pin_sck;
    update->pin_cs_bit  = 1<<update->pin_cs;

    if( (update->port_sdi != spi->port_sdi) || (update->pin_sdi != spi->pin_sdi) )
    {
        hal_gpio_set_input( update->port_sdi, update->pin_sdi_bit );
    }
     
    if( (update->port_sdo != spi->port_sdo) || (update->pin_sdo != spi->pin_sdo) )
    {
        hal_gpio_set_input( spi->port_sdo, spi->pin_sdo_bit );
        hal_gpio_set_output( update->port_sdo, update->pin_sdo_bit );
    }

    if( (update->port_sck != spi->port_sck) || (update->pin_sck != spi->pin_sck) )
    { 
        hal_gpio_set_input( spi->port_sck, spi->pin_sck_bit );
        hal_gpio_set_output( update->port_sck, update->pin_sck_bit );
        if( update->cpol )
            hal_gpio_set( update->port_sck, update->pin_sck_bit );
        else
            hal_gpio_clr( update->port_sck, update->pin_sck_bit );
    }

    if( (update->port_cs != spi->port_cs) || (update->pin_cs != spi->pin_cs) )
    { 
        hal_gpio_set_input( spi->port_cs, spi->pin_cs_bit );
        hal_gpio_set_output( update->port_cs, update->pin_cs_bit );
        hal_gpio_set( update->port_cs, update->pin_cs_bit );
    }
    
    memcpy( spi, update, sizeof(spi_cb_t) );
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
        /* sdi */
        if( hal_gpio_get( spi->port_sdi, spi->pin_sdi_bit ) )
            ret |= 1<<(spi->lsb ? i : (spi->width-1-i));
        /* delay 2 */
        hal_delay_us( spi->delay_us );
    }
    return ret;
}


int emu_spi_write(int spi_index, uint32_t *buf_out, uint32_t *buf_in, int length)
{
    spi_cb_t *spi=spi_cb[spi_index];

    if( spi==0 )
        return 0;

    /* prepare clock and start CS */
    if( spi->cpol )
        hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
    else
        hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
    hal_gpio_clr( spi->port_cs, spi->pin_cs_bit );
    hal_delay_us( spi->delay_us );

    /* write and read */
    while( length-- )
    {
        *buf_in = emu_spi_write_single( spi, *buf_out );
        buf_in++;
        buf_out++;
    }

    /* end clock and CS */
    if( spi->cpol )
        hal_gpio_set( spi->port_sck, spi->pin_sck_bit );
    else
        hal_gpio_clr( spi->port_sck, spi->pin_sck_bit );
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
          'U', shell_str_update, 0, "update" },
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
    uint8_t init=0, deinit=0, read_mode=0, update=0;
    char *p;
    int spi_index=0;
    spi_cb_t spi_init;
    uint32_t buf_out[SPI_RW_BUF_LEN], buf_in[SPI_RW_BUF_LEN];
    int length, i, line_count, repeat;

    /* check which command spi/spi2/spi3/spi4 is used */
#if USE_CMD_SPI2
    if( parse_int(argv[0]+3, (int*)&spi_index) )
        spi_index -= 1;
#endif
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
                    if( (spi_init.width < 1) || (spi_init.width > 32) )
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
            else if( STRCMP( opt.spec->name, shell_str_update ) == 0 )
                update = 1;
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
        return emu_spi_init( spi_index, &spi_init ) ? 0 : 1;
    }
    else if( update )
    {
        return emu_spi_update( spi_index, &spi_init ) ? 0 : 1;
    }
    else if( deinit )
    {
        emu_spi_deinit( spi_index );
        return 0;
    }

    /* parse data params */
    parser.idx++;
    length = 0;
    while( parser.idx < argc )
    {
        //if( ! parse_int(argv[parser.idx], (int*)&buf_out[length]) )
        if( ! parse_int_repeat(argv[parser.idx], (int*)&buf_out[length], &repeat ) )
        {
            shell_write_str( "data err: " );
            shell_write_line( argv[parser.idx] );
            return 1;
        }
        parser.idx++;
        length += 1;
        while( repeat )
        {
            buf_out[length] = buf_out[length-1];
            length += 1;
            repeat -= 1;
            if( length > SPI_RW_BUF_LEN )
            {
                shell_write_err( "buffer" );
                return 1;
            }
        }
    }
    if( length == 0 )
        return -1;  /* no parms, nothing to do */

    /* write and read */
    if( !emu_spi_write( spi_index, buf_out, buf_in, length ) )
        return 1;  /* execute failed, maybe not inited */

    /* output read */
    if( read_mode )
    {
        line_count = 0;
        for( i=0; i<length; i++ )
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


/*************************************************************************/
/* Pulses generator                                                      */
/*************************************************************************/

#if USE_CMD_PULSE
#ifndef CMD_PULSE_PORT
#define CMD_PULSE_PORT  0
#endif
#ifndef CMD_PULSE_PIN
#define CMD_PULSE_PIN  0
#endif

#if USE_CMD_PULSE4
static pulse_cb_t *pulse_cb[4];
#elif USE_CMD_PULSE3
static pulse_cb_t *pulse_cb[3];
#elif USE_CMD_PULSE2
static pulse_cb_t *pulse_cb[2];
#else
static pulse_cb_t *pulse_cb[1];
#endif

void emu_pulse_init_structure(pulse_cb_t *pulse_init)
{
    pulse_init->port = CMD_PULSE_PORT;
    pulse_init->pin = CMD_PULSE_PIN;
    pulse_init->pin_bit = 1<<CMD_PULSE_PIN;
    pulse_init->invert = 0;
    pulse_init->delay_us = 5;
}


void emu_pulse_deinit(int pulse_index)
{
    pulse_cb_t *pulse=pulse_cb[pulse_index];

    if( pulse==0 )
        return;
    hal_gpio_set_input( pulse->port, pulse->pin_bit );
    vPortFree( (void*)pulse );
    pulse_cb[pulse_index] = 0;
}


int emu_pulse_init(int pulse_index, pulse_cb_t *pulse_init)
{
    pulse_cb_t *pulse=pulse_cb[pulse_index];
    
    if( pulse )
    {
        /* free previous pulse pins (maybe different from the new pins),
           but do not free the memory */
        hal_gpio_set_input( pulse->port, pulse->pin_bit );
    }
    else
    {
        pulse = pvPortMalloc( sizeof(pulse_cb_t) );
        if( pulse == 0 )
            return 0;  /* fail */
        pulse_cb[pulse_index] = pulse;
    }
  
    memcpy( pulse, pulse_init, sizeof(pulse_cb_t) );
    pulse->pin_bit = 1<<pulse->pin;
 
    hal_gpio_set_output( pulse->port, pulse->pin_bit );
    if( pulse->invert )
        hal_gpio_set( pulse->port, pulse->pin_bit );
    else
        hal_gpio_clr( pulse->port, pulse->pin_bit );
    return 1;
}


int emu_pulse_generate(int pulse_index, int number)
{
    pulse_cb_t *pulse=pulse_cb[pulse_index];

    if( pulse==0 )
        return 0;
    while( number-- )
    {
        portENTER_CRITICAL();
        if( pulse->invert )
            hal_gpio_clr( pulse->port, pulse->pin_bit );
        else
            hal_gpio_set( pulse->port, pulse->pin_bit );
        hal_delay_us( pulse->delay_us );
        if( pulse->invert )
            hal_gpio_set( pulse->port, pulse->pin_bit );
        else
            hal_gpio_clr( pulse->port, pulse->pin_bit );
        hal_delay_us( pulse->delay_us );
        portEXIT_CRITICAL();
    } 
    return 1;
}


int cmd_pulse( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          0, shell_str_delay, "delay_us", "default 5" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'p', shell_str_pin, "input_pin", "default 0.0" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'I', shell_str_init, 0, shell_str_init },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'D', shell_str_deinit, 0, shell_str_deinit },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          0, shell_str_invert, 0, shell_str_invert },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_number, 0, shell_str_number },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t init=0, deinit=0;
    char *p;
    pulse_cb_t pulse_init;
    int pulse_index=0;
    int counter;
 
    /* check which command pulse/pulse2/pulse3/pulse4 is used */
#if USE_CMD_PULSE2
    if( parse_int(argv[0]+5, (int*)&pulse_index) )
        pulse_index -= 1;
#endif
    emu_pulse_init_structure( &pulse_init );
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit = 1;
            else if( STRCMP( opt.spec->name, shell_str_delay ) == 0 )
                parse_int(opt.value, (int*)&pulse_init.delay_us);
            else if( STRCMP( opt.spec->name, shell_str_invert ) == 0 )
                pulse_init.invert = 1; 
            else if( STRCMP( opt.spec->name, shell_str_pin ) == 0 )
            {
                pulse_init.port = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                pulse_init.pin = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( STRCMP( opt.spec->name, shell_str_setting ) == 0 )
                break;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init )
    {
        return emu_pulse_init( pulse_index, &pulse_init ) ? 0 : 1;
    }
    else if( deinit )
    {
        emu_pulse_deinit( pulse_index );
        return 0;
    }
    else if( parser.idx >= argc )
    {
        return -1;
    }
    else if( ! parse_int(argv[parser.idx], &counter) )
    {
        shell_write_err( shell_str_number );
        return 1;
    }

    return emu_pulse_generate( pulse_index, counter ) ? 0 : 1;
 
err_port:
    shell_write_err( shell_str_port_bit );
    return 1;
}
#endif


/*************************************************************************/
/* Dallas 1-wire                                                         */
/*************************************************************************/

#if USE_CMD_DS1W

#ifndef CMD_DS1W_PORT
#define CMD_DS1W_PORT  0
#endif
#ifndef CMD_DS1W_PIN
#define CMD_DS1W_PIN  0
#endif


#if USE_CMD_DS1W4
static ds1w_cb_t ds1w_cb[4];
#elif USE_CMD_DS1W3
static ds1w_cb_t ds1w_cb[3];
#elif USE_CMD_DS1W2
static ds1w_cb_t ds1w_cb[2];
#else
static ds1w_cb_t ds1w_cb[1];
#endif


void emu_ds1w_init_structure( ds1w_cb_t *ds1w_init )
{
    ds1w_init->port = CMD_DS1W_PORT;
    ds1w_init->pin = CMD_DS1W_PIN;
}


int emu_ds1w_init( int ds1w_index, ds1w_cb_t *ds1w_init )
{
    ds1w_cb_t *ds1w=&ds1w_cb[ds1w_index];

    ds1w->port = ds1w_init->port;
    ds1w->pin = ds1w_init->pin;
    ds1w->pin_bit = 1<<(ds1w_init->pin);
    hal_gpio_set_output_open_drain( ds1w->port, ds1w->pin_bit ); 
    hal_gpio_set( ds1w->port, ds1w->pin_bit ); 
    return 1;
}


void emu_ds1w_deinit( int ds1w_index )
{
    ds1w_cb_t *ds1w=&ds1w_cb[ds1w_index];

    hal_gpio_set_input( ds1w->port, ds1w->pin_bit ); 
}


int emu_ds1w_reset( int ds1w_index )
{
    ds1w_cb_t *ds1w=&ds1w_cb[ds1w_index];
    int present;

    hal_gpio_clr( ds1w->port, ds1w->pin_bit ); 
    hal_delay_us( 500 );
    portENTER_CRITICAL(); 
    hal_gpio_set( ds1w->port, ds1w->pin_bit ); 
    hal_delay_us( 60 );
    present = hal_gpio_get( ds1w->port, ds1w->pin_bit );
    portEXIT_CRITICAL();
    hal_delay_us( 300 );
    return present ? 0 : 1;
}


void emu_ds1w_write_bit( int ds1w_index, int val )
{
    ds1w_cb_t *ds1w=&ds1w_cb[ds1w_index];

    portENTER_CRITICAL(); 
    hal_gpio_clr( ds1w->port, ds1w->pin_bit ); 
    hal_delay_us( val ? 15 : 60 );
    hal_gpio_set( ds1w->port, ds1w->pin_bit ); 
    portEXIT_CRITICAL();
    hal_delay_us( val ? 45 : 0 );
}


void emu_ds1w_write_byte( int ds1w_index, char byte )
{
    int i;

    for( i=0; i<8; i++ )
    {
        emu_ds1w_write_bit( ds1w_index, byte & 0x01 );
        byte >>= 1;
    }
}


int emu_ds1w_read_bit( int ds1w_index )
{
    ds1w_cb_t *ds1w=&ds1w_cb[ds1w_index];
    int val;

    portENTER_CRITICAL(); 
    hal_gpio_clr( ds1w->port, ds1w->pin_bit ); 
    hal_delay_us( 1 );
    hal_gpio_set( ds1w->port, ds1w->pin_bit ); 
    hal_delay_us( 9 );
    val = hal_gpio_get( ds1w->port, ds1w->pin_bit );
    portEXIT_CRITICAL();
    hal_delay_us( 50 );
    return val ? 1 : 0;
}


char emu_ds1w_read_byte( int ds1w_index )
{
    int i;
    char byte=0;
    
    for( i=0; i<8; i++ )
    {
        byte >>= 1;
        if( emu_ds1w_read_bit( ds1w_index ) )
            byte |= 0x80;
    }
    return byte;
}


int cmd_ds1w( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'r', shell_str_read, 0, shell_str_read },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'z', "r0", 0, "read bit 0" },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'w', shell_str_write, "byte", shell_str_write },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          '0', "w0", 0, "write bit 0" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          '1', "w1", 0, "write bit 1" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'R', shell_str_reset, 0, shell_str_reset },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'p', shell_str_pin, shell_str_pin, "default 0.0" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'I', shell_str_init, 0, shell_str_init },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'D', shell_str_deinit, 0, shell_str_deinit },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t reset_set=0, init_set=0, deinit_set=0;
    uint8_t write_set=0, write0_set=0, write1_set=0, read_set=0, read0_set=0;
    char write_byte=0;
    int i;
    char *p;
    int ds1w_index=0;
    ds1w_cb_t ds1w_init;
    
    /* check which command ds1w/ds1w2/ds1w3/ds1w4 is used */
#if USE_CMD_DS1W2
    if( parse_int(argv[0]+4, (int*)&ds1w_index) )
        ds1w_index -= 1;
#endif
    emu_ds1w_init_structure( &ds1w_init );
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_read ) == 0 )
                read_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_write ) == 0 )
            {
                if( parse_int(opt.value, &i) )
                {
                    write_byte = i;
                    write_set = 1;
                }
            }
            else if( strcmp( opt.spec->name, "w0" ) == 0 )
                write0_set = 1;
            else if( strcmp( opt.spec->name, "w1" ) == 0 )
                write1_set = 1;
            else if( strcmp( opt.spec->name, "r0" ) == 0 )
                read0_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_data ) == 0 )
            {
                parser.idx--;
                break;
            }
            else if( STRCMP( opt.spec->name, shell_str_reset ) == 0 )
                reset_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_pin ) == 0 )
            {
                ds1w_init.port = strtol( opt.value, &p, 10 );
                if( !p || (*p!='.') )
                    goto err_port;
                if( *(++p) == 0 )
                    goto err_port;
                ds1w_init.pin = strtol( p, &p, 10 );
                if( p && *p )
                    goto err_port;
            }
            else if( STRCMP( opt.spec->name, shell_str_init ) == 0 )
                init_set = 1;
            else if( STRCMP( opt.spec->name, shell_str_deinit ) == 0 )
                deinit_set = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }

    if( init_set )
    {
        emu_ds1w_init( ds1w_index, &ds1w_init );
        return 0;
    }
    else if( deinit_set )
    {
        emu_ds1w_deinit( ds1w_index );
        return 0;
    }
  
    /* bus reset */ 
    if( reset_set )
    {
        if( ! emu_ds1w_reset(ds1w_index) )
        {
            shell_write_line("not present");
            return 1;
        }
        else
            return 0;
    }

    /* read/write bit/byte */
    if( read0_set )
    {
        shell_printf( "%d\n", emu_ds1w_read_bit(ds1w_index) );
        return 0;
    }
    else if( read_set )
    {
        shell_printf( "%d\n", emu_ds1w_read_byte(ds1w_index) );
        return 0;
    } 
    else if( write0_set )
    {
        emu_ds1w_write_bit( ds1w_index, 0 );
        return 0;
    } 
    else if( write1_set )
    {
        emu_ds1w_write_bit( ds1w_index, 1 );
        return 0;
    } 
    else if( write_set )
    {
        emu_ds1w_write_byte( ds1w_index, write_byte );
        return 0;
    }

    return 1;  /* never run to here, nothing to do? */

err_port:
    shell_write_err( shell_str_port_bit );
    return -1;
}

#endif


