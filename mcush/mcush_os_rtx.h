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
    static uint8_t _static_queue_data_##name[osRtxMessageQueueMemSize(queue_length,item_bytes)];  \
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
        

      
#endif
