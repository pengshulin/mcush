/* DHCP Client task
 * 
 * MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
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
#include "lwip_lib.h"
#include "hal_eth.h"
#ifdef USE_LWIP_DEMO
  uint8_t lwip_demo_tasks_started=0;
  #ifdef USE_LWIP_HTTPSERVER_SIMPLE
    #include "httpserver-netconn.h"
  #else
    extern void httpd_init(void);
  #endif
    #include "shell_example.h"
#endif
#if USE_NETBIOSNS
#include "lwip/apps/netbiosns.h"
#endif

LOGGER_MODULE_NAME("dhcpc");

#define DHCP_START                  1
#define DHCP_WAIT_ADDRESS           2
#define DHCP_ADDRESS_ASSIGNED       3
#define DHCP_TIMEOUT                4
#define DHCP_LINK_DOWN              5
#define DHCP_ADDRESS_MANUAL         6

 
#define DHCPC_TIMER_PERIOD_MS   2000 

os_queue_handle_t queue_dhcpc;
os_timer_handle_t timer_dhcpc;

uint8_t dhcp_state, ip_manual;
struct netif gnetif;
    
const ip_addr_t dns_google={.addr=0x08080808};  // 8.8.8.8
const ip_addr_t dns_google2={.addr=0x04040808};  // 8.8.4.4
const ip_addr_t dns_alibaba={.addr=0x050505DF};  // 223.5.5.5
const ip_addr_t dns_alibaba2={.addr=0x060606DF};  // 223.6.6.6
const ip_addr_t dns_114={.addr=0x72727272};  // 114.114.114.114
const ip_addr_t dns_114_2={.addr=0x73737272};  // 114.114.115.115
const ip_addr_t dns_baidu={.addr=0x4CB4B4B4};  // 180.76.76.76
const ip_addr_t dns_open={.addr=0xDCDC43D0};  // 208.67.220.220
const ip_addr_t dns_open2={.addr=0xDEDC43D0};  // 208.67.222.222

char mac_address_init[6]; 

  

/* mac address config file (/?/mac) ascii contents:
   line 1: AA:BB:CC:DD:EE:FF
 */
int load_mac_from_conf_file(const char *fname)
{
    char buf[64];

    if( ! mcush_file_load_string( fname, buf, 64 ) )
        return 0;
    return parse_mac_addr( strip(buf), mac_address_init );
}


/* ipv4 address config file (/?/ip) ascii contents:
   line 1: aaa.bbb.ccc.ddd   (ip)
   line 2: aaa.bbb.ccc.ddd   (netmask)
   line 3: aaa.bbb.ccc.ddd   (gateway)
 */
int load_ip_from_conf_file(const char *fname, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway )
{
    char buf[128];

    if( ! mcush_file_load_string( fname, buf, 128 ) )
        return 0;
    return parse_ip_netmask_gateway( buf, (char*)ipaddr, (char*)netmask, (char*)gateway );
}


int send_dhcpc_event( uint8_t event )
{
    return os_queue_put( queue_dhcpc, &event, 0 );
}


void timer_dhcpc_callback( os_timer_handle_t timer )
{
    const int8_t evt = DHCPC_EVENT_CHECK_TIMER;

    (void)timer;
    os_queue_put_isr( queue_dhcpc, (void*)&evt );
}

 
void reset_address(void)
{
    ip_addr_t ipaddr, netmask, gw;

    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);
}


