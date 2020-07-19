/* RTOS Abstract Layer for CMSIS-RTOS2-RTX5
 * only most frequently used APIs are ported, including
 * task/queue/semaphore/memory/timer
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush_os.h"

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
    //QueueHandle_t queue;

    //queue = xQueueCreate( queue_length, (unsigned portBASE_TYPE)item_bytes );
    //if( (queue != NULL) && (name != NULL) )
    //{
    //    vQueueAddToRegistry( queue, name );
    //}
    //return queue;
    return NULL;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    osMessageQueueId_t queue;
    osMessageQueueAttr_t attr;

    memset( (void*)&attr, 0, sizeof(attr) );
    attr.name = name;
    attr.cb_mem = (void*)buf->control;
    attr.cb_size = sizeof(osRtxMessageQueue_t);

    queue = osMessageQueueNew( queue_length, item_bytes, (const osMessageQueueAttr_t*)&attr);
    return queue;
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
    //return xTimerCreate( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback );
    return NULL;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buffer )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    //return xTimerCreateStatic( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback, (StaticTimer_t *)buffer );
    return NULL;
#else
    return NULL;
#endif
}



int os_timer_start( os_timer_handle_t timer )
{
    //if( xTimerStart( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    //if( xTimerStop( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    //if( xTimerReset( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    //xTimerDelete( timer, 0 );
}


void *os_malloc( size_t bytes )
{
    //return pvPortMalloc( bytes );
    return NULL;
}


void *os_realloc( void *old_mem, size_t bytes )
{
    //return pvPortRealloc( old_mem, bytes );
    return NULL;
}


void os_free( void *mem )
{
    //vPortFree( mem );
}


#if OS_SUPPORT_STATIC_ALLOCATION
///DEFINE_STATIC_TASK_BUFFER( idle, (configMINIMAL_STACK_SIZE*sizeof(portBASE_TYPE)) );
///
///void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
///{
///    *ppxIdleTaskTCBBuffer = static_task_buffer_idle.control;
///    *ppxIdleTaskStackBuffer = static_task_buffer_idle.stack;
///    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
///}
///
///#if configUSE_TIMERS
///DEFINE_STATIC_TASK_BUFFER( timer, (configTIMER_TASK_STACK_DEPTH*sizeof(portBASE_TYPE)) );
///
///void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
///{
///    *ppxTimerTaskTCBBuffer = static_task_buffer_timer.control;
///    *ppxTimerTaskStackBuffer = static_task_buffer_timer.stack;
///    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
///}
///#endif
///
///
///#if configAPPLICATION_ALLOCATED_HEAP
////* TODO: Fix this, link ucHeap into .userheap area.
///         Currently, the objcopy will generate incorrect 
///         binary file attached with unecessary zero data */
/////__attribute__((section(".userheap")))
///uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  /* for heap_4 */
///#endif
#endif



#endif
