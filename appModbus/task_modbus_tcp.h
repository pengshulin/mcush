/* Modbus/TCP implementation based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_MODBUS_TCP_H__
#define __TASK_MODBUS_TCP_H__

#define TASK_MODBUS_TCP_STACK_SIZE  (2*1024)
#define TASK_MODBUS_TCP_PRIORITY    (OS_PRIORITY_LOW)
#define TASK_MODBUS_TCP_QUEUE_SIZE  (32)

#ifndef MODBUS_TCP_NUM
    #define MODBUS_TCP_NUM   3
#endif

#ifndef MODBUS_TCP_PORT 
    #define MODBUS_TCP_PORT  502
#endif

#ifndef MODBUS_TCP_TIMEOUT 
    #define MODBUS_TCP_TIMEOUT  30
#endif

#ifndef MODBUS_TCP_RECONNECT_TIMEOUT_S
    #define MODBUS_TCP_RECONNECT_TIMEOUT_S   60
#endif

#ifndef MODBUS_TCP_CHECK_TRANSACTION_ID_SEQ
    #define MODBUS_TCP_CHECK_TRANSACTION_ID_SEQ  0
#endif

#ifndef MODBUS_TCP_REPLY_RETRY
    #define MODBUS_TCP_REPLY_RETRY  10  /* retry * 500ms */
#endif

#ifndef MODBUS_TCP_ERR_COUNT
    #define MODBUS_TCP_ERR_COUNT   10
#endif

#ifndef MODBUS_TCP_SWDG_FEED
    #define MODBUS_TCP_SWDG_FEED   0
#endif


#define MODBUS_MULTI_REGISTER_LIMIT  (125)
#define MODBUS_TCP_BUF_LEN_MAX   (12+2*MODBUS_MULTI_REGISTER_LIMIT)


typedef struct {
    uint8_t type;
    int8_t tag;
    uint16_t seq;
    uint32_t data;
} modbus_tcp_event_t;


typedef struct {
    uint16_t transaction_id;
    uint16_t protocol_id;
    uint16_t bytes;
    uint8_t unit_id;
    uint8_t function_code;
} modbus_tcp_head_t;


typedef struct {
    struct tcp_pcb *tpcb;
    uint8_t retries;
    uint8_t pending_request;
    uint8_t pending_reply;
    uint8_t buf_locked;
    uint16_t client_id;
    uint16_t transaction_id;
    uint16_t request_len;
    uint16_t reply_len;
    uint16_t err_cnt;
    char buf[MODBUS_TCP_BUF_LEN_MAX];
    uint32_t tick_connect;
    uint32_t tick_last;
} modbus_tcp_client_t;

#define MODBUS_TCP_EVENT_NET_UP          1
#define MODBUS_TCP_EVENT_NET_DOWN        2
#define MODBUS_TCP_EVENT_CONNECT         10 
#define MODBUS_TCP_EVENT_CONNECTED       11
#define MODBUS_TCP_EVENT_CONNECT_ERROR   12
#define MODBUS_TCP_EVENT_RESET           13
#define MODBUS_TCP_EVENT_START           14
#define MODBUS_TCP_EVENT_CLOSE           15



int send_modbus_tcp_event( uint8_t type, int8_t tag, uint32_t data );
void task_modbus_tcp_init(void);

#include "modbus_def.h"

#endif

