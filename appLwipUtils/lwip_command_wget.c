/* MCUSH designed by Peng Shulin, all rights reserved. */
#if USE_CMD_WGET
#include "mcush.h"
#include "task_logger.h"
#include "task_netmon.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/dns.h"
#include "lwip/icmp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"
#include "lwip_lib.h"

LOGGER_MODULE_NAME("wget");

#ifndef WGET_DEBUG
#define WGET_DEBUG     LWIP_DBG_ON
#endif

#define WGET_DNS_RESOLVE_TIMEOUT_S  5
 
#ifndef WGET_TEMP_FILE 
#define WGET_TEMP_FILE  "/s/wget.tmp"
#endif

const char http_get_headers_fmt[] = "GET /%s HTTP/1.0\r\n\r\n";

typedef struct _wget_cb_t
{
    ip_addr_t server_ip;
    const char *server_hostname;
    const char *server_pathfile;
    const char *local_file;
    int server_port;
    uint8_t dns_resolved;
    uint8_t file_saved;
    uint8_t done;
    int fd;
    struct tcp_pcb *pcb;
    uint8_t timeout;
    uint8_t head_check_state;
    uint8_t head_skipped;
    int len;
} wget_cb_t;
wget_cb_t *wcb;

#define VALID_FILE_SIZE_MIN  16



void wget_hostname_dns_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    (void)name;
    (void)arg;
    if( ipaddr )
        wcb->server_ip = *ipaddr;
    else
        wcb->server_ip.addr = 0;
    wcb->dns_resolved = 1;
}


err_t wget_http_recv_cb(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *p2;

    (void)arg;
    if( err == ERR_OK )
    {
        if( p != NULL )  /* normal receive */
        {
            p2 = p;
            while(p2 != NULL)
            {
                mcush_write( wcb->fd, p2->payload, p2->len );  
                wcb->file_saved = 1;
                wcb->len += p2->len;
                p2 = p2->next;
            }
            tcp_recved(pcb, p->tot_len);
            pbuf_free(p);
        }
        else  /* connection closed */
        {
            tcp_close(pcb);
            wcb->done = 1; 
        }
    }
    return ERR_OK;
}

static void wget_http_error_cb(void *arg, err_t err)
{
    (void)arg;
    (void)err;
    shell_write_err( "tcp" ); 
}


static err_t wget_http_poll_cb(void *arg, struct tcp_pcb *pcb)
{
    (void)arg;
    wcb->timeout++;
    if( wcb->timeout > 20 )
    {
        tcp_abort(pcb);
        shell_write_err( "tcp poll timeout, abort" ); 
    }
    return ERR_OK;
}


static err_t wget_http_sent_cb(void *arg, struct tcp_pcb *pcb, uint16_t len)
{
    (void)arg;
    (void)pcb;
    (void)len;
    wcb->timeout = 0;
    //shell_printf("%d bytes sent\n", len ); 
    return ERR_OK;
}


static err_t wget_http_connected_cb(void *arg, struct tcp_pcb *pcb, err_t err)
{
    char buf[256];
    (void)arg;
    if( err != ERR_OK )
    {
        tcp_close(pcb);
        wcb->done = 1;
    }
    else
    {
        sprintf(buf, http_get_headers_fmt, wcb->server_pathfile);
        tcp_recv(pcb, wget_http_recv_cb);
        tcp_err(pcb, wget_http_error_cb);
        tcp_poll(pcb, wget_http_poll_cb, 10);
        tcp_sent(pcb, wget_http_sent_cb);
        tcp_write(pcb, buf, strlen(buf), TCP_WRITE_FLAG_COPY);
        tcp_output(pcb);
    }
    return ERR_OK;
}

int wget_http_get_file(void)
{
    if( ! tcp_bind_random_port(wcb->pcb) )
        return 0; 
    tcp_connect( wcb->pcb, &wcb->server_ip, wcb->server_port, wget_http_connected_cb );
    return 1;
}

 
#define HEAD_CHECK_STATE_0  0  // init
#define HEAD_CHECK_STATE_A  1  // \r
#define HEAD_CHECK_STATE_B  2  // \r\n
#define HEAD_CHECK_STATE_C  3  // \r\n\r
#define HEAD_CHECK_STATE_D  4  // \r\n\r\n
#define CACHE_SIZE  128
int post_process_temp_file( const char *tmpfile, const char *dstfile )
{
    int fd1, fd2=0, success=0, r;
    char c, state=0, sync=0;
    char buf[CACHE_SIZE];

    fd1 = mcush_open( tmpfile, "r" );
    if( fd1 )
    {
        /* sync for \r\n\r\n */
        while( mcush_read( fd1, &c, 1 ) == 1 )
        {
            switch( state )
            {
            default:
            case HEAD_CHECK_STATE_0:
                if( c == '\r' )
                    state = HEAD_CHECK_STATE_A;
                break;
            case HEAD_CHECK_STATE_A:
                if( c == '\n' )
                    state = HEAD_CHECK_STATE_B;
                else if( c != '\r' )
                    state = HEAD_CHECK_STATE_0;
                break;
            case HEAD_CHECK_STATE_B:
                if( c == '\r' )
                    state = HEAD_CHECK_STATE_C;
                else
                    state = HEAD_CHECK_STATE_0;
                break;
            case HEAD_CHECK_STATE_C:
                if( c == '\n' )
                    state = HEAD_CHECK_STATE_D;
                else
                    state = HEAD_CHECK_STATE_0;
                break;
            }
            if( state == HEAD_CHECK_STATE_D )
            {
                sync = 1;
                break;
            }
        }
    }
    /* copy remaining */
    if( sync )
    {
        fd2 = mcush_open( dstfile, "w+" );
        if( fd2 )
        {
            while( 1 )
            { 
                r = mcush_read( fd1, buf, CACHE_SIZE );
                if( r > 0 )
                {
                    if( mcush_write( fd2, buf, r ) <= 0 )
                        break;
                }
                else
                {
                    success = 1;
                    break;
                }
            }
        }
    }    
        
    if( fd1 )
        mcush_close(fd1);
    if( fd2 )
        mcush_close(fd2);
    return success;
}
    

