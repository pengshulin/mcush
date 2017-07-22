#ifndef _TASK_OW_H_
#define _TASK_OW_H_

#define TASK_OW_STACK_SIZE  (512 / sizeof(portSTACK_TYPE))
#define TASK_OW_PRIORITY    (MCUSH_PRIORITY+1)
#define TASK_OW_QUEUE_SIZE  (4)

extern void task_ow_init(void);

#endif

