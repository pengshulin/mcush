#include "mcush.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"


extern uint8_t  USART_Rx_Buffer[USART_RX_DATA_SIZE]; 
extern uint32_t USART_Rx_ptr_in;
extern uint32_t USART_Rx_ptr_out;
extern uint32_t USART_Rx_length;


#define HAL_VCP_QUEUE_RX_LEN    128
#define HAL_VCP_QUEUE_TX_LEN    128

QueueHandle_t hal_vcp_queue_rx;
QueueHandle_t hal_vcp_queue_tx;

#if configSUPPORT_STATIC_ALLOCATION
StaticQueue_t hal_vcp_queue_rx_data;
uint8_t hal_vcp_queue_rx_buffer[HAL_VCP_QUEUE_RX_LEN];
StaticQueue_t hal_vcp_queue_tx_data;
uint8_t hal_vcp_queue_tx_buffer[HAL_VCP_QUEUE_TX_LEN];
#endif


signed portBASE_TYPE hal_vcp_putc( char c, TickType_t xBlockTime )
{
    if( xQueueSend( hal_vcp_queue_tx, &c, xBlockTime ) == pdPASS )
    {
        return pdPASS;
    }
    else
        return pdFAIL;
}

signed portBASE_TYPE hal_vcp_getc( char *c, TickType_t xBlockTime )
{
    return xQueueReceive( hal_vcp_queue_rx, c, xBlockTime );
}


void hal_vcp_reset(void)
{
    xQueueReset( hal_vcp_queue_rx );
    xQueueReset( hal_vcp_queue_tx );
}


void hal_vcp_enable(uint8_t enable)
{
}


int hal_uart_init(uint32_t baudrate)
{
#if configSUPPORT_STATIC_ALLOCATION
    hal_vcp_queue_rx = xQueueCreateStatic( HAL_VCP_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(signed char),
                                           hal_vcp_queue_rx_buffer, &hal_vcp_queue_rx_data );
    hal_vcp_queue_tx = xQueueCreateStatic( HAL_VCP_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(signed char),
                                           hal_vcp_queue_tx_buffer, &hal_vcp_queue_tx_data );
#else
    hal_vcp_queue_rx = xQueueCreate( HAL_VCP_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(signed char) );
    hal_vcp_queue_tx = xQueueCreate( HAL_VCP_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(signed char) );
#endif
    if( !hal_vcp_queue_rx || !hal_vcp_queue_tx )
        return 0;

    Get_SerialNum();  /* Serial Number from Chip UID */
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    return 1;
}


int  shell_driver_init( void )
{
    return hal_uart_init(0);
}


void shell_driver_reset( void )
{
    hal_vcp_reset();
}


int  shell_driver_read( char *buffer, int len )
{
    return 0;  /* not supported */
}


int  shell_driver_read_char( char *c )
{
    if( hal_vcp_getc( c, portMAX_DELAY ) == pdFAIL )
        return -1;
    else
        return (int)*c;
}


int  shell_driver_read_char_blocked( char *c, int block_time )
{
    if( hal_vcp_getc( c, block_time ) == pdFAIL )
        return -1;
    else
        return (int)*c;
}


int  shell_driver_read_is_empty( void )
{
    return 1;
}

#define WRITE_RETRY         5
#define WRITE_TIMEOUT_MS    1000
#define WRITE_TIMEOUT_TICK  (WRITE_TIMEOUT_MS*configTICK_RATE_HZ/1000)

int  shell_driver_write( const char *buffer, int len )
{
    int written=0;
    int retry;

    while( written < len )
    {
        retry = 0;
        while( hal_vcp_putc( *(char*)((int)buffer + written), WRITE_TIMEOUT_TICK ) == pdFAIL )
        {
            vTaskDelay(1);
            retry++;
            if( retry >= WRITE_RETRY )
                return written;
        }
        written += 1;
    }
    return written;
}


void shell_driver_write_char( char c )
{
    int retry=0;
    
    while( hal_vcp_putc( c, WRITE_TIMEOUT_TICK ) == pdFAIL )
    {
        vTaskDelay(1);
        retry++;
        if( retry >= WRITE_RETRY )
            return;
    }
}

