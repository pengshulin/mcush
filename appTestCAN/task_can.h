#ifndef _TASK_CAN_H_
#define _TASK_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_CAN_STACK_SIZE  (1*1024)
#define TASK_CAN_PRIORITY    (MCUSH_PRIORITY-1)

extern void task_can_init(void);
extern void task_can_entry(void *p);


#ifdef __cplusplus
}
#endif

#endif
