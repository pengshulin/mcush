/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"


#define TASK_VCP_TX_STACK_SIZE  310
#define TASK_VCP_TX_PRIORITY    (MCUSH_PRIORITY)

#define TASK_VCP_TX_READ_TIMEOUT_MS    5  /* timeout for the last byte, actual cycle timeout will be doubled */
#define TASK_VCP_TX_READ_TIMEOUT_TICK  OS_TICKS_MS(5)

#define HAL_VCP_RX_QUEUE_LEN   128  /* memory consumption: RX_LEN x 2 */
#define HAL_VCP_TX_QUEUE_LEN   63  /* memory consumption: TX_LEN x 3 */

#define VCP_WRITE_BLOCK_AUTO_ADAPTED  1
#define VCP_WRITE_BLOCK_AUTO_ADAPTED_BLOCK_TICK  OS_TICKS_MS(1000)


USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_DescriptorsTypeDef FS_Desc;
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

char hal_vcp_rx_buf[HAL_VCP_RX_QUEUE_LEN];
char hal_vcp_tx_buf1[HAL_VCP_TX_QUEUE_LEN];
char hal_vcp_tx_buf2[HAL_VCP_TX_QUEUE_LEN];
int hal_vcp_tx_buf1_len, hal_vcp_tx_buf2_len;
int8_t hal_vcp_tx_use_buf2;

os_queue_handle_t hal_vcp_rx_queue;
os_queue_handle_t hal_vcp_tx_queue;
os_semaphore_handle_t hal_vcp_tx_sem;


/* NOTE: call this hook in ST/USB_Device_Library/usbd_cdc.c/USBD_CDC_DataIn function */
void hal_vcp_tx_done_isr_hook(void)
{
    os_semaphore_put_isr( hal_vcp_tx_sem );
}


void task_vcp_tx_entry(void *p)
{
    char c;
    char *next_buf;
    int *next_buf_len;
    os_tick_t timeout;

    (void)p;

    while(1)
    {
        next_buf = hal_vcp_tx_use_buf2 ? hal_vcp_tx_buf2 : hal_vcp_tx_buf1;
        next_buf_len = hal_vcp_tx_use_buf2 ? &hal_vcp_tx_buf2_len : &hal_vcp_tx_buf1_len;
        
        /* get the first item from the queue */ 
        if( os_queue_get( hal_vcp_tx_queue, &c, -1 ) == 0 )
            continue;
        timeout = os_tick() + TASK_VCP_TX_READ_TIMEOUT_TICK;
        *next_buf = c;
        *next_buf_len = 1;

        /* read as much as possible from the queue with time limit */
        while( *next_buf_len < HAL_VCP_TX_QUEUE_LEN )
        {
            if( os_queue_get( hal_vcp_tx_queue, &c, TASK_VCP_TX_READ_TIMEOUT_TICK ) )
            {
                *(next_buf + *next_buf_len) = c;
                *next_buf_len += 1;
            }
            if( os_tick() > timeout )
                break;
        }

        /* try to send */
        while( hUsbDeviceFS.dev_config )
        {
            if( os_semaphore_get( hal_vcp_tx_sem, -1 ) )
            {
                if( CDC_Transmit_FS( (uint8_t*)next_buf, *next_buf_len ) == USBD_OK )
                {
                    break;
                }
                else
                {
                    os_semaphore_put( hal_vcp_tx_sem );
                    //os_task_switch();
                    os_task_delay(1);
                }
            }
            else
                //os_task_switch();
                os_task_delay(1);
        }

        /* switch to next bank */
        hal_vcp_tx_use_buf2 = hal_vcp_tx_use_buf2 ? 0 : 1;
    }
}


int hal_uart_init(uint32_t baudrate)
{
    os_task_handle_t task;

    (void)baudrate;
#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_QUEUE_BUFFER( vcprx, HAL_VCP_RX_QUEUE_LEN, 1 );
    DEFINE_STATIC_QUEUE_BUFFER( vcptx, HAL_VCP_TX_QUEUE_LEN, 1 );
    hal_vcp_rx_queue = os_queue_create_static( "vcpRxQ", HAL_VCP_RX_QUEUE_LEN, 1, &static_queue_buffer_vcprx );
    hal_vcp_tx_queue = os_queue_create_static( "vcpTxQ", HAL_VCP_TX_QUEUE_LEN, 1, &static_queue_buffer_vcptx );
    DEFINE_STATIC_SEMAPHORE_BUFFER( vcptx );
    hal_vcp_tx_sem = os_semaphore_create_static( 1, 1, &static_semaphore_buffer_vcptx );
#else
    hal_vcp_rx_queue = os_queue_create( "vcpRxQ", HAL_VCP_RX_QUEUE_LEN, 1 );
    hal_vcp_tx_queue = os_queue_create( "vcpTxQ", HAL_VCP_TX_QUEUE_LEN, 1 );
    hal_vcp_tx_sem = os_semaphore_create( 1, 1 );
#endif
    if( (hal_vcp_rx_queue == NULL) || (hal_vcp_tx_queue == NULL) || (hal_vcp_tx_sem == NULL) )
        return 0;
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;

    /* create vcp/tx task */
#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( vcp, TASK_VCP_TX_STACK_SIZE );
    task = os_task_create_static("vcp/txT", task_vcp_tx_entry, NULL, TASK_VCP_TX_STACK_SIZE, TASK_VCP_TX_PRIORITY, &static_task_buffer_vcp);
#else
    task = os_task_create("vcp/txT", task_vcp_tx_entry, NULL, TASK_VCP_TX_STACK_SIZE, TASK_VCP_TX_PRIORITY);
#endif
    if( task == NULL )
        halt("create vcp/tx task");

    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
    return 1;
}


int  shell_driver_init( void )
{
    return hal_uart_init(0);
}


void shell_driver_reset( void )
{
    os_queue_reset( hal_vcp_rx_queue );
    hal_vcp_tx_buf1_len = hal_vcp_tx_buf2_len = 0;
}


int  shell_driver_read( char *buffer, int len )
{
    int bytes=0;
    while( bytes < len )
    {
        if( os_queue_get( hal_vcp_rx_queue, buffer, -1 ) )
        {
            buffer++;
            bytes++;
        }
    }
    return bytes;
}


int  shell_driver_read_char( char *c )
{
    if( os_queue_get( hal_vcp_rx_queue, c, -1 ) )
        return (int)*c;
    else
        return -1;
}


int  shell_driver_read_char_blocked( char *c, int block_ticks )
{
    if( os_queue_get( hal_vcp_rx_queue, c, block_ticks ) )
        return (int)*c;
    else
        return -1;
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
            if( os_queue_put( hal_vcp_tx_queue, (void*)(buffer), 0 ) )
                broken = 0; 
        } 
        else
        {
            if( os_queue_put( hal_vcp_tx_queue, (void*)(buffer), VCP_WRITE_BLOCK_AUTO_ADAPTED_BLOCK_TICK ) == 0 )
                broken = 1; 
        }
        written ++;
        buffer ++;
    }
#else
    /* always blocked, this will make the running task blocked (if connection is broken) */
    while( written < len )
    {
        if( os_queue_put( hal_vcp_tx_queue, (void*)(buffer), -1 ) )
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

