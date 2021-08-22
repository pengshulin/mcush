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
#include "lwip/timeouts.h"

#define DEBUG_MODBUS  1


LOGGER_MODULE_NAME("modbus");

os_queue_handle_t queue_modbus_tcp;

struct tcp_pcb *modbus_tpcb;
modbus_tcp_client_t *clients[MODBUS_TCP_NUM];
static unsigned int modbus_client_id_new;

void do_modbus_tcp_client_close(modbus_tcp_client_t *client);


int send_modbus_tcp_event( uint8_t type, uint32_t data )
{
    modbus_tcp_event_t evt;
    evt.type = type;
    evt.data = data;
    if( os_queue_put( queue_modbus_tcp, &evt, 0 ) )
        return 1;
    return 0;
}


void modbus_server_restart_timer( void *arg )
{
    (void)arg;
    //logger_const_debug( "server restart timer" );
    send_modbus_tcp_event( MODBUS_TCP_EVENT_START, 0 );
    sys_untimeout( modbus_server_restart_timer, 0 );
}


void modbus_reply_resend_timer( void *arg )
{
    //logger_const_debug( "reply resend timer" );
    send_modbus_tcp_event( MODBUS_TCP_EVENT_REPLY, (uint32_t)arg );
    sys_untimeout( modbus_reply_resend_timer, arg );
}



void modbus_tcp_error_cb(void *arg, err_t err)
{
    LOGGER_MODULE_NAME("modbus.err");
    modbus_tcp_client_t *client;
    if( arg == NULL )
    {
        if( err == ERR_ABRT )
            logger_const_debug( "abrt" );
        else if( err == ERR_RST )
            logger_const_debug( "rst" );
        else
            logger_const_debug( "err" );
    }
    else
    {
        client=(modbus_tcp_client_t *)arg;
        if( err == ERR_ABRT )
            logger_printf_debug( "#%d abrt", client->client_id );
        else if( err == ERR_RST )
            logger_printf_debug( "#%d rst", client->client_id );
        else
            logger_printf_debug( "#%d err", client->client_id );
        do_modbus_tcp_client_close( client );
    }
}


err_t modbus_tcp_poll_cb(void *arg, struct tcp_pcb *tpcb)
{
    LOGGER_MODULE_NAME("modbus.poll");
    err_t ret_err = ERR_OK;
    modbus_tcp_client_t *client=(modbus_tcp_client_t *)arg;

    if( client != NULL )
    {
        logger_printf_debug( "#%u 0x%08X", client->client_id, (uint32_t)tpcb );
    }
    return ret_err;
}


err_t modbus_tcp_sent_cb(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    //LOGGER_MODULE_NAME("modbus.sent");
    //modbus_tcp_client_t *es=(modbus_tcp_client_t *)arg;

    (void)arg;
    (void)tpcb;
    (void)len;
    //logger_printf_debug( "#%d %d bytes sent", client->client_id, len );
    return ERR_OK;
}

            
err_t modbus_tcp_recv_cb(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    LOGGER_MODULE_NAME("modbus.recv");
    modbus_tcp_client_t *client=(modbus_tcp_client_t *)arg;
    err_t ret_err;
    struct pbuf *p2;
    modbus_tcp_head_t *head;

    if( p == NULL )
    {
        /* remote host closed connection */
        //logger_const_info( "remote closed" );
        do_modbus_tcp_client_close( client );
        ret_err = ERR_OK;
    }
    else if(err != ERR_OK)
    {
        /* cleanup, for unknown reason */
        if( p != NULL )
        {
            logger_printf_warn( "err=%d", err );
            pbuf_free(p);
        }
        ret_err = err;
    }
    else if(tpcb->state == ESTABLISHED)
    {
        /* prepare/pre-check input packet before process */
        p2 = p;
        client->buf_len = 0;
        while( p2 != NULL )
        {
            if( p2->len + client->buf_len + 1 > MODBUS_TCP_BUF_LEN_MAX )
            {
                //logger_const_error( "buffer too small to recv" );
                memcpy( client->buf+client->buf_len, p2->payload, MODBUS_TCP_BUF_LEN_MAX-client->buf_len );
                client->buf_len = MODBUS_TCP_BUF_LEN_MAX;
                break;
            }
            memcpy( client->buf+client->buf_len, p2->payload, p2->len );
            client->buf_len += p2->len;
            //client->buf[client->buf_len] = 0;
            p2 = p2->next;
        }
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* process packet */
        if( client->buf_len >= sizeof(modbus_tcp_head_t) )
        {
            head = (modbus_tcp_head_t*)client->buf;
            swap_bytes( (uint8_t*)&head->bytes, ((uint8_t*)&head->bytes)+1 );
            if( client->buf_len != sizeof(modbus_tcp_head_t) - 2 + head->bytes )
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
                    send_modbus_tcp_event( MODBUS_TCP_EVENT_REQUEST, (uint32_t)client );
                }
            }
        }
        else
        {
            logger_const_warn( "head err" );
        }
        ret_err = ERR_OK;
    }
    else
    {
        /* ignore all data when state not ready */
        logger_printf_debug( "ignored in state %s", tcp_debug_state_str(tpcb->state));
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        ret_err = ERR_OK;
    }
    return ret_err;
}


