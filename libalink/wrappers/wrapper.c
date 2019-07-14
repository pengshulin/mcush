/* ALI-IOT SDK ported for MCUSH/LWIP platform 
 * MCUSH designed by Peng Shulin, all rights reserved.
 */
#include "mcush.h"
#include "task_dhcpc.h"
#include "task_logger.h"
#include "env.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/dns.h"
#include "lwip/icmp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"
#include "lwip_commands.h"
#include "lwip_lib.h"

LOGGER_MODULE_NAME("alink");
 
#define ALINK_TIMEOUT  (10*configTICK_RATE_HZ)

//#define DEBUG_ALINK_WRAPPER  1




/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "stdarg.h"

/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(void *ptr)
{
    vPortFree(ptr);
}


/**
 * @brief Get device name from user's system persistent storage
 *
 * @param [ou] device_name: array to store device name, max length is IOTX_DEVICE_NAME_LEN
 * @return the actual length of device name
 */
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1])
{
#ifdef DEVICE_NAME
    strcpy( device_name, DEVICE_NAME );
#else
    if( env.device_name )
        strcpy( device_name, env.device_name );
    else
        strcpy( device_name, "unamed" );
#endif
    return strlen(device_name);
}


/**
 * @brief Get device secret from user's system persistent storage
 *
 * @param [ou] device_secret: array to store device secret, max length is IOTX_DEVICE_SECRET_LEN
 * @return the actual length of device secret
 */
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
#ifdef DEVICE_SECRET
    strcpy( device_secret, DEVICE_SECRET );
#else
    /* device secret will be loaded from file dynamically */
    memset( device_secret, 0, IOTX_DEVICE_SECRET_LEN+1 );
    mcush_file_load_string( DEVICE_SECRET_FILE_NAME, device_secret, IOTX_DEVICE_SECRET_LEN );
    rstrip( device_secret );
#endif
    return strlen(device_secret);
}


/**
 * @brief Get firmware version
 *
 * @param [ou] version: array to store firmware version, max length is IOTX_FIRMWARE_VER_LEN
 * @return the actual length of firmware version
 */
extern const char version_string[];
extern const char build_signature[];
int HAL_GetFirmwareVersion(char *version)
{
    strcpy( version, version_string );
    strcat( version, " " );
    strcpy( version, build_signature );
	return strlen(version);
}


/**
 * @brief Get product key from user's system persistent storage
 *
 * @param [ou] product_key: array to store product key, max length is IOTX_PRODUCT_KEY_LEN
 * @return  the actual length of product key
 */
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1])
{
    strcpy( product_key, PRODUCT_KEY );
    return strlen(product_key);
}


int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1])
{
    strcpy( product_secret, PRODUCT_SECRET );
    return strlen(product_secret);
}


int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
    return (int)1;
}


int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
    return (int)1;
}


/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Malloc(uint32_t size)
{
    return pvPortMalloc(size);
}


/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void *HAL_MutexCreate(void)
{
    return (void*)xSemaphoreCreateMutex();
}


/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(void *mutex)
{
    if( mutex == NULL )
        return;
    vSemaphoreDelete((QueueHandle_t)mutex);
}


/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(void *mutex)
{
    if( mutex == NULL )
        return;

    while(1)
    {
        if( xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY) == pdPASS )
            break;
    }
}


/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(void *mutex)
{
    if( mutex == NULL )
        return;

    xSemaphoreGive((SemaphoreHandle_t)mutex);
}


/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Printf(const char *fmt, ...)
{
    va_list ap;
    char buf[1000];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    if( (buf[0]=='[') && (buf[2]==']') )
    {
        switch( buf[1] )
        {
        case 'E':
        case 'C':
            logger_error( buf );
            break;
        case 'W':
            logger_warn( buf );
            break;
        case 'I':
            logger_info( buf );
            break;
        case 'D':
        case 'F':
        default:
            logger_debug( buf );
            break;
        }
    }
    else
        logger_debug( buf );
    va_end(ap);
}


uint32_t HAL_Random(uint32_t region)
{
    return (uint32_t)1;
}


int HAL_SetDeviceName(char *device_name)
{
    return (int)1;
}


int HAL_SetDeviceSecret(char *device_secret)
{
    return (int)1;
}


