/* MCUSH designed by Peng Shulin, all rights reserved. */
#if USE_CMD_NC
#include "mcush.h"
#include "timers.h"
#include "task_logger.h"
#include "task_dhcpc.h"
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
#include "lwip_lib.h"

#ifndef NC_DEBUG
#define NC_DEBUG     LWIP_DBG_ON
#endif

LOGGER_MODULE_NAME("nc");

#define NC_DNS_RESOLVE_TIMEOUT_S  5

#define NC_SEND_BUF_LEN   512 

typedef struct _nc_cb_t
{
    const char *server_hostname;
    ip_addr_t server_ip;
    int server_port;
    uint8_t dns_resolved;
    uint8_t done;
    uint8_t error;
    uint8_t cancel;
    uint8_t connected;
    uint8_t pending;
    struct tcp_pcb *pcb;
    uint8_t timeout;
    char send_buf[NC_SEND_BUF_LEN];
    uint32_t send_len;
} nc_cb_t;
nc_cb_t *ncb;


void nc_hostname_dns_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    (void)name;
    (void)arg;
    if( ncb == NULL )
        return;
    if( ipaddr )
        ncb->server_ip = *ipaddr;
    else
        ncb->server_ip.addr = 0;
    ncb->dns_resolved = 1;
}


err_t nc_tcp_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *p2;

    (void)arg;
    if( err == ERR_OK )
    {
        if( p != NULL )  /* normal receive */
        {
            p2 = p;
            while(p2 != NULL)
            {
                shell_write( p2->payload, p2->len );  
                p2 = p2->next;
            }
            tcp_recved(pcb, p->tot_len);
            pbuf_free(p);
        }
        else  /* connection closed */
        {
            tcp_abort(pcb);
            ncb->error = 1; 
        }
    }
    else
    {
        tcp_abort(pcb);
        ncb->error = 1; 
    }
    return ERR_OK;
}

static void nc_tcp_error_cb(void *arg, err_t err)
{
    (void)arg;
    (void)err;
    if( ncb->cancel )
        return;
    ncb->error = 1;
}


static err_t nc_tcp_poll_cb(void *arg, struct tcp_pcb *pcb)
{
    (void)arg;
    ncb->timeout++;
    if( ncb->timeout > 20 )
    {
        tcp_abort(pcb);
        ncb->error = 1;
    }
    return ERR_OK;
}


static err_t nc_tcp_sent_cb(void *arg, struct tcp_pcb *pcb, uint16_t len)
{
    (void)arg;
    (void)pcb;
    (void)len;
    ncb->timeout = 0;
    return ERR_OK;
}


static err_t nc_tcp_connected_cb(void *arg, struct tcp_pcb *pcb, err_t err)
{
    (void)arg;
    if( err != ERR_OK )
    {
        tcp_abort(pcb);
        ncb->done = 1;
    }
    else
    {
        shell_write_line( shell_str_connected );
        ncb->connected = 1;
        tcp_recv(pcb, nc_tcp_recv_cb);
        tcp_err(pcb, nc_tcp_error_cb);
        tcp_poll(pcb, nc_tcp_poll_cb, 10);
        tcp_sent(pcb, nc_tcp_sent_cb);
    }
    return ERR_OK;
}


