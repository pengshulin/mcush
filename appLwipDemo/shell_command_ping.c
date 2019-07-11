/* MCUSH designed by Peng Shulin, all rights reserved. */
#if USE_CMD_PING
#include "mcush.h"
#include "timers.h"
#include "task_logger.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/dns.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"
#include "lwip_commands.h"
#include "lwip_lib.h"

LOGGER_MODULE_NAME("ping");

/**
 * PING_DEBUG: Enable debugging for PING.
 */
#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 1000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     1000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

#define PING_DNS_RESOLVE_TIMEOUT_S  5
typedef struct _ping_cb_t
{
    struct raw_pcb *ping_pcb;
    u16_t ping_seq_num;
    u32_t ping_time;
    ip_addr_t ping_target;
    const char *ping_hostname;
    uint8_t dns_resolved;
} ping_cb_t;

ping_cb_t *ping_pcb;

            
extern void logger_ip( const char *prompt, uint32_t address, int shell_mode );

void dns_ping_hostname_check_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    if( ping_pcb )
    {
        if( ipaddr )
            ping_pcb->ping_target = *ipaddr;
        else
            ping_pcb->ping_target.addr = 0;
        ping_pcb->dns_resolved = 1;
    }
}



void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = lwip_htons(++(ping_pcb->ping_seq_num));

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}


u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;
    uint8_t *adr;
        
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);

    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
        pbuf_header(p, -PBUF_IP_HLEN) == 0) {
      iecho = (struct icmp_echo_hdr *)p->payload;

      if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_pcb->ping_seq_num))) {
        adr = (uint8_t*)&(addr->addr);
        shell_printf("ping: recv %u.%u.%u.%u  %u ms\n", adr[0], adr[1], adr[2], adr[3],
            (unsigned int)(sys_now()-ping_pcb->ping_time));

        /* do some ping result processing */
        PING_RESULT(1);
        pbuf_free(p);
        return 1; /* eat the packet */
      }
      /* not eaten, restore original packet */
      pbuf_header(p, PBUF_IP_HLEN);
    }

    return 0; /* don't eat the packet */
}

void ping_send(struct raw_pcb *raw, ip_addr_t *addr)
{
    struct pbuf *p;
    struct icmp_echo_hdr *iecho;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    uint8_t *adr = (uint8_t*)&(addr->addr);

    shell_printf("ping: send %u.%u.%u.%u\n", adr[0], adr[1], adr[2], adr[3] );
    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

    p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
    if (!p)
      return;
    if ((p->len == p->tot_len) && (p->next == NULL)) {
        iecho = (struct icmp_echo_hdr *)p->payload;

        ping_prepare_echo(iecho, (u16_t)ping_size);

        raw_sendto(raw, p, addr);
        ping_pcb->ping_time = sys_now();
    }
    pbuf_free(p);
}


void ping_timeout(void *arg)
{
    struct raw_pcb *pcb = (struct raw_pcb*)arg;

    LWIP_ASSERT("ping_timeout: no pcb given!", pcb != NULL);

    //ip_addr_copy_from_ip4(ping_target, ping_pcb->ping_target);
    ping_send(pcb, &(ping_pcb->ping_target));
    sys_timeout(PING_DELAY, ping_timeout, pcb);
}


void ping_send_now(void)
{
    //ip_addr_t ping_target;
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb->ping_pcb != NULL);
    //ip_addr_copy_from_ip4(ping_target, ping_pcb->ping_target);
    ping_send(ping_pcb->ping_pcb, &(ping_pcb->ping_target));
}


int cmd_ping( int argc, char *argv[] )
{
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "host", 0, "hostname or ip addr" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint32_t i;
    uint8_t ip_set=0, hostname_set=0;
    ping_cb_t pcb;
    char chr;
    ip_addr_t ipaddr;
    char buf[64];

    ping_pcb = 0;
    memset( &pcb, 0, sizeof(ping_cb_t) );

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "host" ) == 0 )
            {
                /* parse ip addr */
                if( ipaddr_aton( opt.value, &ipaddr ) )
                {
                    pcb.ping_target.addr = ipaddr.addr;
                    ip_set = 1;
                }
                else
                {
                    pcb.ping_hostname = opt.value;
                    hostname_set = 1;
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

    ping_pcb = &pcb;
    if( hostname_set )
    {
        shell_printf("dns resolve: %s\n", pcb.ping_hostname);
        if( ERR_OK == dns_gethostbyname( pcb.ping_hostname, &ipaddr, (dns_found_callback)dns_ping_hostname_check_cb, NULL ) )
        {
            pcb.ping_target.addr = ipaddr.addr;
            pcb.dns_resolved = 1;
        }
        else
        {
            /* wait for Ctrl-C */
            for( i=PING_DNS_RESOLVE_TIMEOUT_S*10; i; i-- )
            {
                if( pcb.dns_resolved )
                    break;
                if( shell_driver_read_char_blocked(&chr, 100*configTICK_RATE_HZ/1000) != -1 )
                {
                    if( chr == 0x03 ) /* Ctrl-C for stop */
                    {
                        ping_pcb = 0;
                        return 0;
                    }
                }
            }
            if( i == 0 )
            {
                shell_printf("dns timeout\n");
                ping_pcb = 0;
                return 1;
            }
        }
        if( pcb.ping_target.addr )
        {
            shell_write_line( sprintf_ip(buf, pcb.ping_target.addr, "dns resolved:", 0) ); 
        }
        else
        {
            shell_write_line("dns resolve failed");
            ping_pcb = 0;
            return 1;
        }
    } 

    pcb.ping_pcb = raw_new(IP_PROTO_ICMP);
    LWIP_ASSERT("ping_pcb != NULL", pcb.ping_pcb != NULL);
    raw_recv(pcb.ping_pcb, ping_recv, NULL);
    raw_bind(pcb.ping_pcb, IP_ADDR_ANY);
    sys_timeout(PING_DELAY, ping_timeout, pcb.ping_pcb);
    ping_send_now();

    /* wait for Ctrl-C */
    while( 1 )
    {
        if( shell_driver_read_char_blocked(&chr, portMAX_DELAY) != -1 )
        {
            if( chr == 0x03 ) /* Ctrl-C for stop */
            {
                break;
                //return 0;
            }
        }
    }
 
    sys_untimeout(ping_timeout, pcb.ping_pcb);
    raw_remove(pcb.ping_pcb);
    ping_pcb = 0;
    return 0;
}

#endif
