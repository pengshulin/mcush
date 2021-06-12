#include <stdio.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_drv.h"


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
    char buf[512];
    int ret;
    McushDrv *drv;

    drv = new McushDrv("/dev/ttyUSB0", 9600, 5);
    if( drv->err <= 0 )
    {
        printf("fail to open port\n");
        return 0;
    }

    if( drv->connect() <= 0 )
    {
        printf("fail to connect the device\n");
        return 0;
    }

    if( drv->scpiIdn( buf ) )
        printf( "%s", buf );
    drv->scpiRst();

    return 0;
}

