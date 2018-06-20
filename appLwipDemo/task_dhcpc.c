/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "timers.h"
#include "task_logger.h"
#include "lwip_config.h"
#include "task_dhcpc.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "lwip/ip.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "ethernetif.h"
#include "LAN8742A.h"
#ifdef USE_LWIP_DEMO
  uint8_t httpd_started=0;
  #ifdef USE_LWIP_HTTPSERVER_SIMPLE
    #include "httpserver-netconn.h"
  #else
    extern void httpd_init(void);
  #endif
#endif

#define DHCP_START                  1
#define DHCP_WAIT_ADDRESS           2
#define DHCP_ADDRESS_ASSIGNED       3
#define DHCP_TIMEOUT                4
#define DHCP_LINK_DOWN              5


 
#define DHCPC_TIMER_PERIOD_MS   2000 

QueueHandle_t queue_dhcpc;
TimerHandle_t timer_dhcpc;

uint8_t dhcp_state;
struct netif gnetif;
/* TODO: move it to hal layer */
extern __IO uint32_t  EthStatus;  // in driver lan8742a.c

char mac_address_init[6]; 

   
/* load from file /?/mac and parse in format AA:BB:CC:DD:EE:FF */
int load_mac_from_conf_file(const char *fname)
{
    int fd;
    char buf[32];
    int i;
    unsigned int a,b,c,d,e,f;

    fd = mcush_open( fname, "r" );
    if( fd == 0 )
        return 0;

    i = mcush_read( fd, buf, 2*6+5 );
    if( i != (2*6+5) )
        goto err;
   
    buf[2*6+5] = 0; 
    i = sscanf( buf, "%x:%x:%x:%x:%x:%x", &a, &b, &c, &d, &e, &f );
    if( i != 6 )
        goto err;
   
    if( (a > 255) || (b > 255) || (c > 255) || (d > 255) || (e > 255) || (f > 255) )
        goto err;

    mac_address_init[0] = a;
    mac_address_init[1] = b;
    mac_address_init[2] = c;
    mac_address_init[3] = d;
    mac_address_init[4] = e;
    mac_address_init[5] = f;

    mcush_close(fd);
    return 1;
err:
    mcush_close(fd);
    return 0;
}


void send_dhcpc_event( uint8_t event )
{
    xQueueSend( queue_dhcpc, &event, portMAX_DELAY );
}


void timer_dhcpc_callback(TimerHandle_t *handle)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    const int8_t evt = DHCPC_EVENT_CHECK_TIMER;
    xQueueSendFromISR( queue_dhcpc, (void*)&evt, &xHigherPriorityTaskWoken );
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

 
void reset_address(void)
{
    ip_addr_t ipaddr, netmask, gw;

    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);
}


void logger_mac( const char *prompt, char *address, int shell_mode )
{
    const char format[] = "%s %02X:%02X:%02X:%02X:%02X:%02X";
    char buf[128];
    sprintf( buf, format, prompt,
            address[0], address[1], address[2], 
            address[3], address[4], address[5] );
    if( shell_mode )
        shell_write_line( buf );
    else
        logger_str( LOG_INFO, buf );
}

void logger_ip( const char *prompt, uint32_t address, int shell_mode )
{
    const char format[] = "%s %d.%d.%d.%d";
    char buf[128];
    sprintf( buf, format, prompt,
            (uint8_t)(address), (uint8_t)(address >> 8),
            (uint8_t)(address >> 16), (uint8_t)(address >> 24) );
    if( shell_mode )
        shell_write_line( buf );
    else
        logger_str( LOG_INFO, buf );
}


void do_lwip_init(void)
{
    ip_addr_t ipaddr, netmask, gw;

    ETH_BSP_Config();

    tcpip_init( NULL, NULL );

    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    
    if( !load_mac_from_conf_file("/s/mac") )
        load_mac_from_conf_file("/c/mac");
    logger_mac( "mac:", mac_address_init, 0 );
    
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&gnetif);

    if( EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG) )
    {
        netif_set_up(&gnetif);
        send_dhcpc_event( DHCPC_EVENT_NETIF_UP );
    }
    else
    {
        netif_set_down(&gnetif);
        send_dhcpc_event( DHCPC_EVENT_NETIF_DOWN );
    }

    netif_set_link_callback(&gnetif, ETH_link_callback);
}


