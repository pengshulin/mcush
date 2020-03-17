/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_CAN_H__
#define __HAL_CAN_H__


#ifndef HAL_CAN_QUEUE_RX_LEN
    #define HAL_CAN_QUEUE_RX_LEN           64
#endif
#ifndef HAL_CAN_QUEUE_TX_LEN
    #define HAL_CAN_QUEUE_TX_LEN           32
#endif

typedef struct {
    uint32_t id;  /* can-id in 11/29bits */
    uint8_t ext;  /* extend mode */
    uint8_t remote;  /* remote request */
    uint8_t len;  /* data length */
    char data[8];  /* raw data */
} can_message_t;

typedef struct {
    uint16_t std_id, std_id_mask;
    uint32_t ext_id, ext_id_mask;
    uint8_t ext;  /* extend mode */
    uint8_t remote;  /* remote request */
} can_filter_t;


int hal_can_init( void );
void hal_can_deinit( void );
int hal_can_set_baudrate( int baudrate );
int hal_can_get_baudrate( void );
int hal_can_get_last_error( void );
int hal_can_get_error_count( void );
int hal_can_send( can_message_t *msg );
void hal_can_cancel( int id );
int hal_can_receive( can_message_t *msg );
int hal_can_read( can_message_t *msg, int block_time );
int hal_can_write( can_message_t *msg );
int hal_can_filter_set( int index, can_filter_t *filter, int enabled );
int hal_can_filter_get( int index, can_filter_t *filter, int *enabled );



#endif
