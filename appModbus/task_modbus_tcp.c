/* Modbus/TCP implementation based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "errno.h"
#include "task_blink.h"
#include "task_logger.h"
#include "task_dhcpc.h"
#include "task_modbus_tcp.h"
#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip_lib.h"


LOGGER_MODULE_NAME("modbus");

os_queue_handle_t queue_modbus_tcp;

struct tcp_pcb *modbus_pcb;
struct tcp_pcb *modbus_client_pcb[MODBUS_TCP_NUM];
unsigned int modbus_client_id_new;


int send_modbus_tcp_event( uint8_t type, uint32_t data )
{
    modbus_tcp_event_t evt;
    evt.type = type;
    evt.data = data;
    if( os_queue_put( queue_modbus_tcp, &evt, 0 ) )
        return 1;
    return 0;
}


static void modbus_tcp_free(struct modbus_tcp_state *es)
{
    if( es != NULL )
    {
        if( es->p )
        {
            /* free the buffer chain if present */
            pbuf_free(es->p);
        }
        mem_free(es);
    }
}


static void modbus_tcp_close(struct tcp_pcb *tpcb, struct modbus_tcp_state *es)
{
    int i, id=es->client_id;

    tcp_arg(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_poll(tpcb, NULL, 0);
    modbus_tcp_free(es);
    tcp_close(tpcb);
    /* unregister the client pcb */
    for( i=0; i<MODBUS_TCP_NUM; i++ )
    {
        if( modbus_client_pcb[i] == tpcb )
            modbus_client_pcb[i] = 0;
    }
    logger_printf_info( "client #%u closed", id );
}


static void modbus_tcp_send(struct tcp_pcb *tpcb, struct modbus_tcp_state *es)
{
    struct pbuf *ptr;
    err_t wr_err = ERR_OK;

    while( (wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb)))
    {
        ptr = es->p;

        /* enqueue data for transmission */
        wr_err = tcp_write(tpcb, "MODBUS RETURN:", 14, 1);
        wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
        if( wr_err == ERR_OK )
        {
            u16_t plen;
            plen = ptr->len;
            /* continue with next pbuf in chain (if any) */
            es->p = ptr->next;
            if(es->p != NULL)
            {
                /* new reference! */
                pbuf_ref(es->p);
            }
            /* chop first pbuf from chain */
            pbuf_free(ptr);
            /* we can read more data now */
            tcp_recved(tpcb, plen);
        }
        else if(wr_err == ERR_MEM)
        {
            /* we are low on memory, try later / harder, defer to poll */
            es->p = ptr;
        }
        else
        {
            /* other problem ?? */
        }
    }
}


static void modbus_tcp_error(void *arg, err_t err)
{
    struct modbus_tcp_state *es;

    LWIP_UNUSED_ARG(err);

    es = (struct modbus_tcp_state *)arg;
                
    logger_const_error( "tcp_err" );

    modbus_tcp_free(es);
}


static err_t modbus_tcp_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct modbus_tcp_state *es;

    es = (struct modbus_tcp_state *)arg;
    if( es != NULL )
    {
        if( es->p != NULL )
        {
            /* there is a remaining pbuf (chain)  */
            modbus_tcp_send(tpcb, es);
        }
        else
        {
            /* no remaining pbuf (chain)  */
            if(es->state == ES_CLOSING)
            {
                modbus_tcp_close(tpcb, es);
            }
        }
        ret_err = ERR_OK;
    }
    else
    {
        /* nothing to be done */
        tcp_abort(tpcb);
        ret_err = ERR_ABRT;
    }
    return ret_err;
}