void task_dhcpc_entry(void *p)
{
    uint8_t evt;

    do_lwip_init();
    xTimerStart( timer_dhcpc, 0 );
    while(1)
    {
        if( pdPASS != xQueueReceive( queue_dhcpc, &evt, portMAX_DELAY ) )
            halt( "dhcpc queue receive" );

        switch( evt )
        {
        case DHCPC_EVENT_NETIF_UP:
            logger_printf( LOG_INFO, "cable connected, dhcp discover...");
            gnetif.flags |= NETIF_FLAG_LINK_UP;
            reset_address();
            dhcp_state = DHCP_WAIT_ADDRESS;
            dhcp_start(&gnetif);
            break;

        case DHCPC_EVENT_NETIF_DOWN:
            logger_printf( LOG_INFO, "cable disconnected");
            gnetif.flags &= ~NETIF_FLAG_LINK_UP;
            dhcp_state = DHCP_LINK_DOWN;
            /* TODO: add net down event */
            break;

        case DHCPC_EVENT_CHECK_TIMER:
            if( dhcp_state == DHCP_WAIT_ADDRESS )
            {
                if( gnetif.ip_addr.addr != 0 )
                {
                    dhcp_state = DHCP_ADDRESS_ASSIGNED;
                    dhcp_stop(&gnetif);
                    logger_ip( "dhcp ip:", gnetif.ip_addr.addr, 0 );    
                    logger_ip( "dhcp netmask:", gnetif.netmask.addr, 0 );
                    logger_ip( "dhcp gateway:", gnetif.gw.addr, 0 );
                    /* TODO: add net up event */
#ifdef USE_LWIP_DEMO
                    if( httpd_started == 0 )
                    {
    #ifdef USE_LWIP_HTTPSERVER_SIMPLE
                        http_server_netconn_init();
    #else
                        httpd_init();
    #endif
                        httpd_started=1;
                    }
#endif
                }
            } 
            
            /* Check link status */
            ETH_CheckLinkStatus( ETHERNET_PHY_ADDRESS ); 
            break; 
 
        default:
            //logger_printf( LOG_INFO, "unexpected dhcp event");
            break;
        }
    }
}


int cmd_lwip( int argc, char *argv[] )
{
    stats_display();
    return 0;
}


int cmd_netstat( int argc, char *argv[] )
{
    switch( dhcp_state )
    {
    case DHCP_LINK_DOWN:
        shell_write_line("Disconnected");
        break;
    case DHCP_WAIT_ADDRESS:
        shell_write_line("DHCP waiting");
        break;
    case DHCP_ADDRESS_ASSIGNED:
        logger_mac( "mac:", mac_address_init, 1 );
        logger_ip( "ip:", gnetif.ip_addr.addr, 1 );    
        logger_ip( "netmask:", gnetif.netmask.addr, 1 );
        logger_ip( "gateway:", gnetif.gw.addr, 1 );
        break;
    default:
        break;
    } 
    return 0;
}



extern int cmd_ping( int argc, char *argv[] );

const shell_cmd_t cmd_tab_lwip[] = {
{   0, 0, "lwip",  cmd_lwip, 
    "lwip stastics",
    "lwip"  },
{   0, 'n', "netstat",  cmd_netstat, 
    "show net status",
    "netstat"  },
{   0, 'p', "ping",  cmd_ping, 
    "ping remote host",
    "ping host/ip"  },
{   CMD_END  } };



void task_dhcpc_init(void)
{
    shell_add_cmd_table( cmd_tab_lwip );
    TaskHandle_t  task_dhcpc;

    queue_dhcpc = xQueueCreate( TASK_DHCPC_QUEUE_SIZE, (unsigned portBASE_TYPE)sizeof(uint8_t) );
    if( !queue_dhcpc )
        halt( "create dhdpc queue" );

    timer_dhcpc = xTimerCreate( (const char * const)"dhcpc", 
                                DHCPC_TIMER_PERIOD_MS / portTICK_RATE_MS,
                                pdTRUE, 0, (TimerCallbackFunction_t)timer_dhcpc_callback );
    if( !timer_dhcpc )
        halt( "create dhdpc timer" );
    
    xTaskCreate(task_dhcpc_entry, (const char *)"dhcpcT", TASK_DHCPC_STACK_SIZE, NULL, TASK_DHCPC_PRIORITY, &task_dhcpc);
    if( !task_dhcpc )
        halt("create dhcpc task");
    mcushTaskAddToRegistered((void*)task_dhcpc);
}


