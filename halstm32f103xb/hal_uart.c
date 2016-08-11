#include "hal.h"


#define QUEUE_UART_RX_LEN    128
#define QUEUE_UART_TX_LEN    128

QueueHandle_t hal_queue_uart_rx;
QueueHandle_t hal_queue_uart_tx;

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

void USART1_IRQHandler(void)
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

    /* If sending or receiving from a queue has caused a task to unblock, and
    the unblocked task has a priority equal to or higher than the currently 
    running task (the task this ISR interrupted), then xHigherPriorityTaskWoken 
    will have automatically been set to pdTRUE within the queue send or receive 
    function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns 
    directly to the higher priority unblocked task. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void hal_uart_reset(void)
{
    xQueueReset( hal_queue_uart_rx );
    xQueueReset( hal_queue_uart_tx );
}


void hal_uart_enable(uint8_t enable)
{
    USART_Cmd(USART1, enable ? ENABLE : DISABLE );
}


int hal_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    hal_queue_uart_rx = xQueueCreate( QUEUE_UART_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_uart_tx = xQueueCreate( QUEUE_UART_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    if( !hal_queue_uart_rx || !hal_queue_uart_tx )
        return 0;
 
    USART_ClearFlag( USART1, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );	
    USART_ITConfig( USART1, USART_IT_CTS | USART_IT_LBD | USART_IT_TXE | USART_IT_TC | \
                  USART_IT_RXNE | USART_IT_IDLE | USART_IT_PE | USART_IT_ERR, DISABLE );
 
    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART Rx/Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* Connect PXx to USARTx TXD */
    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    /* Connect PXx to USARTx RXD */
    //GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
    /* USART configuration */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
 
    USART_ClearFlag( USART1, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );   
    USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

    /* Interrupt Enable */  
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    return 1;
}





