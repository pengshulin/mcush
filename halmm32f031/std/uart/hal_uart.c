/* uart driver template for MCUSH/mm32 
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
   (MCU)                  (PC)
   PA9  USART1_TX ------> RXD
   PA10 USART1_RX <------ TXD 
   ----------------------------- 
 */

#ifndef HAL_UART_DEFINE 
    #define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_AHBPeriphClockCmd
    #define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_AHBPeriph_GPIOA
    #define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB2PeriphClockCmd
    #define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB2Periph_UART1
    #define HAL_UARTx                       UART1
    #define HAL_UARTx_TX_PORT               GPIOA
    #define HAL_UARTx_TX_PIN                GPIO_Pin_9
    #define HAL_UARTx_TX_PINSRC             GPIO_PinSource9
    #define HAL_UARTx_TX_AF                 GPIO_AF_1
    #define HAL_UARTx_RX_PORT               GPIOA
    #define HAL_UARTx_RX_PIN                GPIO_Pin_10
    #define HAL_UARTx_RX_PINSRC             GPIO_PinSource10
    #define HAL_UARTx_RX_AF                 GPIO_AF_1
    #define HAL_UARTx_IRQn                  UART1_IRQn
    #define HAL_UARTx_IRQHandler            UART1_IRQHandler
#endif


#ifndef HAL_UARTx_BAUDRATE
    #define HAL_UARTx_BAUDRATE              9600
#endif
#ifndef HAL_UART_QUEUE_RX_LEN
    #define HAL_UART_QUEUE_RX_LEN           128
#endif
#ifndef HAL_UART_QUEUE_TX_LEN
    #define HAL_UART_QUEUE_TX_LEN           128
#endif


os_queue_handle_t hal_uart_queue_rx, hal_uart_queue_tx;


int hal_uart_init( uint32_t baudrate )
{
    GPIO_InitTypeDef gpio_init;
    UART_InitTypeDef uart_init;
    NVIC_InitTypeDef nvic_init;

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

    //UART_ClearITPendingBit( HAL_UARTx, UART_IT_ERR | UART_IT_PE | UART_OVER_ERR | UART_IT_RXIEN | UART_IT_TXIEN );	
    UART_ITConfig( HAL_UARTx, UART_IT_ERR | UART_IT_PE | UART_OVER_ERR | UART_IT_RXIEN | UART_IT_TXIEN, DISABLE );
 
    /* Enable GPIO clock */
    HAL_UART_RCC_GPIO_ENABLE_CMD( HAL_UART_RCC_GPIO_ENABLE_BIT, ENABLE );
    /* Enable USART clock */
    HAL_UART_RCC_USART_ENABLE_CMD( HAL_UART_RCC_USART_ENABLE_BIT, ENABLE );
    /* Configure USART Rx/Tx as alternate function push-pull */
    gpio_init.GPIO_Pin = HAL_UARTx_RX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;//AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init( HAL_UARTx_RX_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_UARTx_TX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( HAL_UARTx_TX_PORT, &gpio_init );
    /* Connect AF */
    GPIO_PinAFConfig( HAL_UARTx_RX_PORT, HAL_UARTx_RX_PINSRC, HAL_UARTx_RX_AF );
    GPIO_PinAFConfig( HAL_UARTx_TX_PORT, HAL_UARTx_TX_PINSRC, HAL_UARTx_TX_AF );
    /* USART configuration */
    uart_init.BaudRate = baudrate ? baudrate : HAL_UARTx_BAUDRATE;
    uart_init.WordLength = UART_WordLength_8b;
    uart_init.StopBits = UART_StopBits_1;
    uart_init.Parity = UART_Parity_No;
    uart_init.HWFlowControl = UART_HWFlowControl_None;
    uart_init.Mode = UART_Mode_Rx | UART_Mode_Tx;
    UART_Init( HAL_UARTx, &uart_init );
    /* Enable USART */
    UART_Cmd( HAL_UARTx, ENABLE );
    UART_ClearITPendingBit( HAL_UARTx, UART_IT_ERR | UART_IT_PE | UART_OVER_ERR | UART_IT_RXIEN | UART_IT_TXIEN );	
    UART_ITConfig( HAL_UARTx, UART_IT_RXIEN, ENABLE );

    /* Interrupt Enable */  
    nvic_init.NVIC_IRQChannel = HAL_UARTx_IRQn;
    nvic_init.NVIC_IRQChannelPriority = 1;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &nvic_init );

    return 1;
}


void HAL_UARTx_IRQHandler(void)
{
    //portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char c;

    //if( UART_GetITStatus( HAL_UARTx, UART_IT_TXIEN ) == SET )
    if( UART_GetFlagStatus( HAL_UARTx, UART_FLAG_TXEMPTY ) == SET )
    {
#if HAL_UART_QUEUE_TX_LEN
        if( os_queue_get_isr( hal_uart_queue_tx, &c ) )
        {
            UART_SendData( HAL_UARTx, c );
        }
        else
#endif
        {
            UART_ITConfig( HAL_UARTx, UART_IT_TXIEN, DISABLE );        
        }       
    }
    
    if( UART_GetITStatus( HAL_UARTx, UART_IT_RXIEN ) == SET )
    {
        c = UART_ReceiveData( HAL_UARTx );
#if HAL_UART_QUEUE_RX_LEN
        os_queue_put_isr( hal_uart_queue_rx, &c );
#endif
        UART_ClearITPendingBit( HAL_UARTx, UART_IT_RXIEN );
    }

    if( UART_GetITStatus( HAL_UARTx, UART_OVER_ERR ) == SET )
    {
        UART_ReceiveData( HAL_UARTx );
        UART_ClearITPendingBit( HAL_UARTx, UART_OVER_ERR );
    }

    //portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
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
    UART_Cmd(HAL_UARTx, enable ? ENABLE : DISABLE );
}


int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_TX_LEN
    if( UART_GetFlagStatus( HAL_UARTx, UART_FLAG_TXEPT ) == SET )
    {
        UART_SendData( HAL_UARTx, c );
        UART_ITConfig( HAL_UARTx, UART_IT_TXIEN, ENABLE );
        return 1;
    }
    else if( os_queue_put( hal_uart_queue_tx, &c, block_ticks ) )
    {
        UART_ITConfig( HAL_UARTx, UART_IT_TXIEN, ENABLE );
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
    return 1;  /* already inited */
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
        while( hal_uart_feedc( *(char*)((int)buffer + bytes), -1 ) == 0 )
            os_task_delay(1);
        bytes += 1;
    }
    return bytes;
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
        return (int)c;
}


int  shell_driver_read_char_blocked( char *c, int block_ticks )
{
    if( hal_uart_getc( c, block_ticks ) == 0 )
        return -1;
    else
        return (int)c;
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


