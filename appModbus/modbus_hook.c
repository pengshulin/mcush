/* modbus hook functions template */
#include "mcush.h"
#include "task_logger.h"
#include "task_modbus_tcp.h"


#define HOLD_REG_NUM  32

uint16_t hold_reg[HOLD_REG_NUM];


void net_state_change_hook(int connected)
{
    send_modbus_tcp_event( connected ? MODBUS_TCP_EVENT_NET_UP : MODBUS_TCP_EVENT_NET_DOWN, 0 );
}


int modbus_read_coil( uint16_t address, uint8_t *value )
{
    *value = 0;
    return 1;
}


int modbus_write_coil( uint16_t address, uint8_t value )
{
    return 1;
}


int modbus_read_input_register( uint16_t address, uint16_t *value )
{
    const float pi=3.141592654;

    switch( address )
    {
    case 0:
        *value = 0x55AA;
        break;
    case 1:
        *(int16_t*)value = -1;
        break;
    case 2:
        *(int16_t*)value = 1;
        break;
    case 3:
        *value = 0xFFFF;
        break;
    case 4:
        *value = 0;
        break;
    case 5:
        *value = *((uint16_t*)&pi);
        break;
    case 6:
        *value = *(((uint16_t*)&pi)+1);
        break;

    default:
        *value = 0;  /* WARNING: fill zero for undefined registers */
        //return 0;
        break;
    }
    return 1;
}


int modbus_read_hold_register( uint16_t address, uint16_t *value )
{
    if( address < HOLD_REG_NUM )
    {
        *value = hold_reg[address];
        return 1;
    }
    return 0;
}


int modbus_write_hold_register( uint16_t address, uint16_t value )
{
    if( address < HOLD_REG_NUM )
    {
        hold_reg[address] = value;
        return 1;
    }
    else
        return 0;
}

