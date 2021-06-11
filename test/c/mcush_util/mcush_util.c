#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_api.h"


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
    int ret;
    int opt;
    int baudrate=9600;
    int timeout=5;
    const char *port = NULL;

    while( (opt = getopt(argc, argv, "b:p:t:")) != -1 )
    {
       switch (opt) {
       case 'b':
           baudrate = atoi(optarg);
           break;
       case 't':
           timeout = atoi(optarg);
           break;
       case 'p':
           port = optarg;
           break;
       default:
           fprintf(stderr, "Usage: %s [-p port] [-b baudrate] [-t timeout] command\n", argv[0]);
           exit(EXIT_FAILURE);
       }
    }

    if( port == NULL )
    {
        fprintf(stderr, "port not set!\n");
        exit(EXIT_FAILURE);
    }

    if( mcush_open( &dev, port, baudrate, timeout ) <= 0 )
    {
        printf("fail to open port\n");
        exit(EXIT_FAILURE);
    }

    if( mcush_connect( &dev ) <= 0 )
    {
        printf("fail to connect the device\n");
        goto stop;
    }

    /* *idn? check */
    if( mcush_scpi_idn( &dev, buf ) <= 0 )
    {
        printf( "*idn? error\n" );
        goto stop;
    }
    printf( "%s", buf );

    if( optind >= argc )
    {
        printf( "command not set\n" );
        goto stop;
    }

    /* call command and check result */
    ret = mcush_write_command( &dev, argv[optind] );
    if( ret < 0 )
    {
        switch( ret )
        {
        case MCUSH_ERR_RET_CMD_NOT_MATCH:
            printf( "W/R command does not match\n" );
            break;
        case MCUSH_ERR_IO:
            printf( "Port IO error\n" );
            break;
        default:
            printf( "Error return %d\n", ret );
            break;
        }
    }
    else
    {
        printf( "%s\n", dev.result );
    }


stop:
    mcush_close( &dev );
    return 0;
}

