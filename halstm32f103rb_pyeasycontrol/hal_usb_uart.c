#include "mcush.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"


#if !defined(MCUSH_NON_OS)

extern uint8_t  USART_Rx_Buffer[USART_RX_DATA_SIZE]; 
extern uint32_t USART_Rx_ptr_in;
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;


#define QUEUE_UART_RX_LEN    128
#define QUEUE_UART_TX_LEN    128

QueueHandle_t hal_queue_uart_rx;
QueueHandle_t hal_queue_uart_tx;

signed portBASE_TYPE hal_uart_putc( char c, TickType_t xBlockTime )
{

    if( xQueueSend( hal_queue_uart_tx, &c, xBlockTime ) == pdPASS )
    {
        return pdPASS;
    }
    else
        return pdFAIL;
}

signed portBASE_TYPE hal_uart_getc( char *c, TickType_t xBlockTime )
{
    return xQueueReceive( hal_queue_uart_rx, c, xBlockTime );
}


void hal_uart_reset(void)
{
    xQueueReset( hal_queue_uart_rx );
    xQueueReset( hal_queue_uart_tx );
}


void hal_uart_enable(uint8_t enable)
{
}


int hal_uart_init(uint32_t baudrate)
{
    hal_queue_uart_rx = xQueueCreate( QUEUE_UART_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_uart_tx = xQueueCreate( QUEUE_UART_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    if( !hal_queue_uart_rx || !hal_queue_uart_tx )
        return 0;

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
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

#endif

