/* RTOS Abstract Layer for FreeRTOS 
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_FREERTOS



void os_init(void)
{
}


void os_start(void)
{
    vTaskStartScheduler();
}


int os_is_running(void)
{
    if( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
        return 1;
    else
        return 0;
}


os_tick_t os_tick( void )
{
    return xTaskGetTickCount();
}


void os_enter_critical(void)
{
    portENTER_CRITICAL();
}


void os_exit_critical(void)
{
    portEXIT_CRITICAL();
}


void os_disable_interrupts(void)
{
    portDISABLE_INTERRUPTS();
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    vTaskDelay( ticks );
}


void os_task_delay_ms( unsigned int ms )
{
    vTaskDelay( OS_TICKS_MS(ms) );
}


void os_task_delay_s( unsigned int s )
{
    vTaskDelay( OS_TICKS_S(s) );
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    vTaskDelayUntil( old_tick, inc_ticks );
}



os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority )
{
    TaskHandle_t task;

    if( xTaskCreate( (TaskFunction_t)entry, name, stack_bytes/sizeof(portSTACK_TYPE), parm, priority, &task ) == pdPASS )
        return task;
    else
        return NULL;
}


os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    return xTaskCreateStatic( (TaskFunction_t)entry, name, stack_bytes/sizeof(portSTACK_TYPE), parm, priority, buf->stack, buf->control );
#else
    return NULL;
#endif
}


void os_task_delete( os_task_handle_t task )
{
    vTaskDelete( task );
}


void os_task_suspend( os_task_handle_t task )
{
    vTaskSuspend( task );
}


void os_task_resume( os_task_handle_t task )
{
    vTaskResume( task );
}


void os_task_switch( void )
{
    taskYIELD();
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    vTaskPrioritySet( task, (UBaseType_t)new_priority );
}


int os_task_priority_get( os_task_handle_t task )
{
    return uxTaskPriorityGet( task );
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    QueueHandle_t queue;

    queue = xQueueCreate( queue_length, (unsigned portBASE_TYPE)item_bytes );
    if( (queue != NULL) && (name != NULL) )
    {
        vQueueAddToRegistry( queue, name );
    }
    return queue;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    QueueHandle_t queue;

    queue = xQueueCreateStatic( queue_length, (unsigned portBASE_TYPE)item_bytes, buf->data, buf->control );
    if( (queue != NULL) && (name != NULL) )
    {
        vQueueAddToRegistry( queue, name );
    }
    return queue;
#else
    return NULL;
#endif
}


void os_queue_delete( os_queue_handle_t queue )
{
    vQueueDelete( queue );
}


void os_queue_reset( os_queue_handle_t queue )
{
    xQueueReset( queue );
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = portMAX_DELAY;
    if( xQueueSend( queue, data, block_ticks ) != pdPASS )
        return 0;
    else
        return 1;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = portMAX_DELAY;
    if( xQueueReceive( queue, data, block_ticks ) != pdPASS )
        return 0;
    else
        return 1;
}


int os_queue_peek( os_queue_handle_t queue, void *data, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = portMAX_DELAY;
    if( xQueuePeek( queue, data, block_ticks ) != pdPASS )
        return 0;
    else
        return 1;
}


int os_queue_put_isr( os_queue_handle_t queue, void *data )
{
    if( xQueueSendFromISR( queue, data, NULL ) == errQUEUE_FULL )
        return 0;
    else
        return 1;
}


int os_queue_get_isr( os_queue_handle_t queue, void *data )
{
    if( xQueueReceiveFromISR( queue, data, NULL ) == pdFAIL )
        return 0;
    else
        return 1;
}


int os_queue_count( os_queue_handle_t queue )
{
    return uxQueueMessagesWaiting( queue );
}


int os_queue_count_isr( os_queue_handle_t queue )
{
    return uxQueueMessagesWaitingFromISR( queue );
}


int os_queue_is_empty( os_queue_handle_t queue )
{
    return xQueueIsQueueEmpty( queue );
}


int os_queue_is_empty_isr( os_queue_handle_t queue )
{
    return xQueueIsQueueEmptyFromISR( queue );
}


int os_queue_is_full( os_queue_handle_t queue )
{
    return xQueueIsQueueFull( queue );
}


int os_queue_is_full_isr( os_queue_handle_t queue )
{
    return xQueueIsQueueFullFromISR( queue );
}



/* MUTEX */

