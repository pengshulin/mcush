/* Modbus/TCP implementation based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_MODBUS_TCP_H_
#define _TASK_MODBUS_TCP_H_

#define TASK_MODBUS_TCP_STACK_SIZE  (4*1024)
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

#ifndef MODBUS_TCP_CHECK_TRANSACTION_ID_SEQ
    #define MODBUS_TCP_CHECK_TRANSACTION_ID_SEQ  0
#endif

#ifndef MODBUS_TCP_ERR_COUNT
    #define MODBUS_TCP_ERR_COUNT   10
#endif


//#define CLIENT_MEM_MALLOC   os_malloc
//#define CLIENT_MEM_FREE     os_free
#define CLIENT_MEM_MALLOC   mem_malloc
#define CLIENT_MEM_FREE     mem_free


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
    uint16_t reply_len;
    uint16_t buf_len;
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
#define MODBUS_TCP_EVENT_REQUEST         20
#define MODBUS_TCP_EVENT_REPLY           21

#define MODBUS_LOG_FILE  "/s/modbus.log"

#define MODBUS_TCP_RECONNECT_TIMEOUT_S   60


#define MODBUS_CODE_READ_COILS                      1
#define MODBUS_CODE_READ_DISCRETE_INPUTS            2
#define MODBUS_CODE_READ_HOLD_REGISTERS             3
#define MODBUS_CODE_READ_INPUT_REGISTERS            4
#define MODBUS_CODE_WRITE_SINGLE_COIL               5
#define MODBUS_CODE_WRITE_SINGLE_REGISTER           6
#define MODBUS_CODE_WRITE_MULTI_COILS               15
#define MODBUS_CODE_WRITE_MULTI_REGISTERS           16
#define MODBUS_CODE_READ_WRITE_MULTI_REGISTER       23
#define MODBUS_CODE_DISABLE_WRITE_REGISTER          22
#define MODBUS_CODE_READ_FILE_RECORD                20
#define MODBUS_CODE_WRITE_FILE_RECORD               21
#define MODBUS_CODE_READ_DEVICE_CODE                43

#define MODBUS_ERROR_FUNCTION                       1
#define MODBUS_ERROR_DATA_ADDRESS                   2
#define MODBUS_ERROR_DATA_VALUE                     3
#define MODBUS_ERROR_DEVICE_FAULT                   4
#define MODBUS_ERROR_CONFIRM                        5
#define MODBUS_ERROR_DEVICE_BUSY                    6
#define MODBUS_ERROR_CHECK                          8



int send_modbus_tcp_event( uint8_t type, int8_t tag, uint32_t data );

void task_modbus_tcp_init(void);

/* application hook function */
int modbus_read_discrete( uint16_t address, uint8_t *value );
int modbus_read_coil( uint16_t address, uint8_t *value );
int modbus_write_coil( uint16_t address, uint8_t value );
int modbus_read_input_register( uint16_t address, uint16_t *value );
int modbus_read_hold_register( uint16_t address, uint16_t *value );
int modbus_write_hold_register( uint16_t address, uint16_t value );


#endif

