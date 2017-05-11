#ifndef _TASK_TRIG_H_
#define _TASK_TRIG_H_

#define TASK_TRIG_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_TRIG_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_TRIG_QUEUE_SIZE  (4)

extern void task_trig_init(void);

#endif

