/* Modbus RTU/TCP code defination and api hooks */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MODBUS_DEF_H__
#define __MODBUS_DEF_H__


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


/* application hook function */
int modbus_read_discrete( uint16_t address, uint8_t *value );
int modbus_read_coil( uint16_t address, uint8_t *value );
int modbus_write_coil( uint16_t address, uint8_t value );
int modbus_read_input_register( uint16_t address, uint16_t *value );
int modbus_read_hold_register( uint16_t address, uint16_t *value );
int modbus_write_hold_register( uint16_t address, uint16_t value );


#endif