err_t modbus_tcp_accept_cb(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LOGGER_MODULE_NAME("modbus.accept");
    err_t ret_err;
    modbus_tcp_client_t *client;
    struct tcp_pcb *tpcb;
    int i;
    char buf[64];

    (void)(arg);
    if((err != ERR_OK) || (newpcb == NULL))
    {
        logger_const_debug( "err" );
        return ERR_VAL;
    }

    /* search for free slot and register */
    for( i=0; i<MODBUS_TCP_NUM; i++ )
    {
        if( clients[i] == NULL )
        {
            client = (modbus_tcp_client_t *)CLIENT_MEM_MALLOC(sizeof(modbus_tcp_client_t));
            break;
        }
    }
    /* create fail, or slots full, try to reclaim old clients */
    if( (client == NULL) || (i >= MODBUS_TCP_NUM) )
    {
        /* try to reclaim the tpcb with exceptions */
        for( i=0; i<MODBUS_TCP_NUM; i++ )
        {
            client = clients[i];
            tpcb = client->tpcb;
            if( tpcb->state != ESTABLISHED )
            {
                tcp_arg(tpcb, NULL);
                tcp_abort(tpcb);
                tcp_close(tpcb);
                logger_printf_info( "client #d reclaimed", client->client_id );
                if( client->p != NULL )
                    pbuf_free(client->p);
                break;
            }
        }
        if( i >= MODBUS_TCP_NUM )
            client = NULL;  /* not found */
            //return ERR_VAL;  /* fail */
    }
    /* still not found, deep reclaim */
    if( (client == NULL) || (i >= MODBUS_TCP_NUM) )
    {
        //logger_const_info( "deep reclaim" );
        /* nothing to do, ignore incoming connection request */
        /* reconnect after any other client is closed */
    }
    if (client != NULL) 
    {
        clients[i] = client;
        client->tpcb = newpcb;
        client->pending_request = 0;
        client->pending_reply = 0;
        client->retries = 0;
        client->client_id = ++modbus_client_id_new;
        client->p = NULL;
        client->buf_len = 0;
        client->tick_connect = client->tick_last = os_tick();
        newpcb->so_options |= SOF_KEEPALIVE;
        tcp_setprio(newpcb, TCP_PRIO_MIN);
        tcp_arg(newpcb, client);
        tcp_recv(newpcb, modbus_tcp_recv_cb);
        tcp_err(newpcb, modbus_tcp_error_cb);
        tcp_sent(newpcb, modbus_tcp_sent_cb);
        sprintf_ip( buf, newpcb->remote_ip.addr, 0, 0 );
        logger_printf_info( "client #%u (%s/%d) connected 0x%08X", client->client_id, buf, newpcb->remote_port, (void*)newpcb );
        ret_err = ERR_OK;
    }
    else
    {
        logger_const_error( "mem err" );
        tcp_close(tpcb);
        ret_err = ERR_MEM;
    }
    return ret_err;
}


