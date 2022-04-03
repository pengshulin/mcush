/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


/* NOTE: 
   pseudo ttyA/B pair in /dev/shm should be created and combined together
   before application starts, with socat tool:
   "socat PTY,link=/dev/shm/ttyA,rawer  PTY,link=/dev/shm/ttyB,rawer"
   use ttyB for manual/automation debug
*/
#ifndef HAL_UARTx_PORT
    #define HAL_UARTx_PORT                  "/dev/shm/ttyA"
#endif

#ifndef HAL_UARTx_BAUDRATE
    #define HAL_UARTx_BAUDRATE              9600
#endif

/* NOTE: 
   QUEUE length is set to 10 by default (often limited by OS), short but works.
   for larger value, modify /proc/sys/fs/mqueue/msg_max as root 
*/
#ifndef HAL_UART_QUEUE_RX_LEN
    #define HAL_UART_QUEUE_RX_LEN           10
#endif

#ifndef HAL_UART_QUEUE_TX_LEN
    #define HAL_UART_QUEUE_TX_LEN           10
#endif

os_queue_handle_t hal_uart_queue_rx, hal_uart_queue_tx;
int hal_uart_handle;
pthread_t thread_rx, thread_tx;


void *thread_rx_listener(void *arg)
{
    int ret;
    int c;

    (void)arg;
    printf("start thread_rx_listener\n");
    
    while( 1 )
    {
        ret = read( hal_uart_handle, &c, 1 );
        if( ret == -1 )
            break;
        //printf("thread_rx_listener: %c (0x%X)\n", c, c);
        if( os_queue_put( hal_uart_queue_rx, &c, -1 ) == 0 )
            break;
    }
    printf("stop thread_rx_listener\n");
    return NULL;
}


void *thread_tx_writer(void *arg)
{
    int ret;
    char c;

    (void)arg;
    printf("start thread_tx_writer\n");

    while( 1 )
    {
        if( os_queue_get( hal_uart_queue_tx, &c, -1 ) == -1 )
            break;
        //printf("thread_tx_writer: %c (0x%X)\n", c, c);
        ret = write( hal_uart_handle, &c, 1 );
        if( ret < 0 )
            break;
        ret = tcdrain( hal_uart_handle );
        if( ret < 0 )
            break;
    }
    printf("stop thread_tx_writer\n");
    return NULL;
}

int hal_uart_init( uint32_t baudrate )
{

#if OS_SUPPORT_STATIC_ALLOCATION
#if HAL_UART_QUEUE_RX_LEN
    DEFINE_STATIC_QUEUE_BUFFER( uart_rx, HAL_UART_QUEUE_RX_LEN, 1 );
    hal_uart_queue_rx = os_queue_create_static( "rxQ", HAL_UART_QUEUE_RX_LEN, 1,
                                            &static_queue_buffer_uart_rx );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
#if HAL_UART_QUEUE_TX_LEN
    DEFINE_STATIC_QUEUE_BUFFER( uart_tx, HAL_UART_QUEUE_TX_LEN, 1 );
    hal_uart_queue_tx = os_queue_create_static( "txQ", HAL_UART_QUEUE_TX_LEN, 1,
                                            &static_queue_buffer_uart_tx );
    if( hal_uart_queue_tx == NULL )
        return 0;
#endif
#else
#if HAL_UART_QUEUE_RX_LEN
    hal_uart_queue_rx = os_queue_create( "rxQ", HAL_UART_QUEUE_RX_LEN, 1 );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
#if HAL_UART_QUEUE_TX_LEN
    hal_uart_queue_tx = os_queue_create( "txQ", HAL_UART_QUEUE_TX_LEN, 1 );
    if( hal_uart_queue_tx == NULL )
        return 0;
#endif
#endif


    struct termios newtio;
    speed_t speed;
    int ret;

    switch( baudrate )
    {
    case 150: speed = B150; break;
    case 200: speed = B200; break;
    case 300: speed = B300; break;
    case 600: speed = B600; break;
    case 1200: speed = B1200; break;
    case 1800: speed = B1800; break;
    case 2400: speed = B2400; break;
    case 4800: speed = B4800; break;
    case 0:
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    case 460800: speed = B460800; break;
    default: return 0;
    }

    hal_uart_handle = open( HAL_UARTx_PORT, O_RDWR);
    if( hal_uart_handle < 0 )
        return 0;

    memset( &newtio, 0, sizeof(struct termios) );
    ret = tcgetattr( hal_uart_handle, &newtio );
    if( ret < 0 )
        return 0;
    ret = tcflush( hal_uart_handle, TCIOFLUSH );
    if( ret < 0 )
        return 0;

    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_lflag = 0;
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 0;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer */
    newtio.c_cc[VMIN]     = 1;     /* blocking read */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    cfsetspeed( &newtio, speed );
    ret = tcsetattr( hal_uart_handle, TCSANOW, &newtio );
    if( ret < 0 )
        return 0;

    /* create tx/rx threads */
    ret = pthread_create( &thread_tx, NULL, thread_tx_writer, NULL );
    if( ret != 0 )
        return 0;
    ret = pthread_create( &thread_rx, NULL, thread_rx_listener, NULL );
    if( ret != 0 )
    {
        pthread_cancel( thread_tx );
        return 0;
    }

    return 1;
}


