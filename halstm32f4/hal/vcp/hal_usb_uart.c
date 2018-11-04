/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"


#define TASK_VCP_TX_STACK_SIZE  (300)
#define TASK_VCP_TX_PRIORITY    (MCUSH_PRIORITY)

#define TASK_VCP_TX_READ_TIMEOUT_MS    5  /* timeout for the last byte, actual cycle timeout will be doubled */
#define TASK_VCP_TX_READ_TIMEOUT_TICK  (TASK_VCP_TX_READ_TIMEOUT_MS*configTICK_RATE_HZ/1000)

#define VCP_RX_BUF_LEN   128  /* memory consumption: RX_LEN x 2 */
#define VCP_TX_BUF_LEN   128  /* memory consumption: TX_LEN x 3 */


USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

char hal_vcp_rx_buf[VCP_RX_BUF_LEN];
char hal_vcp_tx_buf1[VCP_TX_BUF_LEN];
char hal_vcp_tx_buf2[VCP_TX_BUF_LEN];
int hal_vcp_tx_buf1_len, hal_vcp_tx_buf2_len;
int8_t hal_vcp_tx_use_buf2;
QueueHandle_t hal_queue_vcp_rx;
QueueHandle_t hal_queue_vcp_tx;
SemaphoreHandle_t hal_sem_vcp_tx;


/* NOTE: call this hook in ST/USB_Device_Library/usbd_cdc.c/USBD_CDC_DataIn function */
void hal_vcp_tx_done_isr_hook(void)
{
    xSemaphoreGiveFromISR( hal_sem_vcp_tx, 0 );
}


void task_vcp_tx_entry(void *p)
{
    char c;
    char *next_buf;
    int *next_buf_len;
    TickType_t timeout;

    while(1)
    {
        next_buf = hal_vcp_tx_use_buf2 ? hal_vcp_tx_buf1 : hal_vcp_tx_buf2;
        next_buf_len = hal_vcp_tx_use_buf2 ? &hal_vcp_tx_buf1_len : &hal_vcp_tx_buf2_len;
        
        /* get the first item from the queue */ 
        if( xQueueReceive( hal_queue_vcp_tx, &c, portMAX_DELAY ) != pdPASS )
            continue;
        timeout = xTaskGetTickCount() + TASK_VCP_TX_READ_TIMEOUT_TICK;
        *next_buf = c;
        *next_buf_len = 1;

        /* read as much as possible from the queue with time limit */
        while( *next_buf_len < VCP_TX_BUF_LEN )
        {
            if( xQueueReceive( hal_queue_vcp_tx, &c, TASK_VCP_TX_READ_TIMEOUT_TICK ) == pdPASS )
            {
                *(next_buf + *next_buf_len) = c;
                *next_buf_len += 1;
            }
            if( xTaskGetTickCount() > timeout )
                break;
        }

        /* try to send */
        while( hUsbDeviceFS.dev_config )
        {
            if( xSemaphoreTake( hal_sem_vcp_tx, portMAX_DELAY ) == pdTRUE )
            {
                if( CDC_Transmit_FS( (uint8_t*)next_buf, *next_buf_len ) == USBD_OK )
                    break;
                else
                {
                    xSemaphoreGive( hal_sem_vcp_tx );
                    taskYIELD();
                }
            }
            else
                taskYIELD();
        }

        /* switch to next bank */
        hal_vcp_tx_use_buf2 = hal_vcp_tx_use_buf2 ? 0 : 1;
    }
}



int hal_uart_init(uint32_t baudrate)
{
    TaskHandle_t task_vcp_tx;

    hal_queue_vcp_rx = xQueueCreate( VCP_RX_BUF_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_queue_vcp_tx = xQueueCreate( VCP_TX_BUF_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    hal_sem_vcp_tx = xSemaphoreCreateBinary();
    if( !hal_queue_vcp_rx || !hal_queue_vcp_tx || !hal_sem_vcp_tx )
        return 0;
    xSemaphoreGive(hal_sem_vcp_tx);
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;

    /* create vcp/tx task */
    (void)xTaskCreate(task_vcp_tx_entry, (const char *)"vcp/txT", 
                TASK_VCP_TX_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_VCP_TX_PRIORITY, &task_vcp_tx);
    if( task_vcp_tx == NULL )
        halt("create vcp/tx task");
#if DEBUG
    mcushTaskAddToRegistered((void*)task_vcp_tx);
#endif

    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
    return 1;
}


int  shell_driver_init( void )
{
    return 1;  /* already inited */
}


void shell_driver_reset( void )
{
    xQueueReset( hal_queue_vcp_rx );
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;

}


int  shell_driver_read( char *buffer, int len )
{
    int bytes=0;
    while( bytes < len )
    {
        if( xQueueReceive( hal_queue_vcp_rx, buffer, portMAX_DELAY ) == pdPASS )
        {
            buffer++;
            bytes++;
        }
    }
    return bytes;
}


int  shell_driver_read_char( char *c )
{
    if( xQueueReceive( hal_queue_vcp_rx, c, portMAX_DELAY ) != pdPASS )
        return -1;
    else
        return (int)c;
}


int  shell_driver_read_char_blocked( char *c, int block_time )
{
    if( xQueueReceive( hal_queue_vcp_rx, c, block_time ) != pdPASS )
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
        if( xQueueSend( hal_queue_vcp_tx, (void*)(buffer), portMAX_DELAY ) == pdPASS )
        {
            written ++;
            buffer ++;
        }
    }
    return written;
}


void shell_driver_write_char( char c )
{
    shell_driver_write( (const char*)&c, 1 );
}

