/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_BLINK_H_
#define _TASK_BLINK_H_

#define TASK_BLINK_STACK_SIZE  (200)
#define TASK_BLINK_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_BLINK_QUEUE_SIZE  (4)

extern void task_blink_init(void);
#if !defined(MCUSH_NON_OS)
extern void task_blink_entry(void *p);
#else
extern void task_blink_entry(void);
#endif

int get_errno(void);
int set_errno(int num);

#endif

