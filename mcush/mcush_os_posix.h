/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_POSIX_H__
#define __MCUSH_OS_POSIX_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <errno.h>
#include <time.h>


#define OS_NAME          "POSIX"

#define OS_SUPPORT_STATIC_ALLOCATION  1

typedef int portSTACK_TYPE;

#define OS_STACK_SIZE_MIN        (1024)

#ifndef configMAX_PRIORITIES
    #define configMAX_PRIORITIES      10
#endif

#define OS_PRIORITY_MAX          (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHEST      (configMAX_PRIORITIES-1)
#define OS_PRIORITY_HIGHER       (configMAX_PRIORITIES/2+2)
#define OS_PRIORITY_HIGH         (configMAX_PRIORITIES/2+1)
#define OS_PRIORITY_NORMAL       (configMAX_PRIORITIES/2)
#define OS_PRIORITY_LOW          (configMAX_PRIORITIES/2-1)
#define OS_PRIORITY_LOWER        (configMAX_PRIORITIES/2-2)
#define OS_PRIORITY_LOWEST       (0)
#define OS_PRIORITY_MIN          (0)
#define OS_PRIORITY_IDLE         (0)

#ifndef OS_TICK_RATE
    #define OS_TICK_RATE     250u
#endif

#define OS_TICKS_MS(ms)  (ms*OS_TICK_RATE/1000)
#define OS_TICKS_S(s)    (s*OS_TICK_RATE)
#define OS_MS_PER_TICK   (1000/OS_TICK_RATE)

typedef unsigned int os_tick_t;
typedef pthread_t *os_task_handle_t;
typedef mqd_t *os_queue_handle_t;
typedef pthread_mutex_t *os_mutex_handle_t;
typedef sem_t *os_semaphore_handle_t;
typedef void *os_timer_handle_t;
typedef void *(*pthread_function_t)( void *arg );



typedef struct
{
    pthread_t *control;
    char *stack;
} static_task_buffer_t;
#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static pthread_t _static_task_control_##name;  \
    static char _static_task_stack_##name[stack_bytes/sizeof(portSTACK_TYPE)];  \
    static const static_task_buffer_t static_task_buffer_##name = {  \
        .control = &_static_task_control_##name,  \
        .stack = _static_task_stack_##name }
        
typedef struct
{
    mqd_t *control;
    uint8_t *data;
} static_queue_buffer_t;
#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static mqd_t _static_queue_control_##name;  \
    static uint8_t _static_queue_data_##name[queue_length*item_bytes];  \
    static const static_queue_buffer_t static_queue_buffer_##name = {  \
        .control = &_static_queue_control_##name,  \
        .data = _static_queue_data_##name } 

typedef pthread_mutex_t static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name

typedef sem_t static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef int static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
#else
 


#endif
