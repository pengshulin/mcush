#include <stdio.h>
#include "mcush_api.h"


int main( int argc, char *argv[] )
{
    mcush_dev_t dev;
    char buf[512];
    int ret;

    if( mcush_open( &dev, "/dev/ttyUSB0", 9600 ) <= 0 )
    {
        printf("fail to open device\n");
        return 0;
    }

    if( mcush_connect( &dev ) <= 0 )
    {
        printf("fail to connect the device\n");
        return 0;
    }

    /* directly call command and check result */
    //mcush_write_command( &dev, "*idn?" );
    //printf( "%s", dev.result );
    //mcush_write_command( &dev, "cat /r/readme" );
    //printf( "%s", dev.result );

    /* use api instead */
    if( mcush_scpi_idn( &dev, buf ) > 0 )
        printf( "%s", buf );
    mcush_scpi_rst( &dev );

    /* test port disconnect */
    while( 1 )
    {
        ret = mcush_scpi_idn( &dev, buf );
        if( ret > 0 )
        {
            printf( "." );
            fflush(stdout);
        }
        else
        {
            printf("\nerror ret %d\n", ret ); 
            return 1;
        }
    }

    mcush_close( &dev );
    return 0;
}