static err_t modbus_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct modbus_tcp_state *es;

    LWIP_UNUSED_ARG(len);

    es = (struct modbus_tcp_state *)arg;
    es->retries = 0;

    if( es->p != NULL )
    {
        /* still got pbufs to send */
        tcp_sent(tpcb, modbus_tcp_sent);
        modbus_tcp_send(tpcb, es);
    }
    else
    {
        /* no more pbufs to send */
        if(es->state == ES_CLOSING)
        {
            modbus_tcp_close(tpcb, es);
        }
    }
    return ERR_OK;
}

            
static err_t modbus_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct modbus_tcp_state *es;
    err_t ret_err;
    struct pbuf *p2;
    modbus_tcp_head_t *head;

    LWIP_ASSERT("arg != NULL",arg != NULL);
    es = (struct modbus_tcp_state *)arg;
    if( p == NULL )
    {
        /* remote host closed connection */
        es->state = ES_CLOSING;
        if( es->p == NULL )
        {
            /* we're done sending, close it */
            modbus_tcp_close(tpcb, es);
        }
        else
        {
            /* we're not done yet */
            modbus_tcp_send(tpcb, es);
        }
        ret_err = ERR_OK;
    }
    else if(err != ERR_OK)
    {
        /* cleanup, for unknown reason */
        if( p != NULL )
        {
            pbuf_free(p);
        }
        ret_err = err;
    }
    else if(es->state == ES_READY)
    {
        /* prepare/pre-check input packet before process */
        p2 = p;
        es->buf_len = 0;
        while( p2 != NULL )
        {
            if( p2->len + es->buf_len + 1 > MODBUS_TCP_BUF_LEN_MAX )
            {
                //logger_const_error( "buffer too small to recv" );
                memcpy( es->buf+es->buf_len, p2->payload, MODBUS_TCP_BUF_LEN_MAX-p2->len );
                es->buf_len = MODBUS_TCP_BUF_LEN_MAX;
                break;
            }
            memcpy( es->buf+es->buf_len, p2->payload, p2->len );
            es->buf_len += p2->len;
            //es->buf[es->buf_len] = 0;
            p2 = p2->next;
        }
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* process packet */
        if( es->buf_len >= sizeof(modbus_tcp_head_t) )
        {
            head = (modbus_tcp_head_t*)es->buf;
            swap_bytes( (uint8_t*)&head->bytes, ((uint8_t*)&head->bytes)+1 );
            if( es->buf_len != sizeof(modbus_tcp_head_t) - 2 + head->bytes )
            {
                /* invalid data length */
                logger_const_warn( "length err" );
            }
            else
            {
                if( head->bytes < 6 )
                    logger_const_warn( "parm err" );
                else
                {
                    es->state = ES_BUSY;
                    send_modbus_tcp_event( MODBUS_TCP_EVENT_PACKET, (uint32_t)es );
                }
            }
        }
        else
        {
            /* ignore packets with invalid head size */
            logger_const_warn( "head err" );
        }

        ret_err = ERR_OK;
    }
    else
    {
        /* ignore all data when state not ready */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}


static err_t modbus_tcp_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    err_t ret_err;
    struct modbus_tcp_state *es;
    int i;
    char buf[64];

    LWIP_UNUSED_ARG(arg);
    if((err != ERR_OK) || (newpcb == NULL))
    {
        return ERR_VAL;
    }

    /* search for free slot and register */
    for( i=0; i<MODBUS_TCP_NUM; i++ )
    {
        if( modbus_client_pcb[i] == NULL )
            break;
    }
    if( i >= MODBUS_TCP_NUM )
    {
        /* check for the the first timeout socket (this is not the best policy) */
        for( i=0; i<MODBUS_TCP_NUM; i++ )
        {
            es = (struct modbus_tcp_state *)(modbus_client_pcb[i]->callback_arg);
            if( os_tick() - es->tick_last > MODBUS_TCP_TIMEOUT * configTICK_RATE_HZ )
            {
                /* find one, close it */
                modbus_tcp_close( modbus_client_pcb[i], es );
                break;
            }
        }
        if( i >= MODBUS_TCP_NUM )
            return ERR_VAL;  /* fail */
    }
    modbus_client_pcb[i] = newpcb;

    /* Unless this pcb should have NORMAL priority, set its priority now.
       When running out of pcbs, low priority pcbs can be aborted to create
       new pcbs of higher priority. */
    tcp_setprio(newpcb, TCP_PRIO_MIN);

    es = (struct modbus_tcp_state *)mem_malloc(sizeof(struct modbus_tcp_state));
    if (es != NULL) 
    {
        es->state = ES_READY;
        es->pcb = newpcb;
        es->retries = 0;
        es->client_id = ++modbus_client_id_new;
        es->p = NULL;
        /* pass newly allocated es to our callbacks */
        tcp_arg(newpcb, es);
        tcp_recv(newpcb, modbus_tcp_recv);
        tcp_err(newpcb, modbus_tcp_error);
        tcp_poll(newpcb, modbus_tcp_poll, 0);
        tcp_sent(newpcb, modbus_tcp_sent);
        ret_err = ERR_OK;
        sprintf_ip( buf, newpcb->remote_ip.addr, 0, 0 );
        logger_printf_info( "client #%u (%s) connected 0x%08X", es->client_id, buf, (void*)newpcb );
    }
    else
    {
        ret_err = ERR_MEM;
        logger_const_error( "mem err" );
    }
    return ret_err;
}


