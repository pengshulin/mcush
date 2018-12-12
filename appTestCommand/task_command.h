/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_COMMAND_H_
#define _TASK_COMMAND_H_

#define TASK_COMMAND_STACK_SIZE  (MCUSH_STACK_SIZE)
#define TASK_COMMAND_PRIORITY    (MCUSH_PRIORITY)
#define TASK_COMMAND_QUEUE_SIZE  (4)

extern void task_command_init(void);
extern void task_command_entry(void *p);



#endif