int HAL_SetProductKey(char *product_key)
{
	return (int)1;
}


int HAL_SetProductSecret(char *product_secret)
{
	return (int)1;
}


/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(uint32_t ms)
{
    vTaskDelay( ms * configTICK_RATE_HZ / 1000 );
}


/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void)
{
    return (uint64_t)xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
}


/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsnprintf(str, len, fmt, ap);
    va_end(ap);
    return n;
}


int HAL_Vsnprintf(char *str, const int len, const char *fmt, va_list ap)
{
    return vsnprintf(str, len, fmt, ap);
}


void HAL_Srandom(uint32_t seed)
{
    srand(seed);
}


/***************************************************************************
 * LWIP/TCP related
 ***************************************************************************/

#define ALINK_BUF_LEN   1024
#define ALINK_BUF_MASK  0x3FF
typedef struct _alink_cb_t
{
    struct tcp_pcb *pcb;
    uint32_t server_ip;
    int server_port;
    uint8_t dns_done;
    uint8_t done;
    uint8_t connected;
    uint8_t closed;
    uint8_t timeout;
    uint8_t sent;
    uint8_t written;
    int buf_len, buf_read, buf_write;
    char buf[ALINK_BUF_LEN];
} alink_cb_t;
static alink_cb_t *acb;


void alink_dns_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    if( acb == NULL )
        return;
    if( ipaddr == NULL )
        acb->server_ip = 0;
    else
        acb->server_ip = *(uint32_t*)ipaddr;
    acb->dns_done = 1;
}


void alink_error_cb(void *arg, err_t err)
{
    if( err == ERR_ABRT )
        logger_const_error( "abrt" );
    else if( err == ERR_RST )
        logger_const_error( "rst" );
    else
        logger_const_error( "err" );
}


err_t alink_sent_cb(void *arg, struct tcp_pcb *pcb, uint16_t len)
{
#if DEBUG_ALINK_WRAPPER
    logger_printf_debug( "%d bytes sent", len ); 
#endif
    if( acb )
        acb->sent = 1;
    return ERR_OK;
}


err_t alink_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *p2;
    int i;

    if( acb == NULL )
        return ERR_OK;
 
    if( pcb != acb->pcb )
    {
        logger_printf_debug( "old tcp input ignored (port %d)", pcb->local_port );
        return tcp_close( pcb );
    }

    if( err == ERR_OK )
    {
        if( p != NULL )
        {
            p2 = p;
            while( p2 != NULL )
            {
                if( acb->buf_write + p2->len >= ALINK_BUF_LEN )
                {
                    i = ALINK_BUF_LEN - acb->buf_write;
                    memcpy( acb->buf + acb->buf_write, p2->payload, i );
                    if( p2->len > i )
                    {
                        memcpy( acb->buf, p2->payload+i, p2->len-i );
                        acb->buf_write = p2->len-i;
                    }
                    else
                        acb->buf_write = 0;
                    acb->buf_len += p2->len;
                }
                else
                {
                    memcpy( acb->buf + acb->buf_write, p2->payload, p2->len );
                    acb->buf_write += p2->len;
                    acb->buf_len += p2->len;
                }
                if( acb->buf_len > ALINK_BUF_LEN )
                {
                    acb->buf_len = ALINK_BUF_LEN;
                    acb->buf_read = acb->buf_write;
                }
                p2 = p2->next;
            }
            tcp_recved(pcb, p->tot_len);
            pbuf_free(p);
        }
        else
        {
            /* tcp closed */
            logger_printf_error( "tcp closed (port %d)", pcb->local_port );
            //send_alink_event( ALINK_EVENT_CONNECT_ERROR, (uint32_t)pcb );
            return tcp_close( pcb );
        }
    }
    else
    {
        logger_const_error( "recv_cb err" );
        /* NOTE: something error, do not call tcp_abort and return ERR_ABRT here,
                 the alink task will do that instead */
        //send_alink_event( ALINK_EVENT_CONNECT_ERROR, (uint32_t)pcb );
    }

    //logger_printf_debug( "alink buf_len %d", acb->buf_len );
    return ERR_OK;
}

 
static err_t alink_connected_cb(void *arg, struct tcp_pcb *pcb, err_t err)
{
    if( acb == NULL )
        return ERR_OK;
 
    if( err != ERR_OK )
    {
        tcp_close(pcb);
        acb->closed = 1;
    }
    else
    {
        tcp_recv(pcb, alink_recv_cb);
        tcp_err(pcb, alink_error_cb);
        //tcp_poll(pcb, alink_poll_cb, 10);
        tcp_sent(pcb, alink_sent_cb);
        acb->connected = 1;
    }
    return ERR_OK;
}