int cmd_wget( int argc, char *argv[] )
{
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'u', "url", "url", "http://..." },
        { MCUSH_OPT_VALUE, MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED,
          'f', "file", "output file", "output file name" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint32_t i;
    uint8_t ip_set=0, hostname_set=0;
    wget_cb_t lwcb;
    char chr;
    ip_addr_t ipaddr;
    char *protocol = 0;
    char *server = 0;
    char *pathfile = 0;
    char *file = 0;
    int size;
    char buf[64];

    (void)ip_set;
    memset( &lwcb, 0, sizeof(wget_cb_t) );
    wcb = &lwcb;
    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "url" ) == 0 )
            {
                /* split url into protocol/server/port/pathfile */
                if( split_url( opt.value, &protocol, &server, &wcb->server_port, &pathfile) )
                {
                    shell_write_err( "url parse" );
                    return 1;
                }
                if( wcb->server_port < 0 || wcb->server_port >= 65535 )
                {
                    shell_write_err( "port" );
                    return 1;
                }
                /* parse ip addr, check if dns resolve is need */
                if( ipaddr_aton( server, &ipaddr ) )
                {
                    wcb->server_ip.addr = ipaddr.addr;
                    ip_set = 1;
                }
                else
                {
                    wcb->server_hostname = server;
                    hostname_set = 1;
                }
                wcb->server_pathfile = pathfile;
                if( wcb->server_port == 0 )
                    wcb->server_port = 80;  /* default HTTP port */ 
            }
            else if( strcmp( opt.spec->name, "file" ) == 0 )
            {
                if( opt.value )
                {
                    wcb->local_file = file = (char*)opt.value;
                }
                else
                {
                    shell_write_err( "file" );
                    return 1;
                }
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !protocol || !server || !pathfile || !file )
        return 1;

    /* check protocol, only HTTP is supported now */
    if( strcmp( protocol, "http" ) != 0 )
    {
        shell_write_line( "only support http" ); 
        return 1;
    }
 
    /* dns resolve first */
    if( hostname_set )
    {
        shell_printf("dns resolve: %s\n", server);
        if( ERR_OK == dns_gethostbyname( server, &ipaddr, (dns_found_callback)wget_hostname_dns_cb, NULL ) )
        {
            wcb->server_ip.addr = ipaddr.addr;
            wcb->dns_resolved = 1;
        }
        else
        {
            /* wait for Ctrl-C */
            for( i=WGET_DNS_RESOLVE_TIMEOUT_S*10; i; i-- )
            {
                if( wcb->dns_resolved )
                    break;
                if( shell_driver_read_char_blocked(&chr, OS_TICKS_MS(100)) != -1 )
                {
                    if( chr == 0x03 ) /* Ctrl-C for stop */
                    {
                        wcb = 0;
                        return 0;
                    }
                }
            }
            if( i == 0 )
            {
                shell_printf("dns timeout\n");
                wcb = 0;
                return 1;
            }
        }
        if( wcb->server_ip.addr )
        {
            shell_write_line( sprintf_ip(buf, wcb->server_ip.addr, "dns resolved:", 0) ); 
        }
        else
        {
            shell_write_line("dns resolve failed");
            wcb = 0;
            return 1;
        }
    } 

    /* prepare local record file */
    wcb->fd = mcush_open( WGET_TEMP_FILE, "w+" );
    if( wcb->fd != 0 )
    {
        wcb->pcb = tcp_new();
        if( wcb->pcb == NULL )
        {
            shell_write_err( shell_str_memory );
        }
        else
        {
            /* connect and get file */
            wget_http_get_file();

            /* wait for done or Ctrl-C */
            while( 1 )
            {
                if( wcb->done )
                    break;
                if( shell_driver_read_char_blocked(&chr, OS_TICKS_MS(100)) != -1 )
                {
                    if( chr == 0x03 ) /* Ctrl-C for stop */
                    {
                        break;
                    }
                }
            }
        }
        /* all done, clean */ 
        mcush_close(wcb->fd);
        
        if( wcb->len > VALID_FILE_SIZE_MIN )
        {
            /* analysis temp file and cut out the head */
            if( post_process_temp_file(WGET_TEMP_FILE, wcb->local_file) )
            {
                if( mcush_size( wcb->local_file, &size ) ) 
                {
                    shell_printf( "%u bytes saved\n", size );
                }
                mcush_remove( WGET_TEMP_FILE );
            }
            else
                shell_write_err( "analysis file" );
        }
        else
            shell_write_line( "received data invalid" );
        //raw_remove(pcb.ping_pcb);
    }
    else
        shell_write_err( "file" );
    
    wcb = 0;
    return lwcb.file_saved ? 0 : 1;
}

#endif
