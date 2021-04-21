#include <stdio.h>
#include "mcush_api.h"


int main( int argc, char *argv[] )
{
    mcush_dev_t dev;

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

    mcush_write_command( &dev, "*idn?" );
    printf( "%s", dev.result );

    mcush_write_command( &dev, "cat /r/readme" );
    printf( "%s", dev.result );

    mcush_close( &dev );
    return 0;
}

