#include "mcush.h"
#include "ring_buffer.h"


#define QUEUE_UART_RX_LEN    128
//#define QUEUE_UART_TX_LEN    128

QueueHandle_t hal_queue_uart_rx;
//QueueHandle_t hal_queue_uart_tx;

RINGBUFF_T rb_uart_tx, rb_uart_rx;
#define RB_UART_TX_LEN  128
#define RB_UART_RX_LEN  128
char buffer_uart_tx[RB_UART_TX_LEN], buffer_uart_rx[RB_UART_RX_LEN];


signed portBASE_TYPE hal_uart_putc( char c, TickType_t xBlockTime )
{

    if( xQueueSend( hal_queue_uart_tx, &c, xBlockTime ) == pdPASS )
    {
        USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
        return pdPASS;
    }
    else
        return pdFAIL;
}

signed portBASE_TYPE hal_uart_getc( char *c, TickType_t xBlockTime )
{
    return xQueueReceive( hal_queue_uart_rx, c, xBlockTime );
}

void USART3_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char c;

    if( USART_GetITStatus( USART1, USART_IT_TXE ) == SET )
    {
        if( xQueueReceiveFromISR( hal_queue_uart_tx, &c, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            USART_SendData( USART1, c );
        }
        else
        {
            USART_ITConfig( USART1, USART_IT_TXE, DISABLE );        
        }       
        USART_ClearITPendingBit( USART1, USART_IT_TXE );
    }
    
    if( USART_GetITStatus( USART1, USART_IT_RXNE ) == SET )
    {
        c = USART_ReceiveData( USART1 );
        xQueueSendFromISR( hal_queue_uart_rx, &c, &xHigherPriorityTaskWoken );
        USART_ClearITPendingBit( USART1, USART_IT_RXNE );
    }   

    if( USART_GetITStatus( USART1, USART_IT_ORE_RX ) == SET )
    {
        USART_ReceiveData( USART1 );
        //USART_ClearFlag( USART1, USART_FLAG_ORE );    
        USART_ClearITPendingBit( USART1, USART_IT_ORE_RX );
    }

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void hal_uart_reset(void)
{
    xQueueReset( hal_queue_uart_rx );
    //xQueueReset( hal_queue_uart_tx );
    taskENTER_CRITICAL();
    RingBuffer_Flush( &rb_uart_tx );
    RingBuffer_Flush( &rb_uart_rx );
    taskEXIT_CRITICAL();
}


void hal_uart_enable(uint8_t enable)
{
    USART_Cmd(USART1, enable ? ENABLE : DISABLE );
}

int hal_uart_init(uint32_t baudrate)
{

    RingBuffer_Init( &rb_uart_tx, (void*)buffer_uart_tx, 1, RB_UART_TX_LEN );
    RingBuffer_Init( &rb_uart_rx, (void*)buffer_uart_rx, 1, RB_UART_RX_LEN );

    hal_queue_uart_rx = xQueueCreate( QUEUE_UART_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_uart_tx = xQueueCreate( QUEUE_UART_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    if( !hal_queue_uart_rx || !hal_queue_uart_tx )
        return 0;
 
    NVIC_EnableIRQ(USART3_IRQn);
    return 1;
}




int  shell_driver_init( void )
{
    return 1;  /* already inited */
}


void shell_driver_reset( void )
{
    hal_uart_reset();
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

int  shell_driver_read_char_blocked( char *c, int block_time )
{
    if( hal_uart_getc( c, block_time ) == pdFAIL )
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
        while( hal_uart_putc( *(char*)((int)buffer + written) , portMAX_DELAY ) == pdFAIL )
            vTaskDelay(1);
        written += 1;
    }
    return written;
}

void shell_driver_write_char( char c )
{
    while( hal_uart_putc( c, portMAX_DELAY ) == pdFAIL )
        vTaskDelay(1);
}