void do_modbus_tcp_client_close(modbus_tcp_client_t *client)
{
    struct tcp_pcb *tpcb;
    int client_id, i;

    if( client==NULL )
        return;
    client_id = client->client_id;
    tpcb = client->tpcb;
    /* free client record */
    CLIENT_MEM_FREE(client);
    for( i=0; i<MODBUS_TCP_NUM; i++ )
    {
        os_enter_critical();
        if( clients[i] == client )
            clients[i] = 0;
        os_exit_critical();
    }
    /* free tcp_pcb */
    if( tpcb != NULL )
    {
        tcp_arg(tpcb, NULL);
        tcp_close(tpcb);
    }
    logger_printf_info( "client #%u closed 0x%08X", client_id, tpcb );
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
    modbus_tcp_client_t *client;
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
            //logger_const_info( "evt_start" );
            if( modbus_tpcb == NULL )
                modbus_tpcb = tcp_new_ip_type( IPADDR_TYPE_ANY );
            if( modbus_tpcb != NULL )
            {
                err = tcp_bind(modbus_tpcb, IP_ANY_TYPE, MODBUS_TCP_PORT);
                if (err == ERR_OK)
                {
                    modbus_tpcb = tcp_listen(modbus_tpcb);
                    tcp_accept( modbus_tpcb, modbus_tcp_accept_cb );
                    logger_printf_debug( "listening on port %d, tpcb=0x%08X", MODBUS_TCP_PORT, modbus_tpcb );
                }
                else
                {
                    logger_const_error( "bind err" );
                    sys_timeout( 500, modbus_server_restart_timer, 0 );
                }
            }
            else
            {
                logger_const_error( "server tpcb mem err" );
                sys_timeout( 500, modbus_server_restart_timer, 0 );
            }
            break;

        case MODBUS_TCP_EVENT_CLOSE:
            /* close all registered client pcb */
            //logger_const_info( "evt_close" );
            sys_untimeout( modbus_server_restart_timer, 0 );
            for( i=0; i<MODBUS_TCP_NUM; i++ )
                do_modbus_tcp_client_close( clients[i] );
            if( modbus_tpcb != NULL )
            {
                tcp_close( modbus_tpcb );
                modbus_tpcb = NULL;
                logger_const_info( "server closed" );
            }
            break;

        case MODBUS_TCP_EVENT_REQUEST:
            //logger_const_info( "evt_packet" );
            client = (modbus_tcp_client_t *)(evt.data);
            head = (modbus_tcp_head_t*)client->buf;
            transaction_id = head->transaction_id;
            protocol_id = head->protocol_id;
            address = *(uint16_t*)(client->buf+sizeof(modbus_tcp_head_t));
            size = *(uint16_t*)(client->buf+sizeof(modbus_tcp_head_t)+2);
            swap_bytes( (uint8_t*)&transaction_id, ((uint8_t*)&transaction_id)+1 );
            swap_bytes( (uint8_t*)&protocol_id, ((uint8_t*)&protocol_id)+1 );
            swap_bytes( (uint8_t*)&address, ((uint8_t*)&address)+1 );
            swap_bytes( (uint8_t*)&size, ((uint8_t*)&size)+1 );
            //logger_printf_info( "TID 0x%04X, PID 0x%04X, LEN %u, UID 0x%04X, CODE %u, ADDR 0x%04X, SIZE %u",
            //    transaction_id, protocol_id, head->bytes, head->unit_id, head->function_code, address, size );
            payload = (uint16_t*)(client->buf+sizeof(modbus_tcp_head_t));
            i = do_process_modbus_tcp_packet( (modbus_tcp_head_t*)client->buf, address, size, payload, &write );
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
                client->tick_last = os_tick();
            }
            head->bytes = 2 + write;
            swap_bytes( (uint8_t*)&(head->bytes), ((uint8_t*)&(head->bytes))+1 );
            client->transaction_id = transaction_id;
            client->reply_len = sizeof(modbus_tcp_head_t) + write;
            client->pending_reply = 1;
            client->retries = 0;
            send_modbus_tcp_event( MODBUS_TCP_EVENT_REPLY, evt.data );
            break;

        case MODBUS_TCP_EVENT_REPLY:
            client = (modbus_tcp_client_t *)(evt.data);
            if( ! client->pending_reply )
                break;
            if( ERR_OK == tcp_write( client->tpcb, (char*)client->buf, client->reply_len, 1 ) )
            {
                client->pending_reply = 0;
                tcp_output( client->tpcb );
                break;
            }
            else
            {
                /* 10 * 500ms -> 5000ms total */
                client->retries += 1;
                if( client->retries <= 10 )
                {
                    logger_printf_error( "tcp_write err #%d 0x%08X, id=%d retry %d", 
                            client->client_id, client->tpcb, client->transaction_id, client->retries );
                    sys_timeout( 500, modbus_reply_resend_timer, client );
                }
                else
                    do_modbus_tcp_client_close( client );
            }
            break;

        default:
            break;
        }
    }
}

        
int cmd_modbus_tcp( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          'r', shell_str_reset, 0, "reset connections" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t reset_set=0;
    struct tcp_pcb *tpcb;
    modbus_tcp_client_t *client;
    int i;
    char buf[32];
    
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_reset ) == 0 )
            {
                reset_set = 1;
            }
        }            
        else
            STOP_AT_INVALID_ARGUMENT
    }

    if( reset_set )
    {
        return send_modbus_tcp_event( MODBUS_TCP_EVENT_RESET, 0 ) ? 0 : 1;
    }

 
    if( modbus_tpcb == NULL )
    {
        shell_write_line("stopped");
        return 0;
    }

    for( i=0; i<MODBUS_TCP_NUM; i++ )
    {
        client = clients[i];
        if( client == NULL )
            continue;
        tpcb = client->tpcb;
        shell_printf("#%d 0x%08X 0x%08X", client->client_id, client, (uint32_t)tpcb);
        sprintf_ip( buf, tpcb->remote_ip.addr, 0, 0 );
        shell_printf(" %s %d", buf, tpcb->remote_port);
        shell_printf(" %s", tcp_debug_state_str(tpcb->state));
        get_tick_time_str( buf, os_tick()-client->tick_connect, 0 );
        shell_printf(" %s", buf);
        shell_newline();
    } 

    return 0;
}


const shell_cmd_t cmd_tab_modbus_tcp[] = {
{   0, 'M', "modbustcp",  cmd_modbus_tcp, 
    "modbus control",
    "modbus"  },
{   CMD_END  } };


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
    
    shell_add_cmd_table( cmd_tab_modbus_tcp );
}

