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

#ifndef MEM_LIBC_MALLOC
#define MEM_LIBC_MALLOC                 0  /* 1 - use default malloc, 0 - use lwip mem management */
#endif

#if MEM_LIBC_MALLOC
#define mem_clib_malloc                 pvPortMalloc
#define mem_clib_calloc                 pvPortCalloc
#define mem_clib_free                   vPortFree
#endif

#define MEM_ALIGNMENT                   4  // 4Bytes for ARM

#ifndef MEM_SIZE
#define MEM_SIZE                        (10*1024)  // heap size
#endif

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
//#define LWIP_DEBUG_TIMERNAMES           1
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define ETHARP_DEBUG                    LWIP_DBG_ON
//#define NETIF_DEBUG                     LWIP_DBG_ON
//#define PBUF_DEBUG                      LWIP_DBG_ON
//#define API_LIB_DEBUG                   LWIP_DBG_ON
//#define SOCKETS_DEBUG                   LWIP_DBG_ON
//#define ICMP_DEBUG                      LWIP_DBG_ON
//#define IGMP_DEBUG                      LWIP_DBG_ON
//#define INET_DEBUG                      LWIP_DBG_ON
//#define IP_DEBUG                        LWIP_DBG_ON
//#define MEM_DEBUG                       LWIP_DBG_ON
//#define MEMP_DEBUG                      LWIP_DBG_ON
//#define SYS_DEBUG                       LWIP_DBG_ON
//#define TIMERS_DEBUG                    LWIP_DBG_ON
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
//#define TCP_FR_DEBUG                    LWIP_DBG_ON
//#define TCP_RTO_DEBUG                   LWIP_DBG_ON
//#define TCP_CWND_DEBUG                  LWIP_DBG_ON
//#define TCP_WND_DEBUG                   LWIP_DBG_ON
//#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
//#define TCP_RST_DEBUG                   LWIP_DBG_ON
//#define TCP_QLEN_DEBUG                  LWIP_DBG_ON
//#define TCPIP_DEBUG                     LWIP_DBG_ON
//#define UDP_DEBUG                       LWIP_DBG_ON
//#define SLIP_DEBUG                      LWIP_DBG_ON
//#define DHCP_DEBUG                      LWIP_DBG_ON
//#define DNS_DEBUG                       LWIP_DBG_ON
#include "task_logger.h"
#define LWIP_PLATFORM_DIAG(message)     do{logger_printf_debug2 message;}while(0)
#define LWIP_PLATFORM_ASSERT(message)   do{halt(message);}while(0)
#else
#define LWIP_DEBUG                      0
/* NOTE:
   in release mode, there are two different methods below */
/* 1: ignore ASSERT and the lwip stack MAY STILL WORKS (MAYBE NOT) */
//#define LWIP_PLATFORM_ASSERT(message)  /* define macro as ignored */
/* 2: halt the cpu on ASSERT, then reset by wdg */
#define LWIP_PLATFORM_ASSERT(message)   do{halt(message);}while(0)
#endif

#define LWIP_DNS                        1  // Enable DNS API
#define LWIP_DNS_SECURE                 0

#ifndef USE_NETBIOSNS
#define USE_NETBIOSNS                   0
#endif

#ifndef USE_NETBIOSNS_NAME
#define USE_NETBIOSNS_NAME              "MCUSH"
#endif

#ifndef NETBIOS_LWIP_NAME
#define NETBIOS_LWIP_NAME               "*"
#endif

#define TCPIP_THREAD_NAME               "tcpipT"
#define TCPIP_THREAD_STACKSIZE          4*1024
#define TCPIP_MBOX_SIZE                 5
#define DEFAULT_UDP_RECVMBOX_SIZE       2000
#define DEFAULT_TCP_RECVMBOX_SIZE       2000
#define DEFAULT_ACCEPTMBOX_SIZE         2000
#define DEFAULT_THREAD_STACKSIZE        2*1024
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
