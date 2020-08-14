/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_BLINK_H_
#define _TASK_BLINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_BLINK_STACK_SIZE  (200)
#define TASK_BLINK_PRIORITY    (OS_PRIORITY_NORMAL)

void task_blink_init(void);
void task_blink_entry(void *p);

int get_errno(void);
int set_errno(int num);

int cmd_error( int argc, char *argv[] );

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
