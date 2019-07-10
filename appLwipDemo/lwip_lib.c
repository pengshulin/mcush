/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_logger.h"
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


/* dynamic ports range (refer to RFC 6335) */
#define LOCAL_PORT_START  0xC000
#define LOCAL_PORT_END    0xFFFF
uint16_t tcp_bind_random_port( struct tcp_pcb *pcb )
{
    static uint16_t port = 0, port2;
    if( port == 0 )
    {
        /* init random port (ignore start/end ports) */
        port = xTaskGetTickCount();
        while( (port <= LOCAL_PORT_START) || (port >= LOCAL_PORT_END) )
            port *= 997;  /* the largest prime number under 1000 */
    }
    port2 = port;
    while( tcp_bind( pcb, IP_ADDR_ANY, port ) != ERR_OK )
    {
        if( ++port > LOCAL_PORT_END )
            port = LOCAL_PORT_START;
        if( port == port2 )
            return 0;  /* failed to bind any address */
    }
    /* prepare local port for next round */
    port2 = port;
    if( ++port > LOCAL_PORT_END )
        port = LOCAL_PORT_START;
    return port2;
}