int do_process_modbus_tcp_packet( modbus_tcp_head_t *head, uint16_t address, uint16_t size, uint16_t *payload, int *write )
{
    uint16_t *buf = payload + 2;
    uint8_t *buf2 = (uint8_t*)buf;
    uint16_t word;
    uint8_t byte, byte2;
    uint8_t bytes; 
    int i, err;

    *write = 0;
    switch( head->function_code )
    {
    case MODBUS_CODE_READ_COILS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    SIZE
                (29 33) (00 00) (00 06) (01) (01)  ---  (00 64) (00 0A)
           send: TID     PID     LEN     UID  CODE       BYTES  DAT0   DAT1  ... 
                (29 33) (00 00) (00 05) (01) (01)  ---  (02)   (00)   (00)   ...  */
    case MODBUS_CODE_READ_DISCRETE_INPUTS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    SIZE
                (29 33) (00 00) (00 06) (01) (02)  ---  (00 64) (00 0A)
           send: TID     PID     LEN     UID  CODE       BYTES  DAT0   DAT1  ... 
                (29 33) (00 00) (00 05) (01) (02)  ---  (02)   (00)   (00)   ...  */
        bytes = 0;
        buf2 = (uint8_t*)payload + 1;
        byte = i = 0;
        while( size-- )
        { 
            if( head->function_code == MODBUS_CODE_READ_DISCRETE_INPUTS )
                err = modbus_read_discrete( address, &byte2 );
            else
                err = modbus_read_coil( address, &byte2 );
            if( err )
                return err;
            if( byte2 )
                byte |= 1<<i;
            if( ++i >= 8 )
            {
                bytes++;
                *buf2++ = byte;
                byte = i = 0;
            }
            address++;
        }
        if( i != 0 )
        {
            bytes++;
            *buf2 = byte;
        }
        *((uint8_t*)payload) = bytes;
        *write = bytes + 1;
        break;

    case MODBUS_CODE_WRITE_SINGLE_COIL:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    DAT
                (29 33) (00 00) (00 06) (01) (05)  ---  (00 10) (FF 00)
           send: TID     PID     LEN     UID  CODE       ADDR    DAT
                (29 33) (00 00) (00 06) (01) (05)  ---  (00 10) (FF 00) */
        if( (size != 0xFF00) && (size != 0) )
            return MODBUS_ERROR_DATA_VALUE;
        err = modbus_write_coil( address, size ? 1 : 0 );
        if( err )
            return err;
        *write = 4;
        break;

    case MODBUS_CODE_WRITE_MULTI_COILS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    SIZE    BYTES DAT0 DAT1 ...
                (29 33) (00 00) (00 08) (01) (0F)  ---  (00 10) (00 02) (04)  (00) (00)  ...
           send: TID     PID     LEN     UID  CODE       ADDR    SIZE
                (29 33) (00 00) (00 06) (01) (0F)  ---  (00 10) (00 02) */
        if( *buf2 != (size-1)/8+1 )
            return MODBUS_ERROR_DATA_VALUE;
        buf2++;
        i = 0;
        while( size-- )
        { 
            err = modbus_write_coil( address, (*buf2) & (1<<i) );
            if( err )
                return err;
            if( ++i >= 8 )
            {
                i = 0;
                buf2++;
            }
            address++;
        }
        *write = 4;
        break;

    case MODBUS_CODE_READ_INPUT_REGISTERS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    DAT
                (29 33) (00 00) (00 06) (01) (04)  ---  (27 56) (00 00)
           send: TID     PID     LEN     UID  CODE       BYTES  DAT0    DAT1   ... 
                (29 33) (00 00) (00 17) (01) (04)  ---  (14)   (61 4E) (00 BC) ...  */
    case MODBUS_CODE_READ_HOLD_REGISTERS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    SIZE 
                (29 33) (00 00) (00 06) (01) (03)  ---  (27 56) (00 0A)
           send: TID     PID     LEN     UID  CODE       BYTES  DAT0    DAT1   ... 
                (29 33) (00 00) (00 17) (01) (03)  ---  (14)   (61 4E) (00 BC) ...  */
        if( size > MODBUS_MULTI_REGISTER_LIMIT )
            return MODBUS_ERROR_DATA_VALUE;
        bytes = 0;
        buf2 = (uint8_t*)payload + 1;
        while( size-- )
        {
            if( head->function_code == MODBUS_CODE_READ_HOLD_REGISTERS )
                err = modbus_read_hold_register( address, &word );
            else
                err = modbus_read_input_register( address, &word );
            if( err )
                return err; 
            *buf2++ = (word>>8) & 0xFF;  /* big endian */
            *buf2++ = word & 0xFF;
            bytes += 2;
            address++;
        }
        *((uint8_t*)payload) = bytes;
        *write = bytes + 1;
        break;

    case MODBUS_CODE_WRITE_SINGLE_REGISTER:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    DAT
                (29 33) (00 00) (00 06) (01) (06)  ---  (27 56) (00 00)
           send: TID     PID     LEN     UID  CODE       ADDR    DAT
                (29 33) (00 00) (00 06) (01) (06)  ---  (27 56) (00 00) */
        err = modbus_write_hold_register( address, size );
        if( err )
            return err;
        *write = 4;
        break;

    case MODBUS_CODE_WRITE_MULTI_REGISTERS:
        /* recv: TID     PID     LEN     UID  CODE       ADDR    SIZE    BYTES  DAT0  DAT1
                (29 33) (00 00) (00 0B) (01) (10)  ---  (27 56) (00 02) (04) (61 4E) (00 BC)
           send: TID     PID     LEN     UID  CODE       ADDR    SIZE   
                (29 33) (00 00) (00 06) (01) (10)  ---  (27 56) (00 02) */
        if( *buf2 != 2*size )
            return MODBUS_ERROR_DATA_VALUE;
        buf2++;
        while( size-- )
        {
            word = *(buf2++);
            word = (word << 8) + *(buf2++);
            err = modbus_write_hold_register( address, word );
            if( err )
                return err;
            address++;
        }
        *write = 4;
        break;

    default:
        return MODBUS_ERROR_FUNCTION;
        break;
    }
    return 0;
}


