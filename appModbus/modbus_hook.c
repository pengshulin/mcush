/* modbus hook functions template */
#include "mcush.h"
#include "task_logger.h"
#include "task_modbus_tcp.h"


#define HOLD_REG_NUM  32

uint16_t hold_reg[HOLD_REG_NUM];


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
    default:
        return 0;
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

