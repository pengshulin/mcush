/* Modbus/RTU implementation based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_logger.h"
#include "task_modbus_rtu.h"
#if MODBUS_RTU_SWDG_FEED
#include "task_swdg.h"
#endif

//#define DEBUG_MODBUS
LOGGER_MODULE_NAME("modbus");

modbus_rtu_client_t client;

extern int hal_modbus_init(int baudrate);
extern int hal_modbus_putc( char c, os_tick_t block_ticks );
extern int hal_modbus_getc( char *c, os_tick_t block_ticks );

__weak uint8_t hal_modbus_set_address( uint8_t new_address )
{
    static uint8_t address = MODBUS_RTU_DEFAULT_UNIT_ID;
    uint8_t old_address;
    old_address = address;
    address = new_address;
    return old_address;
}


int exec_modbus_rtu_request( uint8_t function_code, uint16_t address, uint16_t size, uint16_t *payload, int *write )
{
    uint16_t *buf = payload + 2;
    uint8_t *buf2 = (uint8_t*)buf;
    uint16_t word;
    uint8_t byte, byte2;
    uint8_t bytes; 
    int i, err;

    *write = 0;
    switch( function_code )
    {
    case MODBUS_CODE_READ_COILS:
        /* recv: UID  CODE       ADDR    SIZE
                (01)  (01)  ---  (00 64) (00 0A)
           send: TID  CODE       BYTES  DAT0   DAT1  ... 
                (01)  (01)  ---  (02)   (00)   (00)   ...  */
    case MODBUS_CODE_READ_DISCRETE_INPUTS:
        /* recv: UID  CODE       ADDR    SIZE
                (01)  (02)  ---  (00 64) (00 0A)
           send: UID  CODE       BYTES  DAT0   DAT1  ... 
                (01)  (02)  ---  (02)   (00)   (00)   ...  */
        bytes = 0;
        buf2 = (uint8_t*)payload + 1;
        byte = i = 0;
        while( size-- )
        { 
            if( function_code == MODBUS_CODE_READ_DISCRETE_INPUTS )
                err = modbus_read_discrete( address, &byte2 );
            else
                err = modbus_read_coil( address, &byte2 );
            if( err )
                return err;
            if( byte2 )
                byte |= 1<<i;
            if( ++i >= 8 )
            {
                bytes++;
                *buf2++ = byte;
                byte = i = 0;
            }
            address++;
        }
        if( i != 0 )
        {
            bytes++;
            *buf2 = byte;
        }
        *((uint8_t*)payload) = bytes;
        *write = bytes + 1;
        break;

    case MODBUS_CODE_WRITE_SINGLE_COIL:
        /* recv: UID  CODE       ADDR    DAT
                (01) (05)  ---  (00 10) (FF 00)
           send: UID  CODE       ADDR    DAT
                (01) (05)  ---  (00 10) (FF 00) */
        if( (size != 0xFF00) && (size != 0) )
            return MODBUS_ERROR_DATA_VALUE;
        err = modbus_write_coil( address, size ? 1 : 0 );
        if( err )
            return err;
        *write = 4;
        break;

    case MODBUS_CODE_WRITE_MULTI_COILS:
        /* recv: UID  CODE       ADDR    SIZE    BYTES DAT0 DAT1 ...
                (01) (0F)  ---  (00 10) (00 02) (04)  (00) (00)  ...
           send: UID  CODE       ADDR    SIZE
                (01) (0F)  ---  (00 10) (00 02) */
        if( *buf2 != (size-1)/8+1 )
            return MODBUS_ERROR_DATA_VALUE;
        buf2++;
        i = 0;
        while( size-- )
        { 
            err = modbus_write_coil( address, (*buf2) & (1<<i) );
            if( err )
                return err;
            if( ++i >= 8 )
            {
                i = 0;
                buf2++;
            }
            address++;
        }
        *write = 4;
        break;

    case MODBUS_CODE_READ_INPUT_REGISTERS:
        /* recv: UID  CODE       ADDR    DAT
                (01) (04)  ---  (27 56) (00 00)
           send: UID  CODE       BYTES  DAT0    DAT1   ... 
                (01) (04)  ---  (14)   (61 4E) (00 BC) ...  */
    case MODBUS_CODE_READ_HOLD_REGISTERS:
        /* recv: UID  CODE       ADDR    SIZE 
                (01) (03)  ---  (27 56) (00 0A)
           send: UID  CODE       BYTES  DAT0    DAT1   ... 
                (01) (03)  ---  (14)   (61 4E) (00 BC) ...  */
        if( size > MODBUS_MULTI_REGISTER_LIMIT )
            return MODBUS_ERROR_DATA_VALUE;
        bytes = 0;
        buf2 = (uint8_t*)payload + 1;
        while( size-- )
        {
            if( function_code == MODBUS_CODE_READ_HOLD_REGISTERS )
                err = modbus_read_hold_register( address, &word );
            else
                err = modbus_read_input_register( address, &word );
            if( err )
                return err; 
            *buf2++ = (word>>8) & 0xFF;  /* big endian */
            *buf2++ = word & 0xFF;
            bytes += 2;
            address++;
        }
        *((uint8_t*)payload) = bytes;
        *write = bytes + 1;
        break;

    case MODBUS_CODE_WRITE_SINGLE_REGISTER:
        /* recv: UID  CODE       ADDR    DAT
                (01) (06)  ---  (27 56) (00 00)
           send: UID  CODE       ADDR    DAT
                (01) (06)  ---  (27 56) (00 00) */
        err = modbus_write_hold_register( address, size );
        if( err )
            return err;
        *write = 4;
        break;

    case MODBUS_CODE_WRITE_MULTI_REGISTERS:
        /* recv: UID  CODE       ADDR    SIZE    BYTES  DAT0  DAT1
                (01) (10)  ---  (27 56) (00 02) (04) (61 4E) (00 BC)
           send: UID  CODE       ADDR    SIZE   
                (01) (10)  ---  (27 56) (00 02) */
        if( *buf2 != 2*size )
            return MODBUS_ERROR_DATA_VALUE;
        buf2++;
        while( size-- )
        {
            word = *(buf2++);
            word = (word << 8) + *(buf2++);
            err = modbus_write_hold_register( address, word );
            if( err )
                return err;
            address++;
        }
        *write = 4;
        break;

    default:
