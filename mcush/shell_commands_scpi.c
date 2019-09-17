/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


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

    shell_write_str( shell_str_mcush );
    shell_write_char( ',' );
    shell_write_line( MCUSH_VERSION_STRING );
    buf[0] = 0;
    if( hal_get_serial_number(buf) && strlen(buf) )
        shell_write_line( buf );

    return 0;
#endif
}
#endif


#if USE_CMD_SCPI_RST_CUSTOM
int cmd_scpi_rst_custom( int argc, char *argv[] );
#endif
#if USE_CMD_SCPI_RST
int cmd_scpi_rst( int argc, char *argv[] )
{
#if USE_CMD_SCPI_RST_CUSTOM
    return cmd_scpi_rst_custom( argc, argv );
#else
    /* *rst command ignore all arguments */
    hal_platform_reset();
    return 0;
#endif
}
#endif

