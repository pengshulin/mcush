#ifndef _TASK_1WIRE_H_
#define _TASK_1WIRE_H_

#define TASK_1WIRE_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_1WIRE_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_1WIRE_QUEUE_SIZE  (4)

extern void task_1wire_init(void);

#endif

