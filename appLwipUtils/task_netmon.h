/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_NETMON_H_
#define _TASK_NETMON_H_


#define TASK_NETMON_NAME         "netmonT"
#define TASK_NETMON_STACK_SIZE   (3*1024)
#define TASK_NETMON_PRIORITY     OS_PRIORITY_LOW
#define TASK_NETMON_QUEUE_SIZE   4


#define NETMON_EVENT_NETIF_UP     1
#define NETMON_EVENT_NETIF_DOWN   2
#define NETMON_EVENT_CHECK_TIMER  3
#define NETMON_EVENT_START_DEMO   10

#define NETMON_DISCOVER_TIMEOUT_S   60

#if USE_NET_CHANGE_HOOK
void net_state_change_hook(int connected);
#endif
 
int send_netmon_event(uint8_t event); 

void task_netmon_init(void);

#endif

