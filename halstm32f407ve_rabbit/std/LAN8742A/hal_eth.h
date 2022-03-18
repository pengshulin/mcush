#ifndef _HAL_ETH_H_
#define _HAL_ETH_H_

#include "LAN8742A.h"

#define RMII_MODE


int hal_eth_init(void);
int hal_eth_is_linked(void);
void hal_eth_check_link_status(int phy_addr);
void hal_eth_link_callback(struct netif *netif);


#endif
