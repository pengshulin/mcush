/* uart driver template for MCUSH/stm32 
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
   PA10 USART1_RX <-----> TXD 
   ----------------------------- 
 */

#ifndef HAL_UART_DEFINE 
    #define HAL_UART_RCC_GPIO_ENABLE_CMD    RCC_AHBPeriphClockCmd
    #define HAL_UART_RCC_GPIO_ENABLE_BIT    RCC_AHBPeriph_GPIOA
    #define HAL_UART_RCC_USART_ENABLE_CMD   RCC_APB2PeriphClockCmd
    #define HAL_UART_RCC_USART_ENABLE_BIT   RCC_APB2Periph_USART1
    #define HAL_UARTx                       USART1
    #define HAL_UARTx_TX_PORT               GPIOA
    #define HAL_UARTx_TX_PIN                GPIO_Pin_9
    #define HAL_UARTx_TX_PINSRC             GPIO_PinSource9
    #define HAL_UARTx_TX_AF                 GPIO_AF_1
    #define HAL_UARTx_RX_PORT               GPIOA
    #define HAL_UARTx_RX_PIN                GPIO_Pin_10
    #define HAL_UARTx_RX_PINSRC             GPIO_PinSource10
    #define HAL_UARTx_RX_AF                 GPIO_AF_1
    #define HAL_UARTx_IRQn                  USART1_IRQn
    #define HAL_UARTx_IRQHandler            USART1_IRQHandler
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
#ifndef HAL_UART_QUEUE_ADD_TO_REG
    #define HAL_UART_QUEUE_ADD_TO_REG       1
#endif

os_queue_handle_t hal_uart_queue_rx, hal_uart_queue_tx;


int hal_uart_init( uint32_t baudrate )
{
    GPIO_InitTypeDef gpio_init;
    USART_InitTypeDef usart_init;
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

    USART_ClearFlag( HAL_UARTx, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );	
    USART_ITConfig( HAL_UARTx, USART_IT_CTS | USART_IT_LBD | USART_IT_TXE | USART_IT_TC | \
                    USART_IT_RXNE | USART_IT_IDLE | USART_IT_PE | USART_IT_ERR, DISABLE );
 
    /* Enable GPIO clock */
    HAL_UART_RCC_GPIO_ENABLE_CMD( HAL_UART_RCC_GPIO_ENABLE_BIT, ENABLE );
    /* Enable USART clock */
    HAL_UART_RCC_USART_ENABLE_CMD( HAL_UART_RCC_USART_ENABLE_BIT, ENABLE );
    /* Configure USART Rx/Tx as alternate function push-pull */
    gpio_init.GPIO_Pin = HAL_UARTx_RX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_Speed = GPIO_Speed_Level_3;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init( HAL_UARTx_RX_PORT, &gpio_init );
    gpio_init.GPIO_Pin = HAL_UARTx_TX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init( HAL_UARTx_TX_PORT, &gpio_init );
    /* Connect AF */
    GPIO_PinAFConfig( HAL_UARTx_RX_PORT, HAL_UARTx_RX_PINSRC, HAL_UARTx_RX_AF );
    GPIO_PinAFConfig( HAL_UARTx_TX_PORT, HAL_UARTx_TX_PINSRC, HAL_UARTx_TX_AF );
    /* USART configuration */
    usart_init.USART_BaudRate = baudrate ? baudrate : HAL_UARTx_BAUDRATE;
    usart_init.USART_WordLength = USART_WordLength_8b;
    usart_init.USART_StopBits = USART_StopBits_1;
    usart_init.USART_Parity = USART_Parity_No;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( HAL_UARTx, &usart_init );
    /* Enable USART */
    USART_Cmd( HAL_UARTx, ENABLE );
    USART_ClearFlag( HAL_UARTx, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );   
    USART_ITConfig( HAL_UARTx, USART_IT_RXNE, ENABLE );

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

    if( USART_GetITStatus( HAL_UARTx, USART_IT_TXE ) == SET )
    {
#if HAL_UART_QUEUE_TX_LEN
        if( os_queue_get_isr( hal_uart_queue_tx, &c ) )
        {
            USART_SendData( HAL_UARTx, c );
        }
        else
#endif
        {
            USART_ITConfig( HAL_UARTx, USART_IT_TXE, DISABLE );        
        }       
    }
    
    if( USART_GetITStatus( HAL_UARTx, USART_IT_RXNE ) == SET )
    {
        c = USART_ReceiveData( HAL_UARTx );
#if HAL_UART_QUEUE_RX_LEN
        os_queue_put_isr( hal_uart_queue_rx, &c );
#endif
        USART_ClearITPendingBit( HAL_UARTx, USART_IT_RXNE );
    }   

    if( USART_GetITStatus( HAL_UARTx, USART_IT_ORE ) == SET )
    {
        USART_ReceiveData( HAL_UARTx );
        USART_ClearITPendingBit( HAL_UARTx, USART_IT_ORE );
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
    USART_Cmd(HAL_UARTx, enable ? ENABLE : DISABLE );
}


int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_TX_LEN
    if( os_queue_put( hal_uart_queue_tx, &c, block_ticks ) )
    {
        USART_ITConfig( HAL_UARTx, USART_IT_TXE, ENABLE );
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
        while( hal_uart_feedc( *(char*)((int)buffer + bytes), portMAX_DELAY ) == 0 )
            os_task_delay(1);
        bytes += 1;
    }
    return bytes;
}


int  shell_driver_read( char *buffer, int len )
{
    return 0;  /* not supported */
}


int  shell_driver_read_char( char *c )
{
    if( hal_uart_getc( c, portMAX_DELAY ) == pdFAIL )
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