void task_modbus_tcp_entry(void *arg)
{
    modbus_tcp_event_t evt;
    err_t err;
    struct modbus_tcp_state *es;
    modbus_tcp_head_t *head;
    uint16_t transaction_id, protocol_id, address, size, *payload;
    int write;
    int i;

    (void)arg;
    while( 1 )
    {
        if( os_queue_get( queue_modbus_tcp, &evt, -1 ) == 0 )
            continue;
        
        switch( evt.type )
        {
        case MODBUS_TCP_EVENT_NET_UP:
            send_modbus_tcp_event( MODBUS_TCP_EVENT_RESET, 0 );
            break;

        case MODBUS_TCP_EVENT_NET_DOWN:
            send_modbus_tcp_event( MODBUS_TCP_EVENT_CLOSE, 0 );
            break;

        case MODBUS_TCP_EVENT_RESET:
            send_modbus_tcp_event( MODBUS_TCP_EVENT_CLOSE, 0 );
            send_modbus_tcp_event( MODBUS_TCP_EVENT_START, 0 );
            break;

        case MODBUS_TCP_EVENT_START:
            modbus_pcb = tcp_new_ip_type( IPADDR_TYPE_ANY );
            if( modbus_pcb != NULL )
            {
                err = tcp_bind(modbus_pcb, IP_ANY_TYPE, MODBUS_TCP_PORT);
                if (err == ERR_OK)
                {
                    modbus_pcb = tcp_listen(modbus_pcb);
                    tcp_accept( modbus_pcb, modbus_tcp_accept );
                    logger_printf_debug( "listening on port %d, pcb=0x%08X", MODBUS_TCP_PORT, modbus_pcb );
                }
                else
                {
                    /* abort? output diagnostic? */
                }
            }
            else
            {
                /* abort? output diagnostic? */
            }
            break;

        case MODBUS_TCP_EVENT_CLOSE:
            /* close all registered client pcb */
            for( i=0; i<MODBUS_TCP_NUM; i++ )
            {
                if( modbus_client_pcb[i] != NULL )
                    modbus_tcp_close( modbus_client_pcb[i], modbus_client_pcb[i]->callback_arg );
            }
            if( modbus_pcb != NULL )
            {
                tcp_close( modbus_pcb );
                modbus_pcb = 0;
                logger_const_info( "closed" );
            }
            break;

        case MODBUS_TCP_EVENT_PACKET:
            es = (struct modbus_tcp_state *)(evt.data);
            head = (modbus_tcp_head_t*)es->buf;
            transaction_id = head->transaction_id;
            protocol_id = head->protocol_id;
            address = *(uint16_t*)(es->buf+sizeof(modbus_tcp_head_t));
            size = *(uint16_t*)(es->buf+sizeof(modbus_tcp_head_t)+2);
            swap_bytes( (uint8_t*)&transaction_id, ((uint8_t*)&transaction_id)+1 );
            swap_bytes( (uint8_t*)&protocol_id, ((uint8_t*)&protocol_id)+1 );
            swap_bytes( (uint8_t*)&address, ((uint8_t*)&address)+1 );
            swap_bytes( (uint8_t*)&size, ((uint8_t*)&size)+1 );
            //logger_printf_info( "TID 0x%04X, PID 0x%04X, LEN %u, UID 0x%04X, CODE %u, ADDR 0x%04X, SIZE %u",
            //    transaction_id, protocol_id, head->bytes, head->unit_id, head->function_code, address, size );
            payload = (uint16_t*)(es->buf+sizeof(modbus_tcp_head_t));
            i = do_process_modbus_tcp_packet( (modbus_tcp_head_t*)es->buf, address, size, payload, &write );
            if( i )
            {
                /* send error response */
                head->function_code |= 0x80;
                *((uint8_t*)payload) = i;
                write = 1;
            }
            else
            {
                /* normal packet, update tick */
                es->tick_last = os_tick();
            }
            /* send response packet */
            head->bytes = 2 + write;
            swap_bytes( (uint8_t*)&(head->bytes), ((uint8_t*)&(head->bytes))+1 );
            if( ERR_OK == tcp_write( es->pcb, (char*)es->buf, sizeof(modbus_tcp_head_t) + write, 1 ) )
            {
                if( ERR_OK == tcp_output( es->pcb ) )
                {
                    /* fail to send */
                }
            }
            else
            {
                logger_const_error( "tcp_write" );
            }
            es->state = ES_READY;
            break;

        default:
            break;
        }
    }
}


