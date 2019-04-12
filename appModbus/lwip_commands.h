#ifndef __LWIP_COMMANDS_H__
#define __LWIP_COMMANDS_H__

#include "lwip/ip_addr.h"


void ping_init(const ip_addr_t* ping_addr);

void ping_send_now(void);

#endif
