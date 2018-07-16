/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "mcush.h"

#define SYS_LIGHTWEIGHT_PROT            0  // Inter-tasks protection

#define ETHARP_TRUST_IP_MAC             0
#define IP_REASSEMBLY                   0
#define IP_FRAG                         0
#define ARP_QUEUEING                    0

#define NO_SYS                          0

#define MEM_ALIGNMENT                   4  // 4Bytes for ARM

#define MEM_SIZE                        (10*1024)  // heap size

#define MEMP_NUM_PBUF                   100  // memp size
#define MEMP_NUM_UDP_PCB                6  // UDP protocol control blocks
#define MEMP_NUM_TCP_PCB                20  // simulatenously active TCP connections
#define MEMP_NUM_TCP_PCB_LISTEN         5  // listening TCP connections
#define MEMP_NUM_TCP_SEG                20  // simultaneously queued TCP segments
#define MEMP_NUM_SYS_TIMEOUT            10  // simulateously active timeouts


#define PBUF_POOL_SIZE                  40  // pool size
#define PBUF_POOL_BUFSIZE               500  // pool buf size


#define LWIP_TCP                        1
#define TCP_TTL                         255

#define TCP_QUEUE_OOSEQ                 0  // TCP segments in queue support

#define TCP_MSS                         (1500 - 40)  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

#define TCP_SND_BUF                     (5*TCP_MSS) /* TCP sender buffer space (bytes). */

/* TCP sender buffer space (pbufs). This must be at least as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
#define TCP_SND_QUEUELEN                (4* TCP_SND_BUF/TCP_MSS)

#define TCP_WND                         (2*TCP_MSS)  /* TCP receive window. */


#define LWIP_ICMP                       1

#define LWIP_DHCP                       1
#define LWIP_DHCP_MAX_DNS_SERVERS       2
//#define LWIP_DHCP_GET_NTP_SRV           1


#define LWIP_UDP                        1
#define UDP_TTL                         255


#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              1
#define LWIP_PROVIDE_ERRNO              1

#define LWIP_NETIF_LINK_CALLBACK        1  // callback support when interface broken

#define LWIP_NETCONN                    1  // Enable Netconn API (api_lib.c)

#define LWIP_SOCKET                     1  // Enable Socket API (sockets.c)

#if DEBUG
#define LWIP_DEBUG                      1
#else
#define LWIP_DEBUG                      0
#endif

#define LWIP_DNS                        1  // Enable DNS API
#define LWIP_DNS_SECURE                 0


#define TCPIP_THREAD_NAME               "tcpipT"
#define TCPIP_THREAD_STACKSIZE          1000
#define TCPIP_MBOX_SIZE                 5
#define DEFAULT_UDP_RECVMBOX_SIZE       2000
#define DEFAULT_TCP_RECVMBOX_SIZE       2000
#define DEFAULT_ACCEPTMBOX_SIZE         2000
#define DEFAULT_THREAD_STACKSIZE        500
//#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 2) 
#define TCPIP_THREAD_PRIO               (MCUSH_PRIORITY - 1)

#define LWIP_COMPAT_MUTEX               0


#define LWIP_RAW                        1

#define LWIP_TIMEVAL_PRIVATE            0


/****************************************************************************
 * HARDWARE ACCELERATION SUPPORT FOR STM32F4x7
 * CHECKSUM FOR IP, UDP, TCP AND ICMP PACKETS
 ****************************************************************************/  
#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0 
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif



#endif
