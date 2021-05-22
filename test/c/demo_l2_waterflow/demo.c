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

int strap_length = 30;
int push_delay_ms = 50;
int steps = 3;
int colors[] = { 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF };


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

    /* directly call command and check result */
    //mcush_write_command( &dev, "*idn?" );
    //printf( "%s", dev.result );
    //mcush_write_command( &dev, "cat /r/readme" );
    //printf( "%s", dev.result );

    /* use api instead */
    if( mcush_scpi_idn( &dev, buf ) > 0 )
        printf( "%s", buf );
    //mcush_scpi_rst( &dev );

    /* init */
    sprintf( buf, "W -l %d -I", strap_length );
    mcush_write_command( &dev, buf );

    /* push colors again and again until Ctrl-C */
    while( 1 )
    {
        /* push colors */
        for( i=0; i<6; i++ )
        {
            sprintf( buf, "L -l%d -n%d W -w -f 0x%X", push_delay_ms, steps, colors[i] );
            mcush_write_command( &dev, buf );
        }
    }

    mcush_close( &dev );
    return 0;
}