os_mutex_handle_t os_mutex_create( void )
{
    return xSemaphoreCreateMutex();
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    return xSemaphoreCreateMutexStatic( (StaticSemaphore_t*)buf );
#else
    return NULL;
#endif
}


void os_mutex_delete( os_mutex_handle_t mutex )
{
    vSemaphoreDelete( mutex );
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    if( xSemaphoreGive( mutex ) == pdFAIL )
        return 0;
    else
        return 1;
}


int os_mutex_put_isr( os_mutex_handle_t mutex )
{
    if( xSemaphoreGiveFromISR( mutex, NULL ) == pdTRUE )
        return 1;
    else
        return 0;
}


int os_mutex_get( os_mutex_handle_t mutex, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = portMAX_DELAY;
    if( xSemaphoreTake( mutex, block_ticks ) == pdFAIL )
        return 0;
    else
        return 1;
}


int os_mutex_get_isr( os_mutex_handle_t mutex )
{
    if( xSemaphoreTakeFromISR( mutex, NULL ) == pdFAIL )
        return 0;
    else
        return 1;
}


/* SEMAPHORE */

os_semaphore_handle_t os_semaphore_create( int max_count, int init_count )
{
    return xSemaphoreCreateCounting( max_count, init_count );
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    return xSemaphoreCreateCountingStatic( max_count, init_count, (StaticSemaphore_t*)buf );
#else
    return NULL;
#endif
}


void os_semaphore_delete( os_semaphore_handle_t semaphore )
{
    vSemaphoreDelete( semaphore );
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
    if( xSemaphoreGive( semaphore ) == pdFAIL )
        return 0;
    else
        return 1;
}


int os_semaphore_put_isr( os_semaphore_handle_t semaphore )
{
    if( xSemaphoreGiveFromISR( semaphore, NULL ) == pdTRUE )
        return 1;
    else
        return 0;
}


int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = portMAX_DELAY;
    if( xSemaphoreTake( semaphore, block_ticks ) == pdFAIL )
        return 0;
    else
        return 1;
}


int os_semaphore_get_isr( os_semaphore_handle_t semaphore )
{
    if( xSemaphoreTakeFromISR( semaphore, NULL ) == pdFAIL )
        return 0;
    else
        return 1;
}


/* TIMER */

os_timer_handle_t os_timer_create( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback )
{
    return xTimerCreate( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback );
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf )
{
#if OS_SUPPORT_STATIC_ALLOCATION
    return xTimerCreateStatic( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback, (StaticTimer_t *)buf );
#else
    return NULL;
#endif
}


int os_timer_start( os_timer_handle_t timer )
{
    if( xTimerStart( timer, 0 ) == pdPASS )
        return 1;
    else
        return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    if( xTimerStop( timer, 0 ) == pdPASS )
        return 1;
    else
        return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    if( xTimerReset( timer, 0 ) == pdPASS )
        return 1;
    else
        return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    xTimerDelete( timer, 0 );
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    return pvPortMalloc( bytes );
}


void *os_realloc( void *old_mem, size_t bytes )
{
    return pvPortRealloc( old_mem, bytes );
}


void os_free( void *mem )
{
    vPortFree( mem );
}


#if OS_SUPPORT_STATIC_ALLOCATION
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    DEFINE_STATIC_TASK_BUFFER( idle, (configMINIMAL_STACK_SIZE*sizeof(portSTACK_TYPE)) );
    *ppxIdleTaskTCBBuffer = static_task_buffer_idle.control;
    *ppxIdleTaskStackBuffer = static_task_buffer_idle.stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if configUSE_TIMERS
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    DEFINE_STATIC_TASK_BUFFER( timer, (configTIMER_TASK_STACK_DEPTH*sizeof(portSTACK_TYPE)) );
    *ppxTimerTaskTCBBuffer = static_task_buffer_timer.control;
    *ppxTimerTaskStackBuffer = static_task_buffer_timer.stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif


