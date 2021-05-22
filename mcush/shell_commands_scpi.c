/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"

#ifdef MODEL_STRING
    const char model_string[] = MODEL_STRING;
#else
    const char model_string[] = "mcush";
#endif

#ifdef VERSION_STRING
    const char version_string[] = VERSION_STRING;
#else
    const char version_string[] = MCUSH_VERSION_STRING;
#endif


#if USE_CMD_SCPI_IDN
__weak int cmd_scpi_idn( int argc, char *argv[] )
{
    char buf[64];

    /* *idn? command ignore all arguments */
    (void)argc;
    (void)argv;

    shell_write_str( model_string );
    shell_write_char( ',' );
    shell_write_line( version_string );
    buf[0] = 0;
    if( hal_get_serial_number(buf) && strlen(buf) )
        shell_write_line( buf );

    return 0;
}
#endif


#if USE_CMD_SCPI_RST
__weak int cmd_scpi_rst( int argc, char *argv[] )
{
    /* *rst command ignore all arguments */
    (void)argc;
    (void)argv;

    hal_platform_reset();
    return 0;
}
#endif

