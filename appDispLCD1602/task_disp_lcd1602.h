#ifndef _TASK_DISP_LCD1602_H_
#define _TASK_DISP_LCD1602_H_

#define TASK_DISP_LCD1602_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_DISP_LCD1602_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_DISP_LCD1602_QUEUE_SIZE  (4)

extern void task_disp_lcd1602_init(void);

#endif

