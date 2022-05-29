/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_LVGL_H__
#define __TASK_LVGL_H__

#ifndef TASK_LVGL_STACK_SIZE
    #define TASK_LVGL_STACK_SIZE  (4*1024)
#endif

#define TASK_LVGL_PRIORITY    (OS_PRIORITY_LOW)



void task_lvgl_init(void);


#endif

