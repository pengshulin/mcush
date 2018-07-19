#include "mcush.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)                  (PC)
   PA9  USART1_TX ------> RXD
   PA10 USART1_RX <-----> TXD 
   ----------------------------- 
 */

#define RCC_GPIO_ENABLE_CMD   RCC_AHB1PeriphClockCmd
#define RCC_GPIO_ENABLE_BIT   RCC_AHB1Periph_GPIOA
#define RCC_USART_ENABLE_CMD  RCC_APB2PeriphClockCmd
#define RCC_USART_ENABLE_BIT  RCC_APB2Periph_USART1
#define USARTx                USART1
#define USARTx_TX_PORT        GPIOA
#define USARTx_TX_PIN         GPIO_Pin_9
#define USARTx_TX_PINSRC      GPIO_PinSource9
#define USARTx_RX_PORT        GPIOA
#define USARTx_RX_PIN         GPIO_Pin_10
#define USARTx_RX_PINSRC      GPIO_PinSource10
#define USARTx_AF             GPIO_AF_USART1
#define USARTx_IRQn           USART1_IRQn
#define USARTx_IRQHandler     USART1_IRQHandler

 
#define QUEUE_UART_RX_LEN    128
#define QUEUE_UART_TX_LEN    128
QueueHandle_t hal_queue_uart_rx;
QueueHandle_t hal_queue_uart_tx;

signed portBASE_TYPE hal_uart_putc( char c, TickType_t xBlockTime )
{

    if( xQueueSend( hal_queue_uart_tx, &c, xBlockTime ) == pdPASS )
    {
        USART_ITConfig( USARTx, USART_IT_TXE, ENABLE );
        return pdPASS;
    }
    else
        return pdFAIL;
}

signed portBASE_TYPE hal_uart_getc( char *c, TickType_t xBlockTime )
{
    return xQueueReceive( hal_queue_uart_rx, c, xBlockTime );
}

void USARTx_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char c;

    if( USART_GetITStatus( USARTx, USART_IT_TXE ) == SET )
    {
        if( xQueueReceiveFromISR( hal_queue_uart_tx, &c, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            USART_SendData( USARTx, c );
        }
        else
        {
            USART_ITConfig( USARTx, USART_IT_TXE, DISABLE );        
        }       
        USART_ClearITPendingBit( USARTx, USART_IT_TXE );
    }
    
    if( USART_GetITStatus( USARTx, USART_IT_RXNE ) == SET )
    {
        c = USART_ReceiveData( USARTx );
        xQueueSendFromISR( hal_queue_uart_rx, &c, &xHigherPriorityTaskWoken );
        USART_ClearITPendingBit( USARTx, USART_IT_RXNE );
    }   

    if( USART_GetITStatus( USARTx, USART_IT_ORE_RX ) == SET )
    {
        USART_ReceiveData( USARTx );
        //USART_ClearFlag( USARTx, USART_FLAG_ORE );    
        USART_ClearITPendingBit( USARTx, USART_IT_ORE_RX );
    }

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void hal_uart_reset(void)
{
    xQueueReset( hal_queue_uart_rx );
    xQueueReset( hal_queue_uart_tx );
}


void hal_uart_enable(uint8_t enable)
{
    USART_Cmd(USARTx, enable ? ENABLE : DISABLE );
}

int hal_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_init;
    USART_InitTypeDef usart_init;
    NVIC_InitTypeDef nvic_init;

    hal_queue_uart_rx = xQueueCreate( QUEUE_UART_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_uart_tx = xQueueCreate( QUEUE_UART_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    if( !hal_queue_uart_rx || !hal_queue_uart_tx )
        return 0;

    vQueueAddToRegistry( hal_queue_uart_rx, "RxQ" );
    vQueueAddToRegistry( hal_queue_uart_tx, "TxQ" );
 
    USART_ClearFlag( USARTx, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );	
    USART_ITConfig( USARTx, USART_IT_CTS | USART_IT_LBD | USART_IT_TXE | USART_IT_TC | \
                  USART_IT_RXNE | USART_IT_IDLE | USART_IT_PE | USART_IT_ERR, DISABLE );
 
    /* Enable GPIO clock */
    RCC_GPIO_ENABLE_CMD( RCC_GPIO_ENABLE_BIT, ENABLE );
    /* Enable UART clock */
    RCC_USART_ENABLE_CMD( RCC_USART_ENABLE_BIT, ENABLE );
    /* Configure USART Rx/Tx as alternate function push-pull */
    gpio_init.GPIO_Pin = USARTx_RX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( USARTx_RX_PORT, &gpio_init );
    gpio_init.GPIO_Pin = USARTx_TX_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init( USARTx_TX_PORT, &gpio_init );
    /* Connect AF */
    GPIO_PinAFConfig( USARTx_RX_PORT, USARTx_RX_PINSRC, USARTx_AF );
    GPIO_PinAFConfig( USARTx_TX_PORT, USARTx_TX_PINSRC, USARTx_AF );
    /* USART configuration */
    usart_init.USART_BaudRate = baudrate;
    usart_init.USART_WordLength = USART_WordLength_8b;
    usart_init.USART_StopBits = USART_StopBits_1;
    usart_init.USART_Parity = USART_Parity_No;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USARTx, &usart_init );
    /* Enable USART */
    USART_Cmd( USARTx, ENABLE );
    USART_ClearFlag( USARTx, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );   
    USART_ITConfig( USARTx, USART_IT_RXNE, ENABLE );

    /* Interrupt Enable */  
    nvic_init.NVIC_IRQChannel = USARTx_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 10;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &nvic_init );

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

