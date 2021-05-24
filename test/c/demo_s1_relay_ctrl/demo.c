#include <stdio.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_api.h"

#ifdef WIN32
const char device_name[] = "\\\\.\\COM14";
#else
//const char device_name[] = "/dev/ttyUSB0";
const char device_name[] = "/dev/ttyACM1";
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
    int i;

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

    if( mcush_scpi_idn( &dev, buf ) > 0 )
        printf( "%s", buf );

    mcush_write_command( &dev, "gpio -p0 -o 0x0F -c 0x0F" );
    i = 0;
    while( 1 )
    {
        sprintf( buf, "gpio -p0 -c%d", 1<<i );
        mcush_write_command( &dev, buf );
        i++;
        if( i>=4 )
            i = 0;
        sprintf( buf, "gpio -p0 -s%d", 1<<i );
        mcush_write_command( &dev, buf );

        delay_ms(250);
    }

    mcush_close( &dev );
    return 0;
}