#if configAPPLICATION_ALLOCATED_HEAP
/* TODO: Fix this, link ucHeap into .userheap area.
         Currently, the objcopy will generate incorrect 
         binary file attached with unecessary zero data */
//__attribute__((section(".userheap")))
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  /* for heap_4 */
#endif
#endif


void vApplicationTickHook(void) 
{
}


void vApplicationIdleHook(void) 
{
}


void vApplicationMallocFailedHook(void) 
{
    halt("malloc fail");
}

     
void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
//void vApplicationStackOverflowHook( xTaskHandle xTask, signed portCHAR *pcTaskName )
{
    (void)xTask;
    (void)pcTaskName;
    halt("stack overflow");
}


#define TASKS_LIMIT  30
void os_task_info_print(void)
{
    /* sizeof(TaskStatus_t) = 36 bytes */
    TaskStatus_t task_status_array[TASKS_LIMIT];  /* 30*36=1080 bytes */
    int i, count;
    char c;

    count = uxTaskGetSystemState( task_status_array, TASKS_LIMIT, NULL );
    for( i=0; i<count; i++ )
    {
        /* status */
        switch( task_status_array[i].eCurrentState )
        {
        case eRunning: c = 'X'; break;
        case eReady: c = 'R'; break;
        case eBlocked: c = 'B'; break;
        case eSuspended: c = 'S'; break;
        case eDeleted: c = 'D'; break;
        default: c = '?'; break;
        } 
        
        shell_printf( "%2d %8s %c 0x%08X %d/%d 0x%08X 0x%08X (free %d)\n",
                    task_status_array[i].xTaskNumber,
                    task_status_array[i].pcTaskName, c, 
                    task_status_array[i].xHandle,
                    task_status_array[i].uxCurrentPriority, task_status_array[i].uxBasePriority, 
                    task_status_array[i].pxStackBase,
                    *(uint32_t*)task_status_array[i].xHandle,  /* pxTopOfStack is at the begining of TCB_t */
                    //(uint32_t*)mcushGetTaskStackTop( task_status_array[i].xHandle ),
                    task_status_array[i].usStackHighWaterMark * sizeof(portSTACK_TYPE) );
    } 
}


void os_queue_info_print(void)
{
    mcush_queue_info_t qinfo;
    os_queue_handle_t queue;
    const char *name=0;
    int i;

    for( i=0; i<configQUEUE_REGISTRY_SIZE; i++ )
    {
        if( mcushGetQueueRegistered( i, (void**)&queue, &name ) )
        {
            if( mcushGetQueueInfo( queue, &qinfo ) )
            {
                shell_printf( "%8s 0x%08X  %5d %4d %4d  0x%08X - 0x%08X (0x%08X)\n", name, (int)queue, 
                    qinfo.uxLength, qinfo.uxItemSize, qinfo.uxMessagesWaiting, 
                    (int)qinfo.pcHead, (int)qinfo.pcTail, ((int)qinfo.pcTail-(int)qinfo.pcHead) );
            }
        }
    }
}


static void _print_kernel_item_name(const char *name)
{
    int len=strlen(name);

    shell_write_str(name);
    shell_write_char(':');
    while( len++ < 10 )
        shell_write_char(' ');
}


void os_kernel_info_print(void)
{
    _print_kernel_item_name( "OS" );
    shell_printf( "%s %s\n", OS_NAME, tskKERNEL_VERSION_NUMBER );
    _print_kernel_item_name( "SysClock" );
    shell_printf( "%d\n", SystemCoreClock );
    _print_kernel_item_name( "TickRate" );
    shell_printf( "%d\n", OS_TICK_RATE );
    //_print_kernel_item_name( "CurrentNumberOfTasks" );
    //shell_printf( "%d\n", uxTaskGetNumberOfTasks() );
}




#endif
