/* uart driver template for MCUSH/lpc
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


#ifndef HAL_UART_DEFINE 
    #define HAL_UARTx                       USART0
    #define HAL_UARTx_IRQn                  USART0_IRQn
    #define HAL_UARTx_IRQHandler            USART0_IRQHandler
    #define HAL_UARTx_CLOCK                 kCLOCK_Uart0
    #define HAL_UARTx_CLOCK_SEL             kUART0_Clk_From_MainClk
#endif

#ifndef HAL_UARTx_BAUDRATE
    #define HAL_UARTx_BAUDRATE              9600
#endif
#ifndef HAL_UART_RX_QUEUE_LEN
    #define HAL_UART_RX_QUEUE_LEN           64
#endif
#ifndef HAL_UART_TX_QUEUE_LEN
    #define HAL_UART_TX_QUEUE_LEN           64
#endif

  
os_queue_handle_t hal_uart_queue_rx;
os_queue_handle_t hal_uart_queue_tx;


int hal_uart_init(uint32_t baudrate)
{
#if OS_SUPPORT_STATIC_ALLOCATION
#if HAL_UART_RX_QUEUE_LEN
    DEFINE_STATIC_QUEUE_BUFFER( uart_rx, HAL_UART_RX_QUEUE_LEN, 1 );
    hal_uart_queue_rx = os_queue_create_static( "rxQ", HAL_UART_RX_QUEUE_LEN, 1,
                                            &static_queue_buffer_uart_rx );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
#if HAL_UART_TX_QUEUE_LEN
    DEFINE_STATIC_QUEUE_BUFFER( uart_tx, HAL_UART_TX_QUEUE_LEN, 1 );
    hal_uart_queue_tx = os_queue_create_static( "txQ", HAL_UART_TX_QUEUE_LEN, 1,
                                            &static_queue_buffer_uart_tx );
    if( hal_uart_queue_tx == NULL )
        return 0;
#endif
#else
#if HAL_UART_RX_QUEUE_LEN
    hal_uart_queue_rx = os_queue_create( "rxQ", HAL_UART_RX_QUEUE_LEN, 1 );
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif
#if HAL_UART_TX_QUEUE_LEN
    hal_uart_queue_tx = os_queue_create( "txQ", HAL_UART_TX_QUEUE_LEN, 1 );
    if( hal_uart_queue_tx == NULL )
        return 0;
#endif
#endif
  
    CLOCK_Select( HAL_UARTx_CLOCK_SEL );

    usart_config_t config;
    config.baudRate_Bps = (baudrate==0) ? HAL_UARTx_BAUDRATE : baudrate;
    config.parityMode = kUSART_ParityDisabled;
    config.stopBitCount = kUSART_OneStopBit;
    config.bitCountPerChar = kUSART_8BitsPerChar;
    config.loopback = false;
    config.enableRx = true;
    config.enableTx = true;
    config.syncMode = kUSART_SyncModeDisabled;
    USART_Init( HAL_UARTx, &config, SystemCoreClock );
    USART_EnableInterrupts( HAL_UARTx, kUSART_RxReadyInterruptEnable | kUSART_TxReadyInterruptEnable );

    NVIC_SetPriority( HAL_UARTx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), \
                    //configLIBRARY_LOWEST_INTERRUPT_PRIORITY, 0));
                    7, 0));
    NVIC_EnableIRQ(HAL_UARTx_IRQn);
    return 1;
}



int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_TX_QUEUE_LEN
    if( os_queue_put( hal_uart_queue_tx, &c, block_ticks ) )
    {
        USART_EnableInterrupts( HAL_UARTx, kUSART_TxReadyInterruptEnable );
        return 1;
    }
    else
#endif
        return 0;
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
    int status = USART_GetStatusFlags( HAL_UARTx );
    char c;

    if( status & kUSART_TxIdleFlag )
    {
#if HAL_UART_TX_QUEUE_LEN
        if( os_queue_get_isr( hal_uart_queue_tx, &c ) )
        {
            USART_WriteByte( HAL_UARTx, c );
        }
        else
#endif
        {
            USART_DisableInterrupts( HAL_UARTx, kUSART_TxReadyInterruptEnable );
        }       
    }
    
    if( status & kUSART_RxReady )
    {
        c = USART_ReadByte( HAL_UARTx );
#if HAL_UART_RX_QUEUE_LEN
        os_queue_put_isr( hal_uart_queue_rx, &c );
#endif
        USART_ClearStatusFlags( HAL_UARTx, kUSART_RxReady );
    }

    if( status & kUSART_HardwareOverrunFlag )
    {
        USART_ReadByte( HAL_UARTx );
        USART_ClearStatusFlags( HAL_UARTx, kUSART_HardwareOverrunFlag );
    }

}


void hal_uart_reset(void)
{
#if HAL_UART_RX_QUEUE_LEN
    os_queue_reset( hal_uart_queue_rx );
#endif
#if HAL_UART_TX_QUEUE_LEN
    os_queue_reset( hal_uart_queue_tx );
#endif
}


//void hal_uart_enable(uint8_t enable)
//{
//}


int  shell_driver_init( void )
{
    return 1;  /* already inited */
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
        return (int)c;
    else
        return -1;
}

int  shell_driver_read_char_blocked( char *c, int block_ticks )
{
    if( hal_uart_getc( c, block_ticks ) )
        return (int)c;
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