#ifdef DEBUG_MODBUS
        logger_printf_warn( "invalid code %u", function_code );
#endif
        return MODBUS_ERROR_FUNCTION;
        break;
    }
    return 0;
}


int process_modbus_rtu_request( void )
{
    uint16_t address, size, *payload, crc;
    int write;
    int err;

    address = *(uint16_t*)(client.buf+2);
    size = *(uint16_t*)(client.buf+4);
    swap_bytes( (uint8_t*)&address, ((uint8_t*)&address)+1 );
    swap_bytes( (uint8_t*)&size, ((uint8_t*)&size)+1 );
    payload = (uint16_t*)(client.buf+2);
    err = exec_modbus_rtu_request( client.buf[1], address, size, payload, &write );
    if( err )
    {
        /* send error response */
        client.buf[1] |= 0x80;
        *((uint8_t*)payload) = err;
        write = 1;
#ifdef DEBUG_MODBUS
        logger_printf_warn( "packet process err=%d, code=%d", err, client.buf[1]&0x7F );
#endif
    }
    else
    {
        /* normal packet, update tick */
        client.tick_last = os_tick();
    }
    /* add crc */ 
    crc = crc16_modbus( (uint8_t*)client.buf, 2+write );
    client.buf[2+write] = (crc)&0xFF;
    client.buf[3+write] = (crc>>8)&0xFF;
    client.reply_len = 2+write+2;
    /* send reply */
    for( write=0; write<client.reply_len; write++ )
    {
        if( hal_modbus_putc( client.buf[write], OS_TICKS_MS(50) ) == 0 )
        {
            err = 1;
            break;
        }
    }
    return err ? 0 : 1;
}


void task_modbus_rtu_entry(void *arg)
{
    uint16_t crc;
    char c;

    (void)arg;
    hal_modbus_init(0);
    memset( (void*)&client, 0, sizeof(client) );
    hal_modbus_set_address( 0x01 );
    client.unit_id = 0x01;
    
    while( 1 )
    {
        client.request_len = 0;
        while( client.request_len < MODBUS_RTU_BUF_LEN_MAX )
        {
            if( hal_modbus_getc( &c, OS_TICKS_MS(4) ) == 0 )
                break;
            client.buf[client.request_len++] = c;
        }

        /* for empty frame, delay longer time to save CPU resource */
        if( client.request_len == 0 )
        {
            os_task_delay_ms( OS_TICKS_MS(50) );
            continue;
        }

        /* ignore invalid frames */
        if( client.request_len < 4 )
            continue;
        if( client.buf[0] != client.unit_id  )
            continue;
        /* crc match */
        crc = client.buf[client.request_len-1];
        crc = (crc<<8) + client.buf[client.request_len-2];
        if( crc != crc16_modbus( (uint8_t*)client.buf, client.request_len-2 ) )
            continue;

        if( process_modbus_rtu_request() )
            client.succ_cnt++;
        else
            client.err_cnt++;
    }
}


void task_modbus_rtu_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( modbus_rtu, TASK_MODBUS_RTU_STACK_SIZE );
    task = os_task_create_static( "mbrtuT", task_modbus_rtu_entry, NULL,
                TASK_MODBUS_RTU_STACK_SIZE, TASK_MODBUS_RTU_PRIORITY,
                &static_task_buffer_modbus_rtu );
#else
    task = os_task_create( "mbrtuT", task_modbus_rtu_entry, NULL,
                TASK_MODBUS_RTU_STACK_SIZE, TASK_MODBUS_RTU_PRIORITY );
#endif
    if( task == NULL )
        halt("create modbus rtu task");
}

