/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_BLINK_H_
#define _TASK_BLINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_BLINK_STACK_SIZE  (200)
#define TASK_BLINK_PRIORITY    (MCUSH_PRIORITY)
#define TASK_BLINK_QUEUE_SIZE  (4)

extern void task_blink_init(void);
extern void task_blink_entry(void *p);

int get_errno(void);
int set_errno(int num);

/****************************************************************************/
/* reserved error code id (range from 1 ~ 9) */
/****************************************************************************/

#define ERRNO_RTC_ERROR                     1
#define ERRNO_FILE_SYSTEM_INIT_ERROR        2
#define ERRNO_FILE_READ_WRITE_ERROR         3


#ifdef __cplusplus
}
#endif

#endif
