#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "mcush_api.h"
#include "mcush_base64.h"
#include <termios.h>
#include <pthread.h>
#include <signal.h>


int baudrate=9600;
int timeout=5;
int quiet=0;
#ifdef WIN32
const char *port = NULL;
#else
const char *port = "/dev/ttyUSB0";
#endif
const char *model = NULL;
//int shell_alive=0;

int connected=0;
pthread_t thread_rx;
pthread_t thread_tx;


void delay_ms(int ms)
{
#ifdef WIN32
    Sleep(ms);  // in ms
#else
    usleep(ms*1000);  // in us
#endif
}


static void print_usage( int argc, char *argv[] )
{
    printf("Usage: %s [-q] [-p port] [-b baudrate] [-t timeout] [-m model] [-n] [-r] command [<parms>]\n", argv[0]);
    printf("arguments list:\n");
    printf("  -q         quiet mode, do not print command response\n");
    printf("  -p         serial port, default /dev/ttyUSB0\n");
    printf("  -b         baudrate, default 9600\n");
    printf("  -t         IO timeout, default 5s\n");
    printf("  -m         model match\n");
    printf("  -n         no scpi idn check\n");
    printf("  -r         scpi rst\n");
    printf("commands list:\n");
    printf("  check      check if command is supported by device\n");
    printf("             <cmd>\n");
    printf("  run        run commands one by one\n");
    printf("             <quoted_cmd1_args> [quoted_cmd2_args] ...\n");
    printf("  get        get/pull file\n");
    printf("             <dev_pathname> [<local_pathname>]\n");
    printf("  put        put/push file\n");
    printf("             <local_pathname> <dev_pathname>\n");
    printf("  shell      interactive shell\n");
}


int exec_run_command( mcush_dev_t *dev, const char *command )
{
    int ret = mcush_write_command( dev, command );
    if( ret < 0 )
    {
        switch( ret )
        {
        case MCUSH_ERR_API:
            printf( "API parms error\n" );
            break;
        case MCUSH_ERR_IO:
            printf( "Port IO error\n" );
            break;
        case MCUSH_ERR_MEMORY:
            printf( "Memory error\n" );
            break;
        case MCUSH_ERR_TIMEOUT:
            printf( "Timeout error\n" );
            break;
        case MCUSH_ERR_SYNTAX:
            printf( "Syntax error\n" );
            printf( "%s\n", dev->result );
            break;
        case MCUSH_ERR_EXEC:
            printf( "Execution error\n" );
            break;
        case MCUSH_ERR_RET_CMD_NOT_MATCH:
            printf( "W/R command does not match\n" );
            break;
        default:
            printf( "Error return %d\n", ret );
            break;
        }
    }
    else
    {
        if( quiet == 0 )
            printf( "%s\n", dev->result );
    }
    return ret;
}


int exec_check_command( mcush_dev_t *dev, const char *command )
{
    int ret;
    char buf[512], *p;

    strcpy( buf, "? -c " );
    strcat( buf, command );
    ret = exec_run_command( dev, buf );
    if( ret < 0 )
        return ret;
    if( strtol( (const char*)dev->result, &p, 10 ) == 1 )
    {
        /* if command is supported, return normal exit code ZERO */
        return 0;
    }
    return 1;
}


/* read file from device and save as local file */
int exec_get_file( mcush_dev_t *dev, const char *dev_pathname, const char *local_pathname )
{
    int ret;
    char buf[512];
    base64_decodestate state_de;
    char *p, *p2;
    int i, j;
    FILE *fout=NULL;

    strcpy( buf, "cat -b " );
    strcat( buf, dev_pathname );
    ret = exec_run_command( dev, buf );
    if( ret < 0 )
        return ret;
    /* prepare local file handler */
    if( local_pathname != NULL )
    {
        if( strcmp( local_pathname, "-" ) != 0 )
        {
            fout = fopen( local_pathname, "wb+" );
            if( fout == NULL )
            {
                printf( "Failed to write to file %s\n", local_pathname );
                return 1;
            }
        }
    }
    /* parse and print/save b64 encoded binary data */
    base64_init_decodestate( &state_de );
    p2 = (char*)dev->result;
    while( 1 )
    {
        p = p2;
        i = 0;
        while( *p2 && *p2 != '\n')
        {
            p2++;
            i++;
        }
        if( !*p2 && !i )
            break;
        if( *p2 == '\n' )
            *p2++ = 0;
        if( !i )
            break;
        j = base64_decode_block( (const char*)p, i, (char*)&buf, &state_de );
        if( j )
        {
            if( fout != NULL )
            {
                if( fwrite( buf, 1, j, fout ) != j )
                {
                    printf( "Failed to append to file %s\n", local_pathname );
                    fclose( fout );
                }
            }
            else
            {
                for( i=0; i<j; i++ )
                    putchar( buf[i] );
            }
        }
    }
    if( fout != NULL )
        fclose( fout );
    return 0;
}


int exec_put_file( mcush_dev_t *dev, const char *local_pathname, const char *dev_pathname )
{
    /* TODO: read local file and save to device */
    return 0;
}


void *thread_serial_rx_listener(void *arg) 
{
    mcush_dev_t *dev = (mcush_dev_t *)arg;
    char c;

    while( connected )
    {
        if(mcush_getc( dev, &c ) )
        {
            putchar( c );
        }
        else
        {
            puts("\n[disconnected]");
            connected = 0;
        }
        fflush(stdout);
    }
    pthread_cancel( thread_tx );
    return NULL;
}


