#ifndef _TASK_BLINK_H_
#define _TASK_BLINK_H_

#define TASK_BLINK_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_BLINK_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_BLINK_QUEUE_SIZE  (4)

extern void task_blink_init(void);

#endif
