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

#define HAL_VCP_QUEUE_RX_LEN   128  /* memory consumption: RX_LEN x 2 */
#define HAL_VCP_QUEUE_TX_LEN   63  /* memory consumption: TX_LEN x 3 */

#define VCP_WRITE_BLOCK_AUTO_ADAPTED  1
#define VCP_WRITE_BLOCK_AUTO_ADAPTED_BLOCK_TICK  (1000*configTICK_RATE_HZ/1000)


USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

char hal_vcp_rx_buf[HAL_VCP_QUEUE_RX_LEN];
char hal_vcp_tx_buf1[HAL_VCP_QUEUE_TX_LEN];
char hal_vcp_tx_buf2[HAL_VCP_QUEUE_TX_LEN];
int hal_vcp_tx_buf1_len, hal_vcp_tx_buf2_len;
int8_t hal_vcp_tx_use_buf2;
QueueHandle_t hal_vcp_queue_rx;
QueueHandle_t hal_vcp_queue_tx;
SemaphoreHandle_t hal_vcp_sem_tx;
#if configSUPPORT_STATIC_ALLOCATION
StaticQueue_t hal_vcp_queue_rx_data;
uint8_t hal_vcp_queue_rx_buffer[HAL_VCP_QUEUE_RX_LEN];
StaticQueue_t hal_vcp_queue_tx_data;
uint8_t hal_vcp_queue_tx_buffer[HAL_VCP_QUEUE_TX_LEN];
StaticSemaphore_t hal_vcp_sem_tx_data;
StaticTask_t hal_vcp_tx_task_data;
StackType_t hal_vcp_tx_task_buffer[TASK_VCP_TX_STACK_SIZE/sizeof(portSTACK_TYPE)];
#endif



/* NOTE: call this hook in ST/USB_Device_Library/usbd_cdc.c/USBD_CDC_DataIn function */
void hal_vcp_tx_done_isr_hook(void)
{
    xSemaphoreGiveFromISR( hal_vcp_sem_tx, 0 );
}


void task_vcp_tx_entry(void *p)
{
    char c;
    char *next_buf;
    int *next_buf_len;
    TickType_t timeout;

    while(1)
    {
        next_buf = hal_vcp_tx_use_buf2 ? hal_vcp_tx_buf2 : hal_vcp_tx_buf1;
        next_buf_len = hal_vcp_tx_use_buf2 ? &hal_vcp_tx_buf2_len : &hal_vcp_tx_buf1_len;
        
        /* get the first item from the queue */ 
        if( xQueueReceive( hal_vcp_queue_tx, &c, portMAX_DELAY ) != pdPASS )
            continue;
        timeout = xTaskGetTickCount() + TASK_VCP_TX_READ_TIMEOUT_TICK;
        *next_buf = c;
        *next_buf_len = 1;

        /* read as much as possible from the queue with time limit */
        while( *next_buf_len < HAL_VCP_QUEUE_TX_LEN )
        {
            if( xQueueReceive( hal_vcp_queue_tx, &c, TASK_VCP_TX_READ_TIMEOUT_TICK ) == pdPASS )
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
            if( xSemaphoreTake( hal_vcp_sem_tx, portMAX_DELAY ) == pdTRUE )
            {
                if( CDC_Transmit_FS( (uint8_t*)next_buf, *next_buf_len ) == USBD_OK )
                {
                    break;
                }
                else
                {
                    xSemaphoreGive( hal_vcp_sem_tx );
                    //taskYIELD();
                    vTaskDelay(1);
                }
            }
            else
                //taskYIELD();
                vTaskDelay(1);
        }

        /* switch to next bank */
        hal_vcp_tx_use_buf2 = hal_vcp_tx_use_buf2 ? 0 : 1;
    }
}


int hal_uart_init(uint32_t baudrate)
{
    TaskHandle_t task_vcp_tx;

#if configSUPPORT_STATIC_ALLOCATION
    hal_vcp_queue_rx = xQueueCreateStatic( HAL_VCP_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(signed char),
                                           hal_vcp_queue_rx_buffer, &hal_vcp_queue_rx_data );
    hal_vcp_queue_tx = xQueueCreateStatic( HAL_VCP_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(signed char),
                                           hal_vcp_queue_tx_buffer, &hal_vcp_queue_tx_data );
    hal_vcp_sem_tx = xSemaphoreCreateBinaryStatic(&hal_vcp_sem_tx_data);
#else
    hal_vcp_queue_rx = xQueueCreate( HAL_VCP_QUEUE_RX_LEN, (unsigned portBASE_TYPE)sizeof(signed char) );
    hal_vcp_queue_tx = xQueueCreate( HAL_VCP_QUEUE_TX_LEN, (unsigned portBASE_TYPE)sizeof(signed char) );
    hal_vcp_sem_tx = xSemaphoreCreateBinary();
#endif
    if( !hal_vcp_queue_rx || !hal_vcp_queue_tx || !hal_vcp_sem_tx )
        return 0;
    xSemaphoreGive(hal_vcp_sem_tx);
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;

    /* create vcp/tx task */
#if configSUPPORT_STATIC_ALLOCATION
    task_vcp_tx = xTaskCreateStatic(task_vcp_tx_entry, (const char *)"vcp/txT", 
                TASK_VCP_TX_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_VCP_TX_PRIORITY, hal_vcp_tx_task_buffer, &hal_vcp_tx_task_data);
#else
    xTaskCreate(task_vcp_tx_entry, (const char *)"vcp/txT", 
                TASK_VCP_TX_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_VCP_TX_PRIORITY, &task_vcp_tx);
#endif
    if( task_vcp_tx == NULL )
        halt("create vcp/tx task");

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
    xQueueReset( hal_vcp_queue_rx );
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;

}


int  shell_driver_read( char *buffer, int len )
{
    int bytes=0;
    while( bytes < len )
    {
        if( xQueueReceive( hal_vcp_queue_rx, buffer, portMAX_DELAY ) == pdPASS )
        {
            buffer++;
            bytes++;
        }
    }
    return bytes;
}


int  shell_driver_read_char( char *c )
{
    if( xQueueReceive( hal_vcp_queue_rx, c, portMAX_DELAY ) != pdPASS )
        return -1;
    else
        return (int)c;
}


int  shell_driver_read_char_blocked( char *c, int block_time )
{
    if( xQueueReceive( hal_vcp_queue_rx, c, block_time ) != pdPASS )
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
#if VCP_WRITE_BLOCK_AUTO_ADAPTED
    /* auto adapt, ignore short-time continuous data if connection broken and queue full */
    static uint8_t broken=0;
    while( written < len )
    {
        if( broken )
        {
            if( xQueueSend( hal_vcp_queue_tx, (void*)(buffer), 0 ) == pdPASS )
                broken = 0; 
        } 
        else
        {
            if( xQueueSend( hal_vcp_queue_tx, (void*)(buffer), VCP_WRITE_BLOCK_AUTO_ADAPTED_BLOCK_TICK ) != pdPASS )
                broken = 1; 
        }
        written ++;
        buffer ++;
    }
#else
    /* always blocked, this will make the running task blocked (if connection is broken) */
    while( written < len )
    {
        if( xQueueSend( hal_vcp_queue_tx, (void*)(buffer), portMAX_DELAY ) == pdPASS )
        {
            written ++;
            buffer ++;
        }
    }
#endif
    return written;
}


void shell_driver_write_char( char c )
{
    shell_driver_write( (const char*)&c, 1 );
}