void *thread_serial_tx_writer(void *arg) 
{
    mcush_dev_t *dev = (mcush_dev_t *)arg;
    int ch;

    printf("Shell Mode, Ctrl-] to quit, ? for help\n");
    mcush_putc( dev, MCUSH_TERMINATOR_RESET );
    while( connected )
    {
        ch = getchar();
        if( ch == 29 )  /* Ctrl - ] */
        {
            connected = 0;
            break;
        }
        mcush_putc( dev, ch );
    }
    pthread_cancel( thread_rx );
    return NULL;
}


/* simple interactive shell for manual debug */
int exec_shell( mcush_dev_t *dev )
{
    struct termios oldattr, newattr, serattr;
    int err;

    /* backup/update terminal behavior */
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    newattr.c_cc[VINTR] = 0;  /* Ctrl-C */
    newattr.c_cc[VSUSP] = 0;  /* Ctrl-Z */
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );

    /* update serial port into blocking read mode */
    tcgetattr( dev->handle, &serattr );
    serattr.c_cc[VTIME] = 0;  /* timeout */
    serattr.c_cc[VMIN] = 1;  /* blocking read */
    tcsetattr( dev->handle, TCSANOW, &serattr );

    /* start RX listening thread */
    err = pthread_create( &thread_rx, NULL, thread_serial_rx_listener, (void*)(dev) );
    if( err )
        return err;
    /* start TX writer thread */
    err = pthread_create( &thread_tx, NULL, thread_serial_tx_writer, (void*)(dev) );
    if( err )
    {
        pthread_cancel( thread_rx );
        return err;
    }

    /* wait until tx thread is stopped */
    pthread_join( thread_rx, NULL );
    pthread_join( thread_tx, NULL );

    /* restore terminal */
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    putchar('\n');
    return 0;
}


int main( int argc, char *argv[] )
{
    mcush_dev_t dev;
    char buf[512];
    int opt;
    char *cmd = NULL;
    int err=0;
    int idn=1;
    int rst=0;

    while( (opt = getopt(argc, argv, "qb:p:t:m:nr")) != -1 )
    {
        switch (opt) {
        case 'q': quiet = 1; break;
        case 'b': baudrate = atoi(optarg); break;
        case 't': timeout = atoi(optarg); break;
        case 'p': port = optarg; break;
        case 'm': model = optarg; break;
        case 'n': idn = 0; break;
        case 'r': rst = 1; break;
        default:
            print_usage( argc, argv );
            exit(EXIT_FAILURE);
        }
    }

    if( port == NULL )
    {
        printf("port not set\n");
        exit(EXIT_FAILURE);
    }

    if( mcush_open( &dev, port, baudrate, timeout ) <= 0 )
    {
        printf("fail to open port %s\n", port);
        exit(EXIT_FAILURE);
    }

    /* connect: send Ctrl-C and wait for prompt */
    if( mcush_connect( &dev ) <= 0 )
    {
        printf("fail to connect the device %s\n", port);
        err = 1;
        goto close_stop;
    }

    /* *idn? check */
    if( idn || (model != NULL) )
    {
        if( mcush_scpi_idn( &dev, buf ) <= 0 )
        {
            printf( "*idn? error\n" );
            err = 1;
            goto close_stop;
        }
        if( quiet == 0 )
            printf( "%s", buf );
    }

    /* model match check */
    if( model != NULL )
    {
        /* just compare the prefix, ignore the remaining */
        if( strncmp(buf, model, strlen(model)) != 0 
            /*|| buf[strlen(model)] != ','*/ )
        {
            printf( "model match error\n" );
            err = 1;
            goto close_stop;
        }
    }

    connected = 1;
    /* scpi reset */
    if( rst )
    {
        if( mcush_scpi_rst( &dev ) <= 0 )
        {
            printf( "*rst error\n" );
            err = 1;
            goto close_stop;
        }
    }

    if( optind >= argc )
        goto close_stop;  /* nothing to do */
    cmd = argv[optind++];

    if( strcmp(cmd, "check") == 0 )
    {
        if( optind >= argc )
        {
            printf( "command not set\n" );
            err = 1;
            goto close_stop;
        }
        err = exec_check_command( &dev, argv[optind] );
    }
    else if( strcmp(cmd, "run") == 0 )
    {
        if( optind >= argc )
        {
            printf( "command not set\n" );
            err = 1;
            goto close_stop;
        }
        /* execute multiple commands one by one */
        while( optind < argc )
        {
            err = exec_run_command( &dev, argv[optind] );
            if( err < 0 )
                break;
            optind++;
        }
        if( err > 0 )
            err = 0;
    }
    else if( strcmp(cmd, "get") == 0 )
    {
        if( optind >= argc )
        {
            printf("<dev_pathname> [<local_pathname>] not set\n");
            err = 1;
            goto close_stop;
        }
        exec_get_file( &dev, argv[optind], (optind+1>=argc) ? NULL : argv[optind+1] );
    }
    else if( strcmp(cmd, "put") == 0 )
    {
        if( optind+1 >= argc )
        {
            printf("<local_pathname> <dev_pathname> not set\n");
            err = 1;
            goto close_stop;
        }
        exec_put_file( &dev, argv[optind], argv[optind] );
    }
    else if( strcmp(cmd, "shell") == 0 )
    {
        exec_shell( &dev );
    }
    else
    {
        printf("unsupported command %s\n", cmd);
        print_usage( argc, argv );
        err = 1;
    }

close_stop:
    mcush_close( &dev );
    exit(err);
}