void do_lwip_init(void)
{
    ip_addr_t ipaddr, netmask, gateway;
    char buf[256];
    unsigned int hash;
    int len, no_config;
    int i;

    hal_eth_init();

    tcpip_init( NULL, NULL );
 
    no_config = 0; 
    if( !load_mac_from_conf_file("/s/mac") && !load_mac_from_conf_file("/c/mac") )
    {
        no_config = 1; 
        /* fixed address: may conflicts when there are two or more devices */
        //memcpy( mac_address_init, (void*)"\x00\x11\x22\x33\x44\x55", 6 );  /* simple integer */
        memcpy( mac_address_init, (void*)"\x4C\x4B\x53\x4F\x46\x54", 6 ); /* LKSOFT */
        /* dynamic address: use serial number to encrypt the last 3 bytes of mac addr */
        len = hal_get_serial_number(buf);
        if( len )
        {
            for( i=0, hash=0x534F4654; i<len; i++ )
            {
                //hash = hash * 521 ^ ((hash>>8)&0xFFFF);
                //hash ^= buf[i]<<(8*(i%3));
                hash ^= ((hash << 3) + buf[i] + (hash >> 2));  /* simple and quick */
            }
            mac_address_init[3] = (hash>>16)&0xFF; 
            mac_address_init[4] = (hash>>8)&0xFF; 
            mac_address_init[5] = hash&0xFF; 
        }
    }
    sprintf_mac( buf, mac_address_init, "mac:", 0 );
    if( no_config )
        strcat( buf, " (no config)" );
    logger_info( buf );
 
    if( !load_ip_from_conf_file("/s/ip", &ipaddr, &netmask, &gateway) && 
        !load_ip_from_conf_file("/c/ip", &ipaddr, &netmask, &gateway) )
    {
        logger_const_warn( "no ip config" );
        ipaddr.addr = 0;
        netmask.addr = 0;
        gateway.addr = 0;
        ip_manual = 0;
    }
    else
    {
        logger_info( sprintf_ip_mask_gw( buf, ipaddr.addr, netmask.addr, gateway.addr, "config", 0 ) );
        ip_manual = 1;
        dns_setserver( 0, (const ip_addr_t *)&gateway );
#if DNS_MAX_SERVERS > 1
        dns_setserver( 1, (const ip_addr_t *)&dns_google );
        //dns_setserver( 1, (const ip_addr_t *)&dns_114 );
#endif
    }
    
    netif_add(&gnetif, &ipaddr, &netmask, &gateway, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&gnetif);

    if( hal_eth_is_linked() )
    {
        netif_set_up(&gnetif);
        send_dhcpc_event( DHCPC_EVENT_NETIF_UP );
    }
    else
    {
        netif_set_down(&gnetif);
        send_dhcpc_event( DHCPC_EVENT_NETIF_DOWN );
    }

    netif_set_link_callback(&gnetif, hal_eth_link_callback);
}


