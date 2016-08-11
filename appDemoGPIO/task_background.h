#ifndef _TASK_BACKGROUND_H_
#define _TASK_BACKGROUND_H_

#define TASK_BACKGROUND_STACK_SIZE  (256 / sizeof(portSTACK_TYPE))
#define TASK_BACKGROUND_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_BACKGROUND_QUEUE_SIZE  (4)

extern void task_background_init(void);

#endif

