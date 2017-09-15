#if defined(MCUSH_NON_OS)
#include "mcush.h"

#define UART_RX_LEN    128

char uart_rx_buf[UART_RX_LEN];
int uart_rx_idx, uart_rx_size;

void hal_uart_putc( char c )
{
    while( USART_GetFlagStatus( USART1, USART_FLAG_TC ) != SET );
    USART_SendData( USART1, c );
    while( USART_GetFlagStatus( USART1, USART_FLAG_TC ) != SET );
}

int hal_uart_getc( char *c )
{
    if( uart_rx_size )
    {
        *c = uart_rx_buf[(uart_rx_idx + UART_RX_LEN - uart_rx_size--)% UART_RX_LEN];
        return 1;
    }
    return 0;
}

void USART1_IRQHandler(void)
{
    char c;

    if( USART_GetITStatus( USART1, USART_IT_TXE ) == SET )
    {
        USART_ClearITPendingBit( USART1, USART_IT_TXE );
    }

    if( USART_GetITStatus( USART1, USART_IT_RXNE ) == SET )
    {
        c = USART_ReceiveData( USART1 );
        if( uart_rx_size < UART_RX_LEN )
        {
            uart_rx_buf[uart_rx_idx] = c;
            uart_rx_size += 1;
            uart_rx_idx = (uart_rx_idx+1) % UART_RX_LEN;
            set_event( event_mcush, EVT_MCUSH_CHAR );     
        }
        USART_ClearITPendingBit( USART1, USART_IT_RXNE );
    }   

    if( USART_GetITStatus( USART1, USART_IT_ORE_RX ) == SET )
    {
        USART_ReceiveData( USART1 );
        USART_ClearITPendingBit( USART1, USART_IT_ORE_RX );
    }

}


void hal_uart_reset(void)
{
    uart_rx_idx = uart_rx_size = 0;
}


void hal_uart_enable(uint8_t enable)
{
}

int hal_uart_init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    USART_ClearFlag( USART1, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TC | USART_FLAG_RXNE );	
    USART_ITConfig( USART1, USART_IT_CTS | USART_IT_LBD | USART_IT_TXE | USART_IT_TC | \
                  USART_IT_RXNE | USART_IT_IDLE | USART_IT_PE | USART_IT_ERR, DISABLE );
 
    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART Rx/Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* Connect PXx to USARTx TXD */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    /* Connect PXx to USARTx RXD */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

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
    if( hal_uart_getc( c ) )
        return (int)c;
    return -1;
}

int  shell_driver_read_char_blocked( char *c, int block_time )
{
    int t0 = get_sys_tick_count(); 
    while( get_sys_tick_count() <= t0 + block_time )
    {
        if( hal_uart_getc( c ) )
            return (int)c;
    }
    return -1;
}

int  shell_driver_read_is_empty( void )
{
    return uart_rx_size ? 0 : 1;
}

int  shell_driver_write( const char *buffer, int len )
{
    int written=0;

    while( written < len )
    {
        hal_uart_putc( *(char*)((int)buffer + written) );
        written += 1;
    }
    return written;
}

void shell_driver_write_char( char c )
{
    hal_uart_putc( c );
}

#endif
