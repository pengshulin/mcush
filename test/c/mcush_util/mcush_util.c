#include <stdio.h>
#include <string.h>
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


void print_usage( int argc, char *argv[] )
{
    fprintf(stderr, "Usage: %s [-p port] [-b baudrate] [-t timeout] command [<parms>]\n", argv[0]);
    fprintf(stderr, "commands list:\n");
    fprintf(stderr, "  reset      scpi *rst command\n");
    fprintf(stderr, "  check      check if command is supported by device\n");
    fprintf(stderr, "             <cmd>\n");
    fprintf(stderr, "  run        run command until done\n");
    fprintf(stderr, "             <quoted_cmd_args>\n");
    fprintf(stderr, "  get        get file\n");
    fprintf(stderr, "             <src_pathname> <local_pathname>\n");
    fprintf(stderr, "  put        put file\n");
    fprintf(stderr, "             <local_pathname> <dst_pathname>\n");
}


int exec_run_command( mcush_dev_t *dev, const char *command )
{
    int ret = mcush_write_command( dev, command );
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
        printf( "%s\n", dev->result );
    }
    return ret;
}


int exec_check_command( mcush_dev_t *dev, const char *command )
{
    return 1;
}


int exec_get_file( mcush_dev_t *dev, const char *src_pathname, const char *local_pathname )
{
    /* TODO: read file from device and save as local file */
    return 0;
}


int exec_put_file( mcush_dev_t *dev, const char *local_pathname, const char *dst_pathname )
{
    /* TODO: read local file and save to device */
    return 0;
}


int main( int argc, char *argv[] )
{
    mcush_dev_t dev;
    char buf[512];
    int opt;
    int baudrate=9600;
    int timeout=5;
    const char *port = NULL;
    char *cmd = NULL;
    int err=0;


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
           print_usage( argc, argv );
           exit(EXIT_FAILURE);
       }
    }

    if( port == NULL )
    {
        fprintf(stderr, "port not set\n");
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
        err = 1;
        goto stop;
    }

    /* *idn? check */
    if( mcush_scpi_idn( &dev, buf ) <= 0 )
    {
        printf( "*idn? error\n" );
        err = 1;
        goto stop;
    }
    printf( "%s", buf );

    if( optind >= argc )
        goto stop;  /* nothing to do */
    cmd = argv[optind++];

    if( strcmp(cmd, "reset") == 0 )
    {
        if( mcush_scpi_rst( &dev ) <= 0 )
        {
            printf( "*rst error\n" );
            err = 1;
        }
    }
    else if( strcmp(cmd, "check") == 0 )
    {
        if( optind >= argc )
        {
            printf( "command not set\n" );
            err = 1;
            goto stop;
        }
        err = exec_check_command( &dev, argv[optind] ) ? 0 : 1;
    }
    else if( strcmp(cmd, "run") == 0 )
    {
        if( optind >= argc )
        {
            printf( "command not set\n" );
            err = 1;
            goto stop;
        }
        exec_run_command( &dev, argv[optind] );
    }
    else if( strcmp(cmd, "get") == 0 )
    {
        if( optind+1 >= argc )
        {
            printf("<src_pathname> <local_pathname> not set\n");
            err = 1;
            goto stop;
        }
        exec_get_file( &dev, argv[optind], argv[optind] );
    }
    else if( strcmp(cmd, "put") == 0 )
    {
        if( optind+1 >= argc )
        {
            printf("<local_pathname> <dst_pathname> not set\n");
            err = 1;
            goto stop;
        }
        exec_put_file( &dev, argv[optind], argv[optind] );
    }
    else
    {
        fprintf( stderr, "unsupported command %s\n", cmd );
        print_usage( argc, argv );
        err = 1;
    }

stop:
    mcush_close( &dev );
    return err;
}

