/* uart driver template for MCUSH/lpc43xx
   (change the uart port)
1. define HAL_UART_DEFINE
2. define other HAL_UARTxxxxxxx configurations
   (port the driver to other module)
1. search and replace HAL_UART wth HAL_XXXX
2. search and replace hal_uart to hal_xxxx
3. modify/remove shell apis at the bottom
*/
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)         (PC)
   TXD_n ------> RXD
   RXD_n <------ TXD 
   ----------------------------- 
 */
#include "mcush.h"
#include "ring_buffer.h"


#ifndef HAL_UART_DEFINE 
    #define HAL_UARTx                       LPC_USART3
    #define HAL_UARTx_IRQn                  USART3_IRQn
    #define HAL_UARTx_IRQHandler            UART3_IRQHandler
#endif

#ifndef HAL_UARTx_BAUDRATE
    #define HAL_UARTx_BAUDRATE              9600
#endif
#ifndef HAL_UART_RX_QUEUE_LEN
    #define HAL_UART_RX_QUEUE_LEN           128
#endif
#ifndef HAL_UART_TX_QUEUE_LEN
    #define HAL_UART_TX_QUEUE_LEN           128
#endif
 
/* rx buffer: half for ringbuffer and half for os_queue */
os_queue_handle_t hal_uart_queue_rx;
/* tx buffer: ringbuffer instead of os_queue */
//os_queue_handle_t hal_uart_queue_tx;
#if OS_SUPPORT_STATIC_ALLOCATION
DEFINE_STATIC_QUEUE_BUFFER( uart_rx, HAL_UART_RX_QUEUE_LEN/2, 1 );
//DEFINE_STATIC_QUEUE_BUFFER( uart_tx, HAL_UART_TX_QUEUE_LEN, 1 );
#endif

static RINGBUFF_T rb_uart_tx, rb_uart_rx;
#define RB_UART_TX_LEN  HAL_UART_TX_QUEUE_LEN
#define RB_UART_RX_LEN  HAL_UART_RX_QUEUE_LEN/2
static char buffer_uart_tx[RB_UART_TX_LEN], buffer_uart_rx[RB_UART_RX_LEN];


int hal_uart_init(uint32_t baudrate)
{
    RingBuffer_Init( &rb_uart_rx, (void*)buffer_uart_rx, 1, RB_UART_RX_LEN );
    RingBuffer_Init( &rb_uart_tx, (void*)buffer_uart_tx, 1, RB_UART_TX_LEN );
#if OS_SUPPORT_STATIC_ALLOCATION
#if HAL_UART_RX_QUEUE_LEN
    hal_uart_queue_rx = os_queue_create_static( "rxQ", HAL_UART_RX_QUEUE_LEN/2, 1,
                                            &static_queue_buffer_uart_rx );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
//#if HAL_UART_TX_QUEUE_LEN
//    hal_uart_queue_tx = os_queue_create_static( "txQ", HAL_UART_TX_QUEUE_LEN, 1,
//                                            &static_queue_buffer_uart_tx );
//    if( hal_uart_queue_tx == NULL )
//        return 0;
//#endif
#else
#if HAL_UART_RX_QUEUE_LEN
    hal_uart_queue_rx = os_queue_create( "rxQ", HAL_UART_RX_QUEUE_LEN/2, 1 );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
//#if HAL_UART_TX_QUEUE_LEN
//    hal_uart_queue_tx = os_queue_create( "txQ", HAL_UART_TX_QUEUE_LEN, 1 );
//    if( hal_uart_queue_tx == NULL )
//        return 0;
//#endif
#endif
 
    Chip_UART_Init(HAL_UARTx);
    Chip_UART_SetBaud(HAL_UARTx, baudrate==0 ? HAL_UARTx_BAUDRATE : baudrate);
    Chip_UART_ConfigData(HAL_UARTx, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
    Chip_UART_TXEnable(HAL_UARTx);
    //Chip_UART_SetupFIFOS(HAL_UARTx, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV0)); // 1Byte
    //Chip_UART_SetupFIFOS(HAL_UARTx, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV1)); // 4Bytes
    Chip_UART_SetupFIFOS(HAL_UARTx, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV2)); // 8Bytes
    //Chip_UART_SetupFIFOS(HAL_UARTx, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV3)); // 14Bytes
    //Chip_UART_IntEnable(HAL_UARTx, (UART_IER_RBRINT | UART_IER_RLSINT));
    Chip_UART_IntEnable(HAL_UARTx, (UART_IER_RBRINT | UART_IER_THREINT | UART_IER_RLSINT));
    NVIC_SetPriority( HAL_UARTx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), \
                    //configLIBRARY_LOWEST_INTERRUPT_PRIORITY, 0));
                    7, 0));
    NVIC_EnableIRQ(HAL_UARTx_IRQn);
    return 1;
}



int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_TX_QUEUE_LEN
    int written;

    (void)block_ticks;
    os_enter_critical();
    written = Chip_UART_SendRB( HAL_UARTx, &rb_uart_tx, (const void*)&c, 1 );
    os_exit_critical();
    return written ? 1 : 0;
#else
    return 0;
#endif
}


int hal_uart_getc( char *c, os_tick_t block_ticks )
{
#if HAL_UART_RX_QUEUE_LEN
    return os_queue_get( hal_uart_queue_rx, c, block_ticks );
#else
    return 0;
#endif
}


void HAL_UARTx_IRQHandler(void)
{
    char c;

    Chip_UART_IRQRBHandler( HAL_UARTx, &rb_uart_rx, &rb_uart_tx );
 
    /* ignore uart interrupt before scheduler run (eg. reset by wdg) */
    if( hal_uart_queue_rx == 0 )
        return;

    while( os_queue_is_full_isr( hal_uart_queue_rx ) == 0 )
    {
        /* process received data */
        if( RingBuffer_Pop( &rb_uart_rx, &c ) == 0 )
            break;
        if( os_queue_put_isr( hal_uart_queue_rx, &c ) == 0 )
            break;
    }
}


void hal_uart_reset(void)
{
#if HAL_UART_RX_QUEUE_LEN
    os_queue_reset( hal_uart_queue_rx );
#endif
//#if HAL_UART_TX_QUEUE_LEN
//    os_queue_reset( hal_uart_queue_tx );
//#endif
    os_enter_critical();
    RingBuffer_Flush( &rb_uart_tx );
    RingBuffer_Flush( &rb_uart_rx );
    os_exit_critical();
}


//void hal_uart_enable(uint8_t enable)
//{
//}


int  shell_driver_init( void )
{
    return hal_uart_init(0);
}


void shell_driver_reset( void )
{
    hal_uart_reset();
}

int shell_driver_read( char *buffer, int len )
{
    (void)buffer;
    (void)len;
    return 0;  /* not supported */
}

int shell_driver_read_char( char *c )
{
    if( hal_uart_getc( c, -1 ) )
        return (int)*c;
    else
        return -1;
}

int  shell_driver_read_char_blocked( char *c, int block_ticks )
{
    if( hal_uart_getc( c, block_ticks ) )
        return (int)*c;
    else
        return -1;
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
        while( hal_uart_putc( *(char*)((int)buffer + written), -1 ) == 0 )
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


