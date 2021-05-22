#include <stdio.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_api.h"

#ifdef WIN32
const char device_name[] = "\\\\.\\COM14";
#else
const char device_name[] = "/dev/ttyUSB0";
#endif

void delay_ms(int ms)
{
#ifdef WIN32
    Sleep(ms);  // in ms
#else
    usleep(ms*1000);  // in us
#endif
}


int main( int argc, char *argv[] )
{
    mcush_dev_t dev;
    char buf[512];

    if( mcush_open( &dev, device_name, 9600 ) <= 0 )
    {
        printf("fail to open port\n");
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
    //mcush_scpi_rst( &dev );

    /* load script */
    printf("load\n" );
    mcush_write_command( &dev, "load /c/run" );
    printf("load done\n" );

    mcush_close( &dev );
    return 0;
}

