/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_RTX_H__
#define __MCUSH_OS_RTX_H__

#include "rtx_lib.h"

#define OS_NAME                 osRtxKernelId

#define OS_SUPPORT_STATIC_ALLOCATION  1

#define OS_STACK_SIZE_MIN       (128)

#define OS_PRIORITY_MAX         (osPriorityISR)
#define OS_PRIORITY_HIGHEST     (osPriorityRealtime)
#define OS_PRIORITY_HIGHER      (osPriorityHigh)
#define OS_PRIORITY_HIGH        (osPriorityAboveNormal)
#define OS_PRIORITY_NORMAL      (osPriorityNormal)
#define OS_PRIORITY_LOW         (osPriorityBelowNormal)
#define OS_PRIORITY_LOWER       (osPriorityLow)
#define OS_PRIORITY_LOWEST      (osPriorityIdle)
#define OS_PRIORITY_MIN         (osPriorityIdle)
#define OS_PRIORITY_IDLE        (osPriorityIdle)

#define OS_TICK_RATE     (OS_TICK_FREQ)
#define OS_TICKS_MS(ms)  (ms*OS_TICK_FREQ/1000)
#define OS_TICKS_S(s)    (s*OS_TICK_FREQ)

typedef uint32_t os_tick_t;
typedef osThreadId_t os_task_handle_t;
typedef osMessageQueueId_t os_queue_handle_t;
typedef osMutexId_t os_mutex_handle_t;
typedef osSemaphoreId_t os_semaphore_handle_t;
typedef osTimerId_t os_timer_handle_t;
typedef void (*os_timer_callback_t)( os_timer_handle_t timer );


typedef struct
{
    osRtxThread_t *control;
    uint64_t *stack;
} static_task_buffer_t;
#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static osRtxThread_t _static_task_control_##name;  \
    static uint64_t _static_task_stack_##name[stack_bytes/8];  \
    static const static_task_buffer_t static_task_buffer_##name = {  \
        .control = &_static_task_control_##name,  \
        .stack = _static_task_stack_##name }
        
typedef struct
{
    osRtxMessageQueue_t *control;
    uint8_t *data;
} static_queue_buffer_t;
#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static osRtxMessageQueue_t _static_queue_control_##name;  \
    static uint8_t _static_queue_data_##name[queue_length*item_bytes];  \
    static const static_queue_buffer_t static_queue_buffer_##name = {  \
        .control = &_static_queue_control_##name,  \
        .data = _static_queue_data_##name } 

typedef osRtxMutex_t static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name

typedef osRtxSemaphore_t static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef osRtxTimer_t static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
        

      


void os_init(void);
void os_start(void);
int os_is_running(void);
void os_enter_critical(void);
void os_exit_critical(void);

os_tick_t os_tick( void );

/* task */
void os_task_delay( os_tick_t tick );
void os_task_delay_ms( unsigned int ms );
void os_task_delay_s( unsigned int s );
void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks );
os_task_handle_t os_task_create( const char *name, void *entry, void *parm, size_t stack_bytes, int priority );
os_task_handle_t os_task_create_static( const char *name, void *entry, void *parm, size_t stack_bytes, int priority, const static_task_buffer_t *buf );
void os_task_delete( os_task_handle_t task );
void os_task_suspend( os_task_handle_t task );
void os_task_resume( os_task_handle_t task );
void os_task_switch( void );
void os_task_enter_critical( void );
void os_task_exit_critical( void );
void os_task_priority_set( os_task_handle_t task, int new_priority );
int os_task_priority_get( os_task_handle_t task );

/* queue */
os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes );
os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf );
void os_queue_delete( os_queue_handle_t queue );
void os_queue_reset( os_queue_handle_t queue );
int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks ); 
int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks ); 
int os_queue_put_isr( os_queue_handle_t queue, void *data ); 
int os_queue_get_isr( os_queue_handle_t queue, void *data ); 
//int os_queue_is_empty( os_queue_handle_t queue ); 
//int os_queue_is_full( os_queue_handle_t queue ); 

/* mutex */
os_mutex_handle_t os_mutex_create( void );
os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf );
void os_mutex_delete( os_mutex_handle_t *mutex );
int os_mutex_put( os_mutex_handle_t mutex );
int os_mutex_put_isr( os_mutex_handle_t mutex );
int os_mutex_get( os_mutex_handle_t mutex, int block_ticks );
int os_mutex_get_isr( os_mutex_handle_t mutex );

/* semaphore */
os_semaphore_handle_t os_semaphore_create( int max_count, int init_count );
os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf );
void os_semaphore_delete( os_semaphore_handle_t *semaphore );
int os_semaphore_put( os_semaphore_handle_t semaphore );
int os_semaphore_put_isr( os_semaphore_handle_t semaphore );
int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks );
int os_semaphore_get_isr( os_semaphore_handle_t semaphore );

/* timer */
os_timer_handle_t os_timer_create( const char *name, int period_ticks,
         int repeat_mode, os_timer_callback_t callback );
os_timer_handle_t os_timer_create_static( const char *name, int period_ticks,
         int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buffer );
int os_timer_start( os_timer_handle_t timer );
int os_timer_stop( os_timer_handle_t timer );
int os_timer_reset( os_timer_handle_t timer );
void os_timer_delete( os_timer_handle_t timer );

/* memory */
void *os_malloc( size_t bytes );
void *os_realloc( void *old_mem, size_t bytes );
void os_free( void *mem );



#endif
