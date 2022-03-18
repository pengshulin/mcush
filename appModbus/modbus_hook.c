/* modbus hook functions template */
#include "mcush.h"
#include "task_logger.h"
#include "task_modbus_tcp.h"


#define HOLD_REG_NUM  32
#define COIL_NUM      32

uint16_t hold_reg[HOLD_REG_NUM];
uint8_t coil[COIL_NUM/8];


void net_state_change_hook(int connected)
{
    send_modbus_tcp_event( connected ? MODBUS_TCP_EVENT_NET_UP : MODBUS_TCP_EVENT_NET_DOWN, 0, 0 );
}


int modbus_read_discrete( uint16_t address, uint8_t *value )
{
    (void)address;
    *value = 0;
    return 0;
}


int modbus_read_coil( uint16_t address, uint8_t *value )
{
    if( address >= COIL_NUM )
        return MODBUS_ERROR_DATA_ADDRESS;
    *value = coil[address/8] & (1<<(address%8)) ? 1 : 0;
    return 0;
}


int modbus_write_coil( uint16_t address, uint8_t value )
{
    if( address >= COIL_NUM )
        return MODBUS_ERROR_DATA_ADDRESS;
    if( value )
        coil[address/8] |= (1<<(address%8));
    else
        coil[address/8] &= ~(1<<(address%8));
    return 0;
}


int modbus_read_input_register( uint16_t address, uint16_t *value )
{
    const float pi=3.141592654f;

    switch( address )
    {
    case 0:
        *(int16_t*)value = -1;
        break;
    case 1:
        *(int16_t*)value = 1;
        break;
    case 2:
        *value = 0xFFFF;
        break;
    case 3:
        *value = 0;
        break;
    case 4:
        *value = *((uint16_t*)&pi);
        break;
    case 5:
        *value = *(((uint16_t*)&pi)+1);
        break;

    default:
        *value = 0x55AA;
        //*value = 0;  /* WARNING: fill zero for undefined registers */
        //return MODBUS_ERROR_DATA_ADDRESS;
        break;
    }
    return 0;
}


int modbus_read_hold_register( uint16_t address, uint16_t *value )
{
    if( address < HOLD_REG_NUM )
    {
        *value = hold_reg[address];
    }
    else
        return MODBUS_ERROR_DATA_ADDRESS;
    return 0;
}


int modbus_write_hold_register( uint16_t address, uint16_t value )
{
    if( address < HOLD_REG_NUM )
    {
        hold_reg[address] = value;
    }
    else
        return MODBUS_ERROR_DATA_ADDRESS;
    return 0;
}

