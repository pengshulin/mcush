/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __LWIP_LIB_H__
#define __LWIP_LIB_H__
#include "mcush.h"
#include "lwip/tcp.h"

uint16_t tcp_bind_random_port( struct tcp_pcb *pcb );

int split_url( const char *url, char **protocol, char **server, int *port, char **pathfile );



#endif
