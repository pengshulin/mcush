/* RTOS Abstract Layer for None
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_NONE



void os_init(void)
{
}


void os_start(void)
{
}


int os_is_running(void)
{
    return 0;
}


os_tick_t os_tick( void )
{
    return 0;
}


void os_enter_critical(void)
{
}


void os_exit_critical(void)
{
}


void os_disable_interrupts(void)
{
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    (void)ticks;
}


void os_task_delay_ms( unsigned int ms )
{
    (void)ms;
}


void os_task_delay_s( unsigned int s )
{
    (void)s;
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    (void)old_tick;
    (void)inc_ticks;
}



os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority )
{
    (void)name;
    (void)entry;
    (void)parm;
    (void)stack_bytes;
    (void)priority;
    return NULL;
}


os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
    (void)name;
    (void)entry;
    (void)parm;
    (void)stack_bytes;
    (void)priority;
    (void)buf;
    return NULL;
}


void os_task_delete( os_task_handle_t task )
{
    (void)task;
}


void os_task_suspend( os_task_handle_t task )
{
    (void)task;
}


void os_task_resume( os_task_handle_t task )
{
    (void)task;
}


void os_task_switch( void )
{
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    (void)task;
    (void)new_priority;
}


int os_task_priority_get( os_task_handle_t task )
{
    (void)task;
    return 0;
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    (void)name;
    (void)queue_length;
    (void)item_bytes;
    return NULL;;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    (void)name;
    (void)queue_length;
    (void)item_bytes;
    (void)buf;
    return NULL;
}


void os_queue_delete( os_queue_handle_t queue )
{
    (void)queue;
}


void os_queue_reset( os_queue_handle_t queue )
{
    (void)queue;
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    (void)queue;
    (void)data;
    (void)block_ticks;
    return 0;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    (void)queue;
    (void)data;
    (void)block_ticks;
    return 0;
}


int os_queue_put_isr( os_queue_handle_t queue, void *data )
{
    (void)queue;
    (void)data;
    return 0;
}


int os_queue_get_isr( os_queue_handle_t queue, void *data )
{
    (void)queue;
    (void)data;
    return 0;
}


int os_queue_count( os_queue_handle_t queue )
{
    (void)queue;
    return 0;
}


/* MUTEX */

os_mutex_handle_t os_mutex_create( void )
{
    return NULL;
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    (void)buf;
    return NULL;
}


void os_mutex_delete( os_mutex_handle_t mutex )
{
    (void)mutex;
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    (void)mutex;
    return 0;
}


int os_mutex_put_isr( os_mutex_handle_t mutex )
{
    (void)mutex;
    return 0;
}


int os_mutex_get( os_mutex_handle_t mutex, int block_ticks )
{
    (void)mutex;
    (void)block_ticks;
    return 0;
}


int os_mutex_get_isr( os_mutex_handle_t mutex )
{
    (void)mutex;
    return 0;
}


/* SEMAPHORE */

os_semaphore_handle_t os_semaphore_create( int max_count, int init_count )
{
    (void)max_count;
    (void)init_count;
    return NULL;
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
    (void)max_count;
    (void)init_count;
    (void)buf;
    return NULL;
}


void os_semaphore_delete( os_semaphore_handle_t semaphore )
{
    (void)semaphore;
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
    (void)semaphore;
    return 0;
}


int os_semaphore_put_isr( os_semaphore_handle_t semaphore )
{
    (void)semaphore;
    return 0;
}


int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks )
{
    (void)semaphore;
    (void)block_ticks;
    return 0;
}


int os_semaphore_get_isr( os_semaphore_handle_t semaphore )
{
    (void)semaphore;
    return 0;
}


/* TIMER */

os_timer_handle_t os_timer_create( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback )
{
    (void)name;
    (void)period_ticks;
    (void)repeat_mode;
    (void)callback;
    return NULL;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf )
{
    (void)name;
    (void)period_ticks;
    (void)repeat_mode;
    (void)callback;
    (void)buf;
    return NULL;
}


int os_timer_start( os_timer_handle_t timer )
{
    (void)timer;
    return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    (void)timer;
    return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    (void)timer;
    return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    (void)timer;
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    (void)bytes;
    return NULL;
}


void *os_realloc( void *old_mem, size_t bytes )
{
    (void)old_mem;
    (void)bytes;
    return NULL;
}


void os_free( void *mem )
{
    (void)mem;
}

#endif