/**
 * @brief Destroy the specific TCP connection.
 *
 * @param [in] fd: @n Specify the TCP connection by handle.
 *
 * @return The result of destroy TCP connection.
 * @retval < 0 : Fail.
 * @retval   0 : Success.
 */
int HAL_TCP_Destroy(uintptr_t fd)
{
    alink_cb_t *pacb = (alink_cb_t*)fd;

#if DEBUG_ALINK_WRAPPER
    logger_printf_debug("HAL_TCP_Destroy(0x%08X)", (uint32_t)fd);
#endif
    acb = 0;
    if( (void*)fd == NULL )
        return 0;
    tcp_close( pacb->pcb ); 
    vPortFree( (void*)fd );
    return 0;
}


/**
 * @brief Establish a TCP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the TCP server
 * @param [in] port: @n Specify the TCP port of TCP server
 *
 * @return The handle of TCP connection.
   @retval (uintptr_t)(-1): Fail.
   @retval All other values(0 included): Success, the value is handle of this TCP connection.
 */
uintptr_t HAL_TCP_Establish(const char *host, uint16_t port)
{
    alink_cb_t *pacb;
    ip_addr_t ipaddr;
    TickType_t t0;
    char buf[64];

#if DEBUG_ALINK_WRAPPER
    logger_printf_debug("HAL_TCP_Establish(%s, %d)", (char*)host, port);
#endif
    
    pacb = pvPortMalloc(sizeof(alink_cb_t));
    if( pacb == NULL )
        goto stop;
    memset( pacb, 0, sizeof(alink_cb_t) );
    acb = pacb;
    
    /* dns check */
    if( ERR_OK == dns_gethostbyname( host, (ip_addr_t*)&ipaddr, (dns_found_callback)alink_dns_cb, NULL ) )
        acb->server_ip = (uint32_t)ipaddr.addr;
    else
    {
        /* wait for dns done */
        t0 = xTaskGetTickCount(); 
        while( ! acb->dns_done )
        {
            if( xTaskGetTickCount() > t0 + ALINK_TIMEOUT )
            {
#if DEBUG_ALINK_WRAPPER
                logger_const_error( "dns timeout" );
#endif
                break;
            }
            else
                vTaskDelay(100*configTICK_RATE_HZ/1000);
        }
    }

    if( acb->server_ip == 0 )
    {
#if DEBUG_ALINK_WRAPPER
        logger_const_error( "server addr err" );
#endif
        goto stop;
    }
        
    /* tcp connect */
    logger_info( sprintf_ip( buf, acb->server_ip, "server ip", 0) );
    acb->server_port = port;
    acb->pcb = tcp_new();
    if( acb->pcb == NULL )
    {
#if DEBUG_ALINK_WRAPPER
        logger_const_error( "no memory" );
#endif
        goto stop;
    }
    else
    {
        if( tcp_bind_random_port(acb->pcb) )
        {
            logger_printf_debug( "bind port %u", acb->pcb->local_port );
            tcp_connect( acb->pcb, (const ip_addr_t *)&acb->server_ip, acb->server_port, alink_connected_cb );
            /* wait for done */
            t0 = xTaskGetTickCount(); 
            while( !acb->connected && !acb->closed )
            {
                vTaskDelay(configTICK_RATE_HZ / 2);
                if( xTaskGetTickCount() > t0 + ALINK_TIMEOUT )
                {
#if DEBUG_ALINK_WRAPPER
                    logger_const_error( "conn timeout" );
#endif
                    break;
                }
            }
        }
    }
    if( acb->closed )
        goto stop; 

#if DEBUG_ALINK_WRAPPER
    logger_printf_info( "established acb=0x%08X", (uint32_t)acb );
#endif
    return (uintptr_t)acb;

stop:
    acb = 0;
    if( pacb )
        vPortFree( (void*)pacb );
    return (uintptr_t)-1;
}


