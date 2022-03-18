/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_BLINK_H__
#define __TASK_BLINK_H__

#ifdef __cplusplus
extern "C" {
#endif


#define TASK_BLINK_NAME         "blinkT"
#define TASK_BLINK_STACK_SIZE   250
#define TASK_BLINK_PRIORITY     OS_PRIORITY_NORMAL

#ifndef ERRNO_LED_NORMAL
    #define ERRNO_LED_NORMAL    0
#endif

#ifndef ERRNO_LED_ERROR
    #define ERRNO_LED_ERROR     ERRNO_LED_NORMAL
#endif

void task_blink_init(void);
void task_blink_entry(void *arg);

int cmd_error( int argc, char *argv[] );

/***************************************************************/
/* lower IDs 1 ~ 99 are all freely used by user application    */
/* reserved IDs range from 100 ~ 199                           */
/* higher IDs 200 ~ 100000000 are also available               */
/***************************************************************/

#define ERRNO_FILE_SYSTEM_INIT_CHECKING     100
#define ERRNO_FILE_SYSTEM_INIT_ERROR        101
#define ERRNO_FILE_READ_WRITE_ERROR         102



#ifdef __cplusplus
}
#endif

#endif
