#ifndef _TASK_DISP_LED595_H_
#define _TASK_DISP_LED595_H_

#define TASK_DISP_LED595_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_DISP_LED595_PRIORITY    (tskIDLE_PRIORITY)
#define TASK_DISP_LED595_QUEUE_SIZE  (4)

extern void task_disp_led595_init(void);

#endif