void task_dhcpc_entry(void *arg)
{
    char buf[256];
    uint8_t evt;
    int i;

    (void)arg;

    do_lwip_init();
    os_timer_start( timer_dhcpc );

    while(1)
    {
        if( os_queue_get( queue_dhcpc, &evt, -1 ) == 0 )
            continue;

        switch( evt )
        {
        case DHCPC_EVENT_NETIF_UP:
            logger_const_info( "cable connected" );
            gnetif.flags |= NETIF_FLAG_LINK_UP;
            if( ip_manual ) 
            {
                dhcp_state = DHCP_ADDRESS_MANUAL;
#if USE_NET_CHANGE_HOOK
                net_state_change_hook(1);
#if USE_LWIP_DEMO
                send_dhcpc_event( DHCPC_EVENT_START_DEMO );
#endif
#endif
            }
            else
            {
                logger_const_info( "dhcp discover..." );
                reset_address();
                dhcp_state = DHCP_WAIT_ADDRESS;
                dhcp_start(&gnetif);
            }
            break;

        case DHCPC_EVENT_NETIF_DOWN:
            logger_const_info( "cable disconnected" );
            gnetif.flags &= ~NETIF_FLAG_LINK_UP;
            dhcp_state = DHCP_LINK_DOWN;
            if( ip_manual ) 
            {
            }
            else
            {
                dhcp_stop(&gnetif);
            }
#if USE_NETBIOSNS && defined(NETBIOS_LWIP_NAME)
            logger_const_info( "netbiosns stop" );
            netbiosns_stop();
#endif
#if USE_NET_CHANGE_HOOK
            net_state_change_hook(0);
#endif
            break;

        case DHCPC_EVENT_CHECK_TIMER:
            if( dhcp_state == DHCP_WAIT_ADDRESS )
            {
                if( gnetif.ip_addr.addr != 0 )
                {
                    dhcp_state = DHCP_ADDRESS_ASSIGNED;
                    logger_info( sprintf_ip_mask_gw( buf, gnetif.ip_addr.addr, gnetif.netmask.addr, gnetif.gw.addr, "assigned", 0 ) );
                    strcpy( buf, "assigned" );
                    for( i=0; i<DNS_MAX_SERVERS; i++ )
                    {
                        sprintf( buf+strlen(buf), " dns%u: ", i+1 );
                        sprintf_ip( buf+strlen(buf), dns_getserver(i)->addr, 0, "" );
                    }
                    logger_info( buf );
#if USE_NETBIOSNS && defined(NETBIOS_LWIP_NAME)
                    logger_const_info( "netbiosns start" );
                    netbiosns_init();
#endif

#if USE_NET_CHANGE_HOOK
                    net_state_change_hook(1);
#if USE_LWIP_DEMO
                    send_dhcpc_event( DHCPC_EVENT_START_DEMO );
#endif
#endif
                }
            } 
            
            /* Check link status */
            hal_eth_check_link_status( 0 ); 
            break; 
 
#if USE_LWIP_DEMO
        case DHCPC_EVENT_START_DEMO:
            if( lwip_demo_tasks_started )
                break;

#if USE_LWIP_HTTPSERVER_SIMPLE
            http_server_netconn_init();
#else
            httpd_init();
#endif
#if USE_LWIP_SHELL_EXAMPLE
            shell_example_init();
#endif
            lwip_demo_tasks_started=1;
            break;
#endif

        default:
            //logger_const_info( "unexpected dhcp event" );
            break;
        }
    }
}


int cmd_lwip( int argc, char *argv[] )
{
    (void)argc;
    (void)argv;
    stats_display();
    return 0;
}


int cmd_netstat( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED, 
          'c', shell_str_cmd, shell_str_command, "info|up|down|dhcp|ip|dns" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    const char *cmd=0;
    int i;
    char buf[64], succ;
    char *input=0;
    ip_addr_t ipaddr, netmask, gateway;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_cmd ) == 0 )
                cmd = opt.value;
            else
                STOP_AT_INVALID_ARGUMENT 
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( cmd==NULL || strcmp(cmd, "info") == 0 )
    {
        shell_write_line( sprintf_mac( buf, mac_address_init, "mac:", 0) );
        shell_printf( "dhcp: %u\n", ip_manual ? 0 : 1 );
        switch( dhcp_state )
        {
        case DHCP_LINK_DOWN:
            shell_write_line("Disconnected");
            break;
        case DHCP_WAIT_ADDRESS:
            shell_write_line("DHCP waiting");
            break;
        case DHCP_ADDRESS_ASSIGNED:
        case DHCP_ADDRESS_MANUAL:
            shell_write_line( sprintf_ip( buf, gnetif.ip_addr.addr, "ip:", 0 ) );
            shell_write_line( sprintf_ip( buf, gnetif.netmask.addr, "netmask:", 0 ) );
            shell_write_line( sprintf_ip( buf, gnetif.gw.addr, "gateway:", 0 ) );
            for( i=0; i<DNS_MAX_SERVERS; i++ )
            {
                sprintf( buf, "dns%u: ", i+1 );
                sprintf_ip( buf+strlen(buf), dns_getserver(i)->addr, 0, 0 );
                shell_write_line( buf );
            }
            break;
        default:
            break;
        } 
    }
    else if( strcmp(cmd, "up") == 0 )
    {
        return send_dhcpc_event( DHCPC_EVENT_NETIF_UP ) ? 0 : 1;
    }
    else if( strcmp(cmd, "down") == 0 )
    {
        return send_dhcpc_event( DHCPC_EVENT_NETIF_DOWN ) ? 0 : 1;
    }
    else if( strcmp(cmd, "dhcp") == 0 )
    {
        reset_address();
        ip_manual = 0;
        dhcp_state = DHCP_WAIT_ADDRESS;
        dhcp_stop(&gnetif);
        dhcp_start(&gnetif);
    }
    else if( strcmp(cmd, "ip") == 0 )
    {
        input = shell_read_multi_lines(0);
        if( input )
        {
            /* parse manual ip/netmask/gateway setting */
            if( parse_ip_netmask_gateway( (const char*)input, (char*)&ipaddr, (char*)&netmask, (char*)&gateway ) )
            {
                dhcp_stop(&gnetif);
                netif_set_addr(&gnetif, &ipaddr, &netmask, &gateway);
                dns_setserver( 0, (const ip_addr_t *)&gateway );
                ip_manual = 1;
                succ = 1;
            }
            vPortFree( (void*)input );
            if( !succ )
                return 1;
        }
    }
    else if( strcmp(cmd, "dns") == 0 )
    {
        input = shell_read_multi_lines(0);
        if( input )
        {
            /* parse manual dns1/dns2 setting */
            if( parse_dns1_dns2( (const char*)input, (char*)&ipaddr, (char*)&netmask ) )
            {
                dns_setserver( 0, (const ip_addr_t *)&ipaddr );
                dns_setserver( 1, (const ip_addr_t *)&netmask );
            }
        }
    }
    else
        return -1;
    return 0;
}