int cmd_nc( int argc, char *argv[] )
{
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "host", 0, "hostname or ip addr" },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "port", 0, "port" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t ip_set=0, hostname_set=0, port_set=0;
    nc_cb_t lncb;
    char chr;
    ip_addr_t ipaddr;
    int i;
    char buf[64];

    memset( &lncb, 0, sizeof(nc_cb_t) );
    ncb = &lncb;
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "host" ) == 0 )
            {
                /* parse ip addr, check if dns resolve is need */
                if( ipaddr_aton( opt.value, &ipaddr ) )
                {
                    ncb->server_ip.addr = ipaddr.addr;
                    ip_set = 1;
                }
                else
                {
                    ncb->server_hostname = opt.value;
                    hostname_set = 1;
                }
            }
            else if( strcmp( opt.spec->name, "port" ) == 0 )
            {
                if( parse_int( opt.value, &i ) )
                {
                    ncb->server_port = i;
                    port_set = 1;
                }
                else
                {
                    shell_write_err( shell_str_port );
                    return -1;
                }
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( ! ip_set && ! hostname_set )
    {
        shell_write_line("host/ip not set");
        return 1;
    }

    if( ! port_set )
    {
        shell_write_line("port not set");
        return 1;
    }
    else if( (ncb->server_port <= 0) || (ncb->server_port >= 65535) )
    {
        shell_write_err( shell_str_port );
        return 1;
    }

    /* dns resolve first */
    if( hostname_set )
    {
        shell_printf("dns resolve: %s\n", ncb->server_hostname);
        if( ERR_OK == dns_gethostbyname( ncb->server_hostname, &ipaddr, (dns_found_callback)nc_hostname_dns_cb, NULL ) )
        {
            ncb->server_ip.addr = ipaddr.addr;
            ncb->dns_resolved = 1;
        }
        else
        {
            /* wait for Ctrl-C */
            for( i=NC_DNS_RESOLVE_TIMEOUT_S*10; i; i-- )
            {
                if( ncb->dns_resolved )
                    break;
                if( shell_driver_read_char_blocked(&chr, 100*configTICK_RATE_HZ/1000) != -1 )
                {
                    if( chr == 0x03 ) /* Ctrl-C for stop */
                    {
                        ncb = 0;
                        return 0;
                    }
                }
            }
            if( i == 0 )
            {
                shell_printf("dns timeout\n");
                ncb = 0;
                return 1;
            }
        }
        if( ncb->server_ip.addr )
        {
            shell_write_line( sprintf_ip( buf, ncb->server_ip.addr, "dns resolved", 0 ) );
        }
        else
        {
            shell_write_line("dns resolve failed");
            ncb = 0;
            return 1;
        }
    } 

    /* tcp connect */
    ncb->pcb = tcp_new();
    if( ncb->pcb == NULL )
    {
        shell_write_err( shell_str_memory );
        return 1;
    }
    else
    {
        /* connect and get file */
        if( ! tcp_bind_random_port(ncb->pcb) )
        {
            shell_write_err( "bind" );
            return 1;
        }
        tcp_connect( ncb->pcb, &ncb->server_ip, ncb->server_port, nc_tcp_connected_cb );

        /* wait for done or Ctrl-C */
        while( 1 )
        {
            if( ncb->done || ncb->error )
                break;
            if( shell_driver_read_char_blocked(&chr, 100*configTICK_RATE_HZ/1000) != -1 )
            {
                if( chr == 0x03 ) /* Ctrl-C for stop */
                {
                    ncb->cancel = 1;
                    tcp_abort( ncb->pcb );
                    break;
                }
                else if( ncb->connected )
                {
                    /* send input line */
                    shell_write_char( chr );
                    ncb->send_buf[ncb->send_len++] = chr;
                    ncb->pending = 1;
                    if( ncb->send_len >= NC_SEND_BUF_LEN )
                    {
                        tcp_write( ncb->pcb, ncb->send_buf, NC_SEND_BUF_LEN, TCP_WRITE_FLAG_COPY );
                        tcp_output( ncb->pcb );
                        ncb->pending = 0;
                        ncb->send_len = 0;
                    }
                }
            }
            else
            {
                if( ncb->connected && ncb->pending )
                {
                    tcp_write( ncb->pcb, ncb->send_buf, ncb->send_len, TCP_WRITE_FLAG_COPY );
                    tcp_output( ncb->pcb );
                    ncb->pending = 0;
                    ncb->send_len = 0;
                }
            }
        }
    }
    
    ncb = 0;
    shell_write_char( '\n' );
    return lncb.error ? 1 : 0;
}

#endif
