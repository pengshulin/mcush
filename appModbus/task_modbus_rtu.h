/* Modbus/RTU implementation based on MCUSH */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_MODBUS_RTU_H__
#define __TASK_MODBUS_RTU_H__

/* minimum stack size is less than 400, the actual size depends on
   hook functions in your application, increase if necessary
   expecially when you want to call logger api */
#ifndef TASK_MODBUS_RTU_STACK_SIZE
    #define TASK_MODBUS_RTU_STACK_SIZE  (512)
#endif

#define TASK_MODBUS_RTU_PRIORITY    (OS_PRIORITY_LOW)


#ifndef MODBUS_RTU_SWDG_FEED
    #define MODBUS_RTU_SWDG_FEED   0
#endif


#define MODBUS_MULTI_REGISTER_LIMIT  (125)
#define MODBUS_RTU_BUF_LEN_MAX   (6+2*MODBUS_MULTI_REGISTER_LIMIT)


typedef struct {
    uint8_t unit_id;
    uint16_t request_len;
    uint16_t reply_len;
    char buf[MODBUS_RTU_BUF_LEN_MAX];
    uint32_t tick_last;
    uint32_t succ_cnt;
    uint32_t err_cnt;
} modbus_rtu_client_t;


void task_modbus_rtu_init(void);

#include "modbus_def.h"

#endif