extern int cmd_ping( int argc, char *argv[] );
extern int cmd_wget( int argc, char *argv[] );
extern int cmd_nc( int argc, char *argv[] );

const shell_cmd_t cmd_tab_lwip[] = {
{   0, 0, "lwip",  cmd_lwip, 
    "lwip stastics",
    "lwip"  },
{   0, 'n', "netstat",  cmd_netstat, 
    "show net status",
    "netstat"  },
#if USE_CMD_PING
{   0, 'p', "ping",  cmd_ping, 
    "ping remote host",
    "ping host/ip"  },
#endif
#if USE_CMD_WGET
{   0, 0, "wget",  cmd_wget, 
    "get file by url",
    "wget -u <url> -f <file>"  },
#endif
#if USE_CMD_NC
{   0, 0, "nc",  cmd_nc, 
    "tcp test",
    "nc host/ip port"  },
#endif
{   CMD_END  } };



void task_dhcpc_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_QUEUE_BUFFER( dhcpc, TASK_DHCPC_QUEUE_SIZE, 1 );
    queue_dhcpc = os_queue_create_static( "dhcpcQ", TASK_DHCPC_QUEUE_SIZE, 1, &static_queue_buffer_dhcpc );
#else
    queue_dhcpc = os_queue_create( "dhcpcQ", TASK_DHCPC_QUEUE_SIZE, 1 );
#endif
    if( queue_dhcpc == NULL )
        halt( "create dhdpc queue" );

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TIMER_BUFFER( dhcpc );
    timer_dhcpc = os_timer_create_static( "dhcpc", OS_TICKS_MS(DHCPC_TIMER_PERIOD_MS), 1, timer_dhcpc_callback, &static_timer_buffer_dhcpc );
#else
    timer_dhcpc = os_timer_create( "dhcpc", OS_TICKS_MS(DHCPC_TIMER_PERIOD_MS), 1, timer_dhcpc_callback );
#endif
    if( timer_dhcpc == NULL )
        halt( "create dhdpc timer" );
   
#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( dhcpc, TASK_DHCPC_STACK_SIZE );
    task = os_task_create_static( TASK_DHCPC_NAME, task_dhcpc_entry, NULL, TASK_DHCPC_STACK_SIZE, TASK_DHCPC_PRIORITY, &static_task_buffer_dhcpc );
#else
    task = os_task_create( TASK_DHCPC_NAME, task_dhcpc_entry, NULL, TASK_DHCPC_STACK_SIZE, TASK_DHCPC_PRIORITY );
#endif
    if( task == NULL )
        halt("create dhcpc task");

    shell_add_cmd_table( cmd_tab_lwip );
}

