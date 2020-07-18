/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_FREERTOS_H__
#define __MCUSH_OS_FREERTOS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "mcush_freertos_api.h"

#define OS_NAME          "FreeRTOS"

#if configSUPPORT_STATIC_ALLOCATION
    #define OS_SUPPORT_STATIC_ALLOCATION  1
#else
    #define OS_SUPPORT_STATIC_ALLOCATION  0
#endif

#define OS_STACK_SIZE_MIN        (configMINIMAL_STACK_SIZE*sizeof(portBASE_TYPE))

#define OS_PRIORITY_MAX          (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHEST      (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHER       (configMAX_PRIORITIES/2+2)
#define OS_PRIORITY_HIGH         (configMAX_PRIORITIES/2+1)
#define OS_PRIORITY_NORMAL       (configMAX_PRIORITIES/2)
#define OS_PRIORITY_LOW          (configMAX_PRIORITIES/2-1)
#define OS_PRIORITY_LOWER        (configMAX_PRIORITIES/2-2)
#define OS_PRIORITY_LOWEST       (0)
#define OS_PRIORITY_MIN          (0)

#define OS_TICK_RATE     (configTICK_RATE_HZ)
#define OS_TICKS_MS(ms)  (ms*configTICK_RATE_HZ/1000)
#define OS_TICKS_S(s)    (s*configTICK_RATE_HZ)

typedef TickType_t os_tick_t;
typedef TaskHandle_t os_task_handle_t;
typedef QueueHandle_t os_queue_handle_t;
typedef SemaphoreHandle_t os_semaphore_handle_t;
typedef TimerHandle_t os_timer_handle_t;
typedef void (*os_timer_callback_t)( os_timer_handle_t timer );


typedef struct
{
    StaticTask_t *control;
    StackType_t *stack;
} static_task_buffer_t;

#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static StaticTask_t _static_task_control_##name;  \
    static StackType_t _static_task_stack_##name[stack_bytes/sizeof(portSTACK_TYPE)];  \
    static const static_task_buffer_t static_task_buffer_##name = {  \
        .control = &_static_task_control_##name,  \
        .stack = _static_task_stack_##name }
        
typedef struct
{
    StaticQueue_t *control;
    uint8_t *data;
} static_queue_buffer_t;

#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static StaticQueue_t _static_queue_control_##name;  \
    static uint8_t _static_queue_data_##name[queue_length*item_bytes];  \
    static const static_queue_buffer_t static_queue_buffer_##name = {  \
        .control = &_static_queue_control_##name,  \
        .data = _static_queue_data_##name } 

typedef StaticSemaphore_t static_semaphore_buffer_t;

#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        

typedef StaticTimer_t static_timer_buffer_t;

#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
        

      


void os_init(void);
void os_start(void);
int os_is_running(void);

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

/* semaphore */
os_semaphore_handle_t os_semaphore_create_mutex( void );
os_semaphore_handle_t os_semaphore_create_mutex_static( static_semaphore_buffer_t *buf );
os_semaphore_handle_t os_semaphore_create_binary( void );
os_semaphore_handle_t os_semaphore_create_binary_static( static_semaphore_buffer_t *buf );
os_semaphore_handle_t os_semaphore_create_count( int max_count, int init_count );
os_semaphore_handle_t os_semaphore_create_count_static( int max_count, int init_count, static_semaphore_buffer_t *buf );
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
