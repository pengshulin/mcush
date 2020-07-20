/* RTOS Abstract Layer for CMSIS-RTOS2-RTX5
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_RTX



void os_init(void)
{
    osKernelInitialize();
}


void os_start(void)
{
    osKernelStart();
}


int os_is_running(void)
{
    if( osKernelGetState() == osKernelRunning )
        return 1;
    else
        return 0;
}


void os_enter_critical(void)
{

}


void os_exit_critical(void)
{

}


os_tick_t os_tick( void )
{
    return osKernelGetTickCount();
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    osDelay( ticks );
}


void os_task_delay_ms( unsigned int ms )
{
    osDelay( OS_TICKS_MS(ms) );
}


void os_task_delay_s( unsigned int s )
{
    osDelay( OS_TICKS_S(s) );
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    *old_tick = *old_tick + inc_ticks;
    osDelayUntil( *old_tick );
}


os_task_handle_t os_task_create( const char *name, void *entry, void *parm, size_t stack_bytes, int priority )
{
    osThreadId_t task;
    osThreadAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    attr.stack_size = stack_bytes;
    attr.priority = priority;

    task = osThreadNew((osThreadFunc_t)entry, parm, (const osThreadAttr_t*)&attr);
    return task;
}


os_task_handle_t os_task_create_static( const char *name, void *entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
    osThreadId_t task;
    osThreadAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    attr.cb_mem = (void*)(buf->control);
    attr.cb_size = sizeof(osRtxThread_t);
    attr.stack_mem = (void*)buf->stack;
    attr.stack_size = stack_bytes;
    attr.priority = priority;

    task = osThreadNew((osThreadFunc_t)entry, parm, (const osThreadAttr_t*)&attr);
    return task;
}


void os_task_delete( os_task_handle_t task )
{
    osThreadTerminate( task );
}


void os_task_suspend( os_task_handle_t task )
{
    osThreadSuspend( task );
}


void os_task_resume( os_task_handle_t task )
{
    osThreadResume( task );
}


void os_task_switch( void )
{
    osThreadYield();
}


void os_task_enter_critical( void )
{
    osKernelLock();
}


void os_task_exit_critical( void )
{
    osKernelUnlock();
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    osThreadSetPriority( task, (osPriority_t)new_priority );
}


int os_task_priority_get( os_task_handle_t task )
{
    return osThreadGetPriority( task );
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    osMessageQueueAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    return osMessageQueueNew( queue_length, item_bytes, (const osMessageQueueAttr_t*)&attr);
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    osMessageQueueAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    attr.cb_mem = (void*)buf->control;
    attr.cb_size = sizeof(osRtxMessageQueue_t);
    attr.mq_mem = (void*)buf->data;
    attr.mq_size = queue_length*ITEM_SIZE_TO_BLOCK_SIZE(item_bytes);
    return osMessageQueueNew( queue_length, item_bytes, (const osMessageQueueAttr_t*)&attr);
}


void os_queue_delete( os_queue_handle_t queue )
{
    osMessageQueueDelete( queue );
}


void os_queue_reset( os_queue_handle_t queue )
{
    osMessageQueueReset( queue );
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    //if( block_ticks < 0 )
    //    block_ticks = portMAX_DELAY;
    if( osMessageQueuePut( queue, data, 0, block_ticks ) != osOK )
        return 0;
    else
        return 1;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    //if( block_ticks < 0 )
    //    block_ticks = portMAX_DELAY;
    if( osMessageQueueGet( queue, data, 0, block_ticks ) != osOK )
        return 0;
    else
        return 1;
}


int os_queue_put_isr( os_queue_handle_t queue, void *data )
{
    return os_queue_put( queue, data, 0 );
}


int os_queue_get_isr( os_queue_handle_t queue, void *data )
{
    return os_queue_get( queue, data, 0 );
}


int os_queue_count( os_queue_handle_t queue )
{
    return osMessageQueueGetCount( queue );
}

//int os_queue_is_empty( os_queue_handle_t queue )
//{
//
//}
//
//
//int os_queue_is_full( os_queue_handle_t queue )
//{
//
//}


/* MUTEX */

os_mutex_handle_t os_mutex_create( void )
{
    osMutexAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    return osMutexNew( &attr );
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    osMutexAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.cb_mem = (void*)buf;
    attr.cb_size = sizeof(osRtxMutex_t); 
    return osMutexNew( &attr );
}


void os_mutex_delete( os_mutex_handle_t *mutex )
{
    osMutexDelete( mutex );
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    if( osMutexRelease( mutex ) == osOK )
        return 1;
    else
        return 0;
}


int os_mutex_put_isr( os_mutex_handle_t mutex )
{
    return os_mutex_put( mutex );
}


int os_mutex_get( os_mutex_handle_t mutex, int block_ticks )
{
    //if( block_ticks < 0 )
    //    block_ticks = portMAX_DELAY;
    if( osMutexAcquire( mutex, block_ticks ) == osOK )
        return 1;
    else
        return 0;
}


int os_mutex_get_isr( os_mutex_handle_t mutex )
{
    return os_mutex_get( mutex, 0 );
}


/* SEMAPHORE */

os_semaphore_handle_t os_semaphore_create( int max_count, int init_count )
{
    osSemaphoreAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    return osSemaphoreNew( max_count, init_count, &attr );
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
    osSemaphoreAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.cb_mem = (void*)buf;
    attr.cb_size = sizeof(osRtxSemaphore_t); 
    return osSemaphoreNew( max_count, init_count, &attr );
}


void os_semaphore_delete( os_semaphore_handle_t *semaphore )
{
    osSemaphoreDelete( semaphore );
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
    if( osSemaphoreRelease( semaphore ) == osOK )
        return 1;
    else
        return 0;
}


int os_semaphore_put_isr( os_semaphore_handle_t semaphore )
{
    return os_semaphore_put( semaphore );
}


int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks )
{
    //if( block_ticks < 0 )
    //    block_ticks = portMAX_DELAY;
    if( osSemaphoreAcquire( semaphore, block_ticks ) == osOK )
        return 1;
    else
        return 0;
}


int os_semaphore_get_isr( os_semaphore_handle_t semaphore )
{
    return os_semaphore_get( semaphore, 0 );
}


/* TIMER */

os_timer_handle_t os_timer_create( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback )
{
    osTimerAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    return osTimerNew( callback, repeat_mode ? osTimerPeriodic : osTimerOnce, 0, &attr );
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buffer )
{
    osTimerAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    attr.cb_mem = (void*)buffer;
    attr.cb_size = sizeof(osRtxTimer_t);
    return osTimerNew( callback, repeat_mode ? osTimerPeriodic : osTimerOnce, 0, &attr );
}



int os_timer_start( os_timer_handle_t timer )
{
    if( osTimerStart( timer, 0 ) == osOK )
        return 1;
    else
        return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    if( osTimerStop( timer ) == osOK )
        return 1;
    else
        return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    osTimerDelete( timer );
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    void *p;

    osKernelLock();
    p = malloc( bytes );
    osKernelUnlock();
    return p;
}


void *os_realloc( void *old_mem, size_t bytes )
{
    return old_mem;
}


void os_free( void *mem )
{
    if( mem )
    {
        osKernelLock();
        free( mem );
        osKernelUnlock();
    }
}



#endif
