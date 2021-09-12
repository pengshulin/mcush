/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __TASK_SWDG_H__
#define __TASK_SWDG_H__

#define TASK_SWDG_STACK_SIZE  (256)
#define TASK_SWDG_PRIORITY    (OS_PRIORITY_HIGHEST)


#ifndef SWDG_REBOOT_TIMEOUT_S
    #define SWDG_REBOOT_TIMEOUT_S  5*60  /* in seconds */
#endif

 
void swdg_feed(void);
void swdg_reboot_hook(void);

void task_swdg_init(void);

#endif

