/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_OS_NONE_H__
#define __MCUSH_OS_NONE_H__

#define OS_NAME          "NONE"

#define OS_STACK_SIZE_MIN        (100)

#define OS_PRIORITY_MAX          (0)
#define OS_PRIORITY_HIGHEST      (0)
#define OS_PRIORITY_HIGHER       (0)
#define OS_PRIORITY_HIGH         (0)
#define OS_PRIORITY_NORMAL       (0)
#define OS_PRIORITY_LOW          (0)
#define OS_PRIORITY_LOWER        (0)
#define OS_PRIORITY_LOWEST       (0)
#define OS_PRIORITY_MIN          (0)
#define OS_PRIORITY_IDLE         (0)


#define OS_TICK_RATE     250  /* pesudo defination */
#define OS_TICKS_MS(ms)  (ms*OS_TICK_RATE/1000)
#define OS_TICKS_S(s)    (s*OS_TICK_RATE)
#define OS_MS_PER_TICK   (1000/OS_TICK_RATE)

typedef unsigned int os_tick_t;
typedef void *os_task_handle_t;
typedef void *os_queue_handle_t;
typedef void *os_mutex_handle_t;
typedef void *os_semaphore_handle_t;
typedef void *os_timer_handle_t;

typedef void static_task_buffer_t;
#define DEFINE_STATIC_TASK_BUFFER( name, stack_bytes )  \
    static const static_task_buffer_t static_task_buffer_##name
        
typedef void static_queue_buffer_t;
#define DEFINE_STATIC_QUEUE_BUFFER( name, queue_length, item_bytes )  \
    static const static_queue_buffer_t static_queue_buffer_##name

typedef void static_mutex_buffer_t;
#define DEFINE_STATIC_MUTEX_BUFFER( name )  \
    static static_mutex_buffer_t static_mutex_buffer_##name

typedef void static_semaphore_buffer_t;
#define DEFINE_STATIC_SEMAPHORE_BUFFER( name )  \
    static static_semaphore_buffer_t static_semaphore_buffer_##name
        
typedef void static_timer_buffer_t;
#define DEFINE_STATIC_TIMER_BUFFER( name )  \
    static static_timer_buffer_t static_timer_buffer_##name
     

#endif
