/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_FREERTOS_H__
#define __MCUSH_OS_FREERTOS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "mcush_freertos_api.h"

#define OS_NAME          "FreeRTOS " tskKERNEL_VERSION_NUMBER

#if configSUPPORT_STATIC_ALLOCATION
    #define OS_SUPPORT_STATIC_ALLOCATION  1
#else
    #define OS_SUPPORT_STATIC_ALLOCATION  0
#endif

#define OS_STACK_SIZE_MIN        (configMINIMAL_STACK_SIZE*sizeof(portSTACK_TYPE))

#define OS_PRIORITY_MAX          (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHEST      (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHER       (configMAX_PRIORITIES/2+2)
#define OS_PRIORITY_HIGH         (configMAX_PRIORITIES/2+1)
#define OS_PRIORITY_NORMAL       (configMAX_PRIORITIES/2)
#define OS_PRIORITY_LOW          (configMAX_PRIORITIES/2-1)
#define OS_PRIORITY_LOWER        (configMAX_PRIORITIES/2-2)
#define OS_PRIORITY_LOWEST       (0)
#define OS_PRIORITY_MIN          (0)
#define OS_PRIORITY_IDLE         (tskIDLE_PRIORITY)

#define OS_TICK_RATE     configTICK_RATE_HZ
#define OS_TICKS_MS(ms)  (ms*configTICK_RATE_HZ/1000)
#define OS_TICKS_S(s)    (s*configTICK_RATE_HZ)

typedef TickType_t os_tick_t;
typedef TaskHandle_t os_task_handle_t;
typedef QueueHandle_t os_queue_handle_t;
typedef SemaphoreHandle_t os_mutex_handle_t;
typedef SemaphoreHandle_t os_semaphore_handle_t;
typedef TimerHandle_t os_timer_handle_t;

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

typedef StaticSemaphore_t static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name
 
typedef StaticSemaphore_t static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef StaticTimer_t static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
      

#endif