void hal_uart_reset(void)
{
#if HAL_UART_QUEUE_RX_LEN
    os_queue_reset( hal_uart_queue_rx );
#endif
#if HAL_UART_QUEUE_TX_LEN
    os_queue_reset( hal_uart_queue_tx );
#endif
}


void hal_uart_enable(uint8_t enable)
{
    (void)enable;
}


int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_TX_LEN
    if( os_queue_put( hal_uart_queue_tx, &c, block_ticks ) )
    {
        return 1;
    }
    else
#endif
        return 0;
}


int hal_uart_getc( char *c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_RX_LEN
    return os_queue_get( hal_uart_queue_rx, c, block_ticks );
#else
    return 0;
#endif
}


int hal_uart_feedc( char c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_RX_LEN
    if( os_queue_put( hal_uart_queue_rx, &c, block_ticks ) )
        return 1;
    else
#endif
        return 0;
}



/****************************************************************************/
/* shell APIs                                                               */
/****************************************************************************/

int shell_driver_init( void )
{
    return hal_uart_init(0);
}


void shell_driver_reset( void )
{
    hal_uart_reset();
}


int  shell_driver_read_feed( char *buffer, int len )
{
    int bytes=0;
    while( bytes < len )
    {
        while( hal_uart_feedc( *(char*)((intptr_t)buffer + bytes), -1 ) == 0 )
            os_task_delay(1);
        bytes += 1;
    }
    return 0;
}


int  shell_driver_read( char *buffer, int len )
{
    (void)buffer;
    (void)len;
    return 0;  /* not supported */
}


int  shell_driver_read_char( char *c )
{
    if( hal_uart_getc( c, -1 ) == 0 )
        return -1;
    else
        return (int)*c;
}


int  shell_driver_read_char_blocked( char *c, int block_ticks )
{
    if( hal_uart_getc( c, block_ticks ) == 0 )
        return -1;
    else
        return (int)*c;
}


int  shell_driver_read_is_empty( void )
{
    return 1;
}


int  shell_driver_write( const char *buffer, int len )
{
    int written=0;

    while( written < len )
    {
        while( hal_uart_putc( *(char*)((uintptr_t)buffer + written), -1 ) == 0 )
            os_task_delay(1);
        written += 1;
    }
    return written;
}


void shell_driver_write_char( char c )
{
    while( hal_uart_putc( c, -1 ) == 0 )
        os_task_delay(1);
}


