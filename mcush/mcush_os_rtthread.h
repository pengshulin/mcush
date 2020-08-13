/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_RTTHREAD_H__
#define __MCUSH_OS_RTTHREAD_H__
#include "rtthread.h"
#include "rthw.h"

#define OS_NAME          "RTThread"

#define OS_SUPPORT_STATIC_ALLOCATION  1

#define OS_STACK_SIZE_MIN        (128)

#define OS_PRIORITY_MAX          (RT_THREAD_PRIORITY_MAX)
#define OS_PRIORITY_HIGHEST      (RT_THREAD_PRIORITY_MAX)
#define OS_PRIORITY_HIGHER       (RT_THREAD_PRIORITY_MAX/2+2)
#define OS_PRIORITY_HIGH         (RT_THREAD_PRIORITY_MAX/2+1)
#define OS_PRIORITY_NORMAL       (RT_THREAD_PRIORITY_MAX/2)
#define OS_PRIORITY_LOW          (RT_THREAD_PRIORITY_MAX/2-1)
#define OS_PRIORITY_LOWER        (RT_THREAD_PRIORITY_MAX/2-2)
#define OS_PRIORITY_LOWEST       (0)
#define OS_PRIORITY_MIN          (0)
#define OS_PRIORITY_IDLE         (0)

#define OS_TICK_RATE     RT_TICK_PER_SECOND
#define OS_TICKS_MS(ms)  (ms*RT_TICK_PER_SECOND/1000)
#define OS_TICKS_S(s)    (s*RT_TICK_PER_SECOND)

typedef rt_tick_t os_tick_t;
typedef rt_thread_t os_task_handle_t;
typedef rt_mq_t os_queue_handle_t;
typedef rt_mutex_t os_mutex_handle_t;
typedef rt_sem_t os_semaphore_handle_t;
typedef rt_timer_t os_timer_handle_t;
typedef void (*os_timer_callback_t)( void *arg );
typedef void (*os_task_function_t)( void *arg );


typedef struct
{
    struct rt_thread *control;
    uint32_t *stack;
} static_task_buffer_t;
#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static struct rt_thread _static_task_control_##name;  \
    static uint32_t _static_task_stack_##name[stack_bytes/4];  \
    static const static_task_buffer_t static_task_buffer_##name = {  \
        .control = (struct rt_thread *)&_static_task_control_##name,  \
        .stack = (uint32_t *)&_static_task_stack_##name }
        
typedef struct
{
    struct rt_messagequeue *control;
    uint8_t *data;
} static_queue_buffer_t;
#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static struct rt_messagequeue _static_queue_control_##name;  \
    static uint8_t _static_queue_data_##name[queue_length*item_bytes];  \
    static const static_queue_buffer_t static_queue_buffer_##name = {  \
        .control = (struct rt_messagequeue *)&_static_queue_control_##name,  \
        .data = (uint8_t *)_static_queue_data_##name } 

typedef struct rt_mutex static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name
 
typedef struct rt_semaphore static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef struct rt_timer static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
        


#endif
