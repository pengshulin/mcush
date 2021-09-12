/* swdg service task, simulate sotware watchdog mechanism
 * 
 * The task runs as an auxiliary service in MCUSH micro-kerenl RTOS environment,
 * it aims to help you debug your applications.
 * To use this service task, include task_swdg.h file in your application
 * and call task_swdg_init() before you use it.
 * 
 * timeout is defined in SWDG_REBOOT_TIMEOUT_S in seconds
 * call swdg_feed() in your application loop 
 * override swdg_reboot_hook() that will be called before actually reboot
 *  
 * MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_blink.h"
#include "task_logger.h"
#include "task_swdg.h"

LOGGER_MODULE_NAME("swdg");


//#define NO_REBOOT  1  /* option for some conditions (eg. EMC test) that reboot is not allow */
static os_tick_t swdg_tick;


__weak void swdg_reboot_hook(void)
{
    /* TODO: override this hook and do additional work before reboot (eg. save settings ) */
}


void swdg_feed(void)
{
    swdg_tick = os_tick();
}


/* NOTE: log to file (if enabled) before reboot (so short wait is necessary) */
void do_swdg_reboot(void)
{
    logger_const_info( "ready to reboot..." );
    swdg_reboot_hook();
    os_task_delay( 1000 );
    os_enter_critical();
    hal_reboot();
    halt("rebooted");
}


void task_swdg_entry(void *arg)
{
    os_tick_t dt;

    (void)arg;
    swdg_feed();
    logger_const_info( "swdg start" );
    while( 1 )
    {
        /* calculate the maximum ticks that task can sleep */
        dt = os_tick() - swdg_tick;
        os_task_delay_ms( OS_TICKS_S(SWDG_REBOOT_TIMEOUT_S) - dt ); 
        /* after wake up, check if it's timeout */
        dt = os_tick() - swdg_tick;
        if( dt >= OS_TICKS_S(SWDG_REBOOT_TIMEOUT_S) )
        {
            do_swdg_reboot(); /* timeout, reboot */ 
        }
    }
}


void task_swdg_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( swdg, TASK_SWDG_STACK_SIZE );
    task = os_task_create_static( "swdgT", task_swdg_entry, 0, TASK_SWDG_STACK_SIZE, TASK_SWDG_PRIORITY, &static_task_buffer_swdg );
#else
    task = os_task_create( "swdgT", task_swdg_entry, 0, TASK_SWDG_STACK_SIZE, TASK_SWDG_PRIORITY );
#endif
    if( task == NULL )
        halt("create swdg task");
}

