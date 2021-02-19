/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_BLINK_H__
#define __TASK_BLINK_H__

#ifdef __cplusplus
extern "C" {
#endif


#define TASK_BLINK_STACK_SIZE  (224)
#define TASK_BLINK_PRIORITY    (OS_PRIORITY_NORMAL)

#ifndef ERRNO_LED_NORMAL
    #define ERRNO_LED_NORMAL  0
#endif

#ifndef ERRNO_LED_ERROR
    #define ERRNO_LED_ERROR   ERRNO_LED_NORMAL
#endif

void task_blink_init(void);
void task_blink_entry(void *arg);

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
