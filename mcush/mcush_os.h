/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_H__
#define __MCUSH_OS_H__

#define OS_NONE      0
#define OS_FREERTOS  1
#define OS_RTX       2
#define OS_THREADX   3
#define OS_RTTHREAD  4
#define OS_POSIX     5
#define OS_CHIBIOS   6
#define OS_LITEOS    7

#ifndef MCUSH_OS
    #define MCUSH_OS  OS_FREERTOS
#endif

#if MCUSH_OS == OS_NONE
    #include "mcush_os_none.h"
#elif MCUSH_OS == OS_FREERTOS
    #include "mcush_os_freertos.h"
#elif MCUSH_OS == OS_RTX
    #include "mcush_os_rtx.h"
#elif MCUSH_OS == OS_THREADX
    #include "mcush_os_threadx.h"
#elif MCUSH_OS == OS_RTTHREAD
    #include "mcush_os_rtthread.h"
#elif MCUSH_OS == OS_POSIX
    #include "mcush_os_posix.h"
#elif MCUSH_OS == OS_CHIBIOS
    #include "mcush_os_chibios.h"
#elif MCUSH_OS == OS_LITEOS
    #include "mcush_os_liteos.h"
#else
    #error "MCUSH_OS not defined"
#endif


typedef void (*os_timer_callback_t)( os_timer_handle_t timer );
typedef void (*os_task_function_t)( void *arg );

/* APIS */

void os_init(void);
void os_start(void);
int os_is_running(void);
void os_enter_critical(void);
void os_exit_critical(void);
void os_disable_interrupts(void);

os_tick_t os_tick( void );

/* task */
void os_task_delay( os_tick_t tick );
void os_task_delay_ms( unsigned int ms );
void os_task_delay_s( unsigned int s );
void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks );
os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority );
os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority, const static_task_buffer_t *buf );
void os_task_delete( os_task_handle_t task );
void os_task_suspend( os_task_handle_t task );
void os_task_resume( os_task_handle_t task );
void os_task_switch( void );
void os_task_priority_set( os_task_handle_t task, int new_priority );
int os_task_priority_get( os_task_handle_t task );

/* queue */
os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes );
os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf );
void os_queue_delete( os_queue_handle_t queue );
void os_queue_reset( os_queue_handle_t queue );
int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks ); 
int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks ); 
int os_queue_peek( os_queue_handle_t queue, void *data, int block_ticks );
int os_queue_put_isr( os_queue_handle_t queue, void *data ); 
int os_queue_get_isr( os_queue_handle_t queue, void *data ); 
int os_queue_count( os_queue_handle_t queue );
int os_queue_count_isr( os_queue_handle_t queue );
int os_queue_is_empty( os_queue_handle_t queue ); 
int os_queue_is_empty_isr( os_queue_handle_t queue ); 
int os_queue_is_full( os_queue_handle_t queue ); 
int os_queue_is_full_isr( os_queue_handle_t queue ); 

/* mutex */
os_mutex_handle_t os_mutex_create( void );
os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf );
void os_mutex_delete( os_mutex_handle_t mutex );
int os_mutex_put( os_mutex_handle_t mutex );
int os_mutex_put_isr( os_mutex_handle_t mutex );
int os_mutex_get( os_mutex_handle_t mutex, int block_ticks );
int os_mutex_get_isr( os_mutex_handle_t mutex );

/* semaphore */
os_semaphore_handle_t os_semaphore_create( int max_count, int init_count );
os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf );
void os_semaphore_delete( os_semaphore_handle_t semaphore );
int os_semaphore_put( os_semaphore_handle_t semaphore );
int os_semaphore_put_isr( os_semaphore_handle_t semaphore );
int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks );
int os_semaphore_get_isr( os_semaphore_handle_t semaphore );
int os_semaphore_count( os_semaphore_handle_t semaphore );

/* timer */
os_timer_handle_t os_timer_create( const char *name, int period_ticks,
         int repeat_mode, os_timer_callback_t callback );
os_timer_handle_t os_timer_create_static( const char *name, int period_ticks,
         int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf );
int os_timer_start( os_timer_handle_t timer );
int os_timer_stop( os_timer_handle_t timer );
int os_timer_reset( os_timer_handle_t timer );
void os_timer_delete( os_timer_handle_t timer );

/* memory */
void *os_malloc( size_t bytes );
void *os_realloc( void *old_mem, size_t bytes );
void os_free( void *mem );

/* misc */
void os_task_info_print(void);
void os_queue_info_print(void);
void os_kernel_info_print(void);


#endif
