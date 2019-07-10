/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_DHCPC_H_
#define _TASK_DHCPC_H_


#define TASK_DHCPC_STACK_SIZE  (2*1024)
#define TASK_DHCPC_PRIORITY    (MCUSH_PRIORITY - 1)
#define TASK_DHCPC_QUEUE_SIZE  (4)


#define DHCPC_EVENT_NETIF_UP     1
#define DHCPC_EVENT_NETIF_DOWN   2
#define DHCPC_EVENT_CHECK_TIMER  3
#define DHCPC_EVENT_START_DEMO   10

#define DHCPC_DISCOVER_TIMEOUT_S   60

#if USE_NET_CHANGE_HOOK
void net_state_change_hook(int connected);
#endif
 
int send_dhcpc_event(uint8_t event); 

void task_dhcpc_init(void);
void logger_ip( const char *prompt, uint32_t address, int shell_mode );
void logger_mac( const char *prompt, char *address, int shell_mode );

#endif

