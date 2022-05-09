/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_THREADX_H__
#define __MCUSH_OS_THREADX_H__
#include "tx_api.h"
#include "tx_thread.h"
#include "tx_queue.h"
#include "tx_mutex.h"
#include "tx_semaphore.h"
#include "tx_timer.h"


#define OS_NAME          "ThreadX"

#define OS_SUPPORT_STATIC_ALLOCATION  1

#define OS_STACK_SIZE_MIN        (TX_MINIMUM_STACK)

#define OS_PRIORITY_MAX          (0)
#define OS_PRIORITY_HIGHEST      (0)
#define OS_PRIORITY_HIGHER       (TX_MAX_PRIORITIES/2-2)
#define OS_PRIORITY_HIGH         (TX_MAX_PRIORITIES/2-1)
#define OS_PRIORITY_NORMAL       (TX_MAX_PRIORITIES/2)
#define OS_PRIORITY_LOW          (TX_MAX_PRIORITIES/2+1)
#define OS_PRIORITY_LOWER        (TX_MAX_PRIORITIES/2+2)
#define OS_PRIORITY_LOWEST       (TX_MAX_PRIORITIES-1)
#define OS_PRIORITY_MIN          (TX_MAX_PRIORITIES-1)
#define OS_PRIORITY_IDLE         (OS_PRIORITY_LOWEST)

#define OS_TICK_RATE     (TX_TIMER_TICKS_PER_SECOND)
#define OS_TICKS_MS(ms)  (ms*OS_TICK_RATE/1000)
#define OS_TICKS_S(s)    (s*OS_TICK_RATE)
#define OS_MS_PER_TICK   (1000/OS_TICK_RATE)

typedef ULONG os_tick_t;
typedef TX_THREAD *os_task_handle_t;
typedef TX_QUEUE *os_queue_handle_t;
typedef TX_MUTEX *os_mutex_handle_t;
typedef TX_SEMAPHORE *os_semaphore_handle_t;
typedef TX_TIMER *os_timer_handle_t;

typedef VOID (*_os_task_entry_t)(ULONG parm);
typedef VOID (*_os_timer_callback_t)(ULONG parm);

typedef struct
{
    TX_THREAD *control;
    long *stack;
} static_task_buffer_t;
#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static TX_THREAD _static_task_control_##name;  \
    static long _static_task_stack_##name[(stack_bytes+sizeof(long)-1)/sizeof(long)];  \
    static const static_task_buffer_t static_task_buffer_##name = {  \
        .control = &_static_task_control_##name,  \
        .stack = _static_task_stack_##name }
        
typedef struct
{
    TX_QUEUE *control;
    ULONG *data;
} static_queue_buffer_t;
#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static TX_QUEUE _static_queue_control_##name;  \
    static ULONG _static_queue_data_##name[queue_length*sizeof(ULONG)*((item_bytes+sizeof(ULONG)-1)/sizeof(ULONG))];  \
    static const static_queue_buffer_t static_queue_buffer_##name = {  \
        .control = &_static_queue_control_##name,  \
        .data = _static_queue_data_##name } 

typedef TX_MUTEX static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name

typedef TX_SEMAPHORE static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef TX_TIMER static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
      

#endif
