/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __LWIP_LIB_H__
#define __LWIP_LIB_H__
#include "mcush.h"
#include "lwip/tcp.h"

uint16_t tcp_bind_random_port( struct tcp_pcb *pcb );

char *sprintf_mac( char *buf, char *address, const char *prefix, const char *suffix );
char *sprintf_ip( char *buf, uint32_t address, const char *prefix, const char *suffix );
char *sprintf_ip_mask_gw( char *buf, uint32_t ip, uint32_t mask, uint32_t gw, const char *prefix, const char *suffix );




#endif
