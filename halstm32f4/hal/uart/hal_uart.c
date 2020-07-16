/* uart driver template for MCUSH/stm32 
   (change the uart port)
1. define HAL_UART_DEFINE
2. define other HAL_UARTxxxxxxx configurations
   (port the driver to other module)
1. search and replace HAL_UART wth HAL_XXXX
2. search and replace hal_uart to hal_xxxx
3. remove all shell apis at the bottom
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
    #define HAL_UART_RCC_GPIO_ENABLE_CMD    LL_AHB1_GRP1_EnableClock
    #define HAL_UART_RCC_GPIO_ENABLE_BIT    LL_AHB1_GRP1_PERIPH_GPIOA
    #define HAL_UART_RCC_USART_ENABLE_CMD   LL_APB2_GRP1_EnableClock
    #define HAL_UART_RCC_USART_ENABLE_BIT   LL_APB2_GRP1_PERIPH_USART1
    #define HAL_UARTx                       USART1
    #define HAL_UARTx_TX_PORT               GPIOA
    #define HAL_UARTx_TX_PIN                LL_GPIO_PIN_9
    #define HAL_UARTx_TX_AF                 LL_GPIO_AF_7
    #define HAL_UARTx_RX_PORT               GPIOA
    #define HAL_UARTx_RX_PIN                LL_GPIO_PIN_10
    #define HAL_UARTx_RX_AF                 LL_GPIO_AF_7
    #define HAL_UARTx_IRQn                  USART1_IRQn
    #define HAL_UARTx_IRQHandler            USART1_IRQHandler
    #define HAL_UARTx_BAUDRATE              9600
    #define HAL_UART_QUEUE_RX_LEN           128
    #define HAL_UART_QUEUE_TX_LEN           128
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

    
#if HAL_UART_QUEUE_RX_LEN
os_queue_handle_t hal_uart_queue_rx;
#if OS_SUPPORT_STATIC_ALLOCATION
DEFINE_STATIC_QUEUE_BUFFER( hal_uart_rx, HAL_UART_QUEUE_RX_LEN, 1 );
#endif
#endif

#if HAL_UART_QUEUE_TX_LEN
os_queue_handle_t hal_uart_queue_tx;
#if OS_SUPPORT_STATIC_ALLOCATION
DEFINE_STATIC_QUEUE_BUFFER( hal_uart_tx, HAL_UART_QUEUE_TX_LEN, 1 );
#endif
#endif



int hal_uart_init( uint32_t baudrate )
{
    LL_GPIO_InitTypeDef gpio_init;
    LL_USART_InitTypeDef usart_init;

#if HAL_UART_QUEUE_RX_LEN
#if OS_SUPPORT_STATIC_ALLOCATION
    hal_uart_queue_rx = os_queue_create_static( "rxQ", HAL_UART_QUEUE_RX_LEN, 1, &static_queue_buffer_hal_uart_rx );
#else
    hal_uart_queue_rx = os_queue_create( "rxQ", HAL_UART_QUEUE_RX_LEN, 1 );
#endif
    if( hal_uart_queue_rx == NULL )
        return 0;
#endif

#if HAL_UART_QUEUE_TX_LEN
#if OS_SUPPORT_STATIC_ALLOCATION
    hal_uart_queue_tx = os_queue_create_static( "txQ", HAL_UART_QUEUE_TX_LEN, 1, &static_queue_buffer_hal_uart_tx );
#else
    hal_uart_queue_tx = os_queue_create( "txQ", HAL_UART_QUEUE_TX_LEN, 1 );
#endif
    if( hal_uart_queue_tx == NULL )
        return 0;
#endif

    HAL_UART_RCC_GPIO_ENABLE_CMD( HAL_UART_RCC_GPIO_ENABLE_BIT );
    HAL_UART_RCC_USART_ENABLE_CMD( HAL_UART_RCC_USART_ENABLE_BIT );

    /* Configure USART Rx/Tx as alternate function push-pull */
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pin = HAL_UARTx_TX_PIN;
    gpio_init.Alternate = HAL_UARTx_TX_AF;
    LL_GPIO_Init( HAL_UARTx_TX_PORT, &gpio_init);
    gpio_init.Pin = HAL_UARTx_RX_PIN;
    gpio_init.Pull = LL_GPIO_PULL_UP;
    gpio_init.Alternate = HAL_UARTx_RX_AF;
    LL_GPIO_Init( HAL_UARTx_RX_PORT, &gpio_init);

    /* USART configuration */
    usart_init.BaudRate = baudrate ? baudrate : HAL_UARTx_BAUDRATE;
    usart_init.DataWidth = LL_USART_DATAWIDTH_8B;
    usart_init.StopBits = LL_USART_STOPBITS_1;
    usart_init.Parity = LL_USART_PARITY_NONE;
    usart_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usart_init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart_init.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init( HAL_UARTx, &usart_init );
    LL_USART_Enable( HAL_UARTx );
    LL_USART_ClearFlag_nCTS( HAL_UARTx );
    LL_USART_ClearFlag_TC( HAL_UARTx );
    //LL_USART_ClearFlag_RXNE( HAL_UARTx );
    LL_USART_ClearFlag_PE( HAL_UARTx );
    LL_USART_ClearFlag_FE( HAL_UARTx );
    LL_USART_ClearFlag_NE( HAL_UARTx );
    LL_USART_ClearFlag_ORE( HAL_UARTx );
    LL_USART_EnableIT_RXNE( HAL_UARTx );

    /* Interrupt Enable */  
    HAL_NVIC_SetPriority( HAL_UARTx_IRQn, 10, 0 );
    HAL_NVIC_EnableIRQ( HAL_UARTx_IRQn );
    return 1;
}


void HAL_UARTx_IRQHandler(void)
{
    //portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char c;

#if HAL_UART_QUEUE_TX_LEN
    if( LL_USART_IsEnabledIT_TXE( HAL_UARTx ) && LL_USART_IsActiveFlag_TXE( HAL_UARTx ) )
    {
        if( os_queue_get_isr( hal_uart_queue_tx, &c ) )
        {
            LL_USART_TransmitData8( HAL_UARTx, c );
        }
        else
        {
            LL_USART_DisableIT_TXE( HAL_UARTx );        
        }       
    }
#endif
    
#if HAL_UART_QUEUE_RX_LEN
    if( LL_USART_IsActiveFlag_RXNE( HAL_UARTx ) )
    {
        c = LL_USART_ReceiveData8( HAL_UARTx );
        os_queue_put_isr( hal_uart_queue_rx, &c );
    }
#endif

    if( LL_USART_IsActiveFlag_ORE( HAL_UARTx ) )
    {
        LL_USART_ReceiveData8( HAL_UARTx );
        LL_USART_ClearFlag_ORE( HAL_UARTx );
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
    if( enable )
        LL_USART_Enable( HAL_UARTx );
    else
        LL_USART_Disable( HAL_UARTx );
}


int hal_uart_putc( char c, os_tick_t block_ticks )
{
#if HAL_UART_QUEUE_TX_LEN
    if( os_queue_put( hal_uart_queue_tx, &c, block_ticks ) )
    {
        LL_USART_EnableIT_TXE( HAL_UARTx );        
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
    while( hal_uart_putc( c, portMAX_DELAY ) == 0 )
        os_task_delay(1);
}


