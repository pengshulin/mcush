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


static char *sprintf_join_prefix_suffix( char *buf, const char *prefix, const char *content, const char *suffix )
{
    if( buf == NULL )
        return NULL;

    if( prefix )
    {
        strcpy( buf, prefix );
        strcat( buf, " " );
        strcat( buf, content );
    }
    else
        strcpy( buf, content );

    if( suffix )
    {
        strcat( buf, " " );
        strcat( buf, suffix );
    }
    return buf; 
}


char *sprintf_ip( char *buf, uint32_t address, const char *prefix, const char *suffix )
{
    char buf2[16];
 
    sprintf( buf2, "%u.%u.%u.%u", 
            (uint8_t)(address), (uint8_t)(address >> 8),
            (uint8_t)(address >> 16), (uint8_t)(address >> 24) );
    return sprintf_join_prefix_suffix( buf, prefix, buf2, suffix );
}


char *sprintf_ip_mask_gw( char *buf, uint32_t ip, uint32_t mask, uint32_t gw, const char *prefix, const char *suffix )
{
    char buf2[256];

    sprintf( buf2, "ip(%u.%u.%u.%u) netmask(%u.%u.%u.%u) gateway(%u.%u.%u.%u)", 
            (uint8_t)(ip), (uint8_t)(ip >> 8), (uint8_t)(ip >> 16), (uint8_t)(ip >> 24),
            (uint8_t)(mask), (uint8_t)(mask >> 8), (uint8_t)(mask >> 16), (uint8_t)(mask >> 24),
            (uint8_t)(gw), (uint8_t)(gw >> 8), (uint8_t)(gw >> 16), (uint8_t)(gw >> 24) );
    return sprintf_join_prefix_suffix( buf, prefix, buf2, suffix );
}


char *sprintf_mac( char *buf, char *address, const char *prefix, const char *suffix )
{
    char buf2[32];
 
    sprintf( buf2, "%02X:%02X:%02X:%02X:%02X:%02X", 
            address[0], address[1], address[2], 
            address[3], address[4], address[5] );
    return sprintf_join_prefix_suffix( buf, prefix, buf2, suffix );
}