/**
 * @brief Read data from the specific TCP connection with timeout parameter.
 *        The API will return immediately if 'len' be received from the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a TCP connection.
 * @param [out] buf @n A pointer to a buffer to receive incoming data.
 * @param [out] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval       -2 : TCP connection error occur.
 * @retval       -1 : TCP connection be closed by remote server.
 * @retval        0 : No any data be received in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be received in 'timeout_ms' timeout period.

 * @see None.
 */
int32_t HAL_TCP_Read(uintptr_t fd, char *buf, uint32_t len, uint32_t timeout_ms)
{
    alink_cb_t *acb = (alink_cb_t *)fd;
    TickType_t t0;
    int bytes=0;
    char *p = buf;

#if DEBUG_ALINK_WRAPPER
    logger_printf_debug("HAL_TCP_Read(0x%08X, %u, %u)", (uint32_t)fd, len, timeout_ms);
#endif
    if( acb == NULL )
        return -1;
    if( len == 0 )
        return 0;

    if( len > ALINK_BUF_LEN )
        halt("len err");
    t0 = xTaskGetTickCount(); 
    while( bytes < len )
    {
        if( acb->buf_len )
        {
            *p++ = acb->buf[acb->buf_read];
            acb->buf_read = (acb->buf_read + 1) & ALINK_BUF_MASK;
            acb->buf_len--;
            bytes++;
        }
        else if( xTaskGetTickCount() > t0 + timeout_ms*configTICK_RATE_HZ/1000 )
        {
            //logger_const_debug( "timeout" );
            break;
        }
        else
            vTaskDelay(100*configTICK_RATE_HZ/1000);
     }

#if DEBUG_ALINK_WRAPPER
    if( bytes )
        logger_buffer( buf, bytes );
#endif
    return bytes;
}



/**
 * @brief Write data into the specific TCP connection.
 *        The API will return immediately if 'len' be written into the specific TCP connection.
 *
 * @param [in] fd @n A descriptor identifying a connection.
 * @param [in] buf @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] len @n The length, in bytes, of the data pointed to by the 'buf' parameter.
 * @param [in] timeout_ms @n Specify the timeout value in millisecond. In other words, the API block 'timeout_ms' millisecond maximumly.
 *
 * @retval      < 0 : TCP connection error occur..
 * @retval        0 : No any data be write into the TCP connection in 'timeout_ms' timeout period.
 * @retval (0, len] : The total number of bytes be written in 'timeout_ms' timeout period.

 * @see None.
 */

int32_t HAL_TCP_Write(uintptr_t fd, const char *buf, uint32_t len, uint32_t timeout_ms)
{
    alink_cb_t *acb = (alink_cb_t *)fd;
    TickType_t t0;

#if DEBUG_ALINK_WRAPPER
    logger_printf_debug("HAL_TCP_Write(0x%08X, %u, %u)", (uint32_t)fd, len, timeout_ms);
    logger_buffer( buf, len );
#endif
   
    if( acb == NULL )
        return -1;

    t0 = xTaskGetTickCount(); 
    acb->sent = acb->written = 0;
    while( ! acb->sent )
    {
        if( ! acb->written )
        {
            if( ERR_OK == tcp_write( acb->pcb, buf, len, 1 ) )
            {
                //logger_printf_debug("%u bytes written", len);
                acb->written = 1;
            }
        }
        
        if( xTaskGetTickCount() > t0 + timeout_ms*configTICK_RATE_HZ/1000 )
        {
#if DEBUG_ALINK_WRAPPER
            logger_const_debug( "tcp_write timeout" );
#endif
            break;
        }
        else
            vTaskDelay(100*configTICK_RATE_HZ/1000);
    }

    if( acb->sent )
	    return len;
    else if( acb->written )
        return 0;
    else
        return -1;
}




//int32_t HAL_SSL_Destroy(uintptr_t handle)
//{
//    return (int32_t)1;
//}
//
//
//uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt, uint32_t ca_crt_len)
//{
//    return (uintptr_t)1;
//}
//
//
//int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
//{
//    return (int)1;
//}
//
//
//int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
//{
//    return (int)1;
//}
//
//
//