void task_modbus_tcp_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_QUEUE_BUFFER( modbus_tcp, TASK_MODBUS_TCP_QUEUE_SIZE, sizeof(modbus_tcp_event_t) );
    queue_modbus_tcp = os_queue_create_static( "modbusTQ", 
                    TASK_MODBUS_TCP_QUEUE_SIZE, sizeof(modbus_tcp_event_t),
                    &static_queue_buffer_modbus_tcp );
#else
    queue_modbus_tcp = os_queue_create_( "modbusTQ", 
                    TASK_MODBUS_TCP_QUEUE_SIZE, sizeof(modbus_tcp_event_t) );
#endif
    if( queue_modbus_tcp == NULL )
        halt("create modbus tcp queue");

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( modbus_tcp, TASK_MODBUS_TCP_STACK_SIZE );
    task = os_task_create_static( "modbusTT", task_modbus_tcp_entry, NULL,
                TASK_MODBUS_TCP_STACK_SIZE, TASK_MODBUS_TCP_PRIORITY,
                &static_task_buffer_modbus_tcp );
#else
    task = os_task_create( "modbusTT", task_modbus_tcp_entry, NULL,
                TASK_MODBUS_TCP_STACK_SIZE, TASK_MODBUS_TCP_PRIORITY );
#endif
    if( task == NULL )
        halt("create modbus tcp task");
}

