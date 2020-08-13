/* RTOS Abstract Layer for RTThread
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_RTTHREAD



void os_init(void)
{
    rt_system_scheduler_init();
}


void os_start(void)
{
    rt_system_scheduler_start();
}


int os_is_running(void)
{
    //if( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
        return 1;
    //else
    //    return 0;
}


os_tick_t os_tick( void )
{
    return rt_tick_get();
}


void os_enter_critical(void)
{
    rt_enter_critical();
}


void os_exit_critical(void)
{
    rt_exit_critical();
}


void os_disable_interrupts(void)
{
    rt_hw_interrupt_disable();
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    rt_thread_delay( ticks );
}


void os_task_delay_ms( unsigned int ms )
{
    rt_thread_delay( OS_TICKS_MS(ms) );
}


void os_task_delay_s( unsigned int s )
{
    rt_thread_delay( OS_TICKS_S(s) );
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    rt_thread_delay_until( old_tick, inc_ticks );
}



os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority )
{
    rt_thread_t task;

    task = rt_thread_create( name, entry, parm, stack_bytes, priority, 10 );
    if( task != RT_NULL )
    {
        rt_thread_startup( task );
        return task;
    }
    else
        return NULL;
}


os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
    rt_err_t err;

    err = rt_thread_init( buf->control, name, entry, parm, buf->stack, stack_bytes, priority, 10 );
    if( err == RT_EOK )
    {
        rt_thread_startup( buf->control );
        return (os_task_handle_t)buf->control;
    }
    else
        return NULL;
}


void os_task_delete( os_task_handle_t task )
{
    rt_thread_delete( task );
}


void os_task_suspend( os_task_handle_t task )
{
    rt_thread_suspend( task );
}


void os_task_resume( os_task_handle_t task )
{
    rt_thread_resume( task );
}


void os_task_switch( void )
{
    rt_thread_yield();
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    rt_uint8_t priority = new_priority;

    rt_thread_control( task, RT_THREAD_CTRL_CHANGE_PRIORITY, (void*)&priority ); 
}


int os_task_priority_get( os_task_handle_t task )
{
    return task->current_priority;
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    rt_mq_t queue;

    queue = rt_mq_create( name, item_bytes, queue_length, 0 );
    if( queue != RT_NULL )
        return queue;
    else
        return NULL;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    rt_err_t err;

    err = rt_mq_init( buf->control, name, buf->data, item_bytes, queue_length, 0 );
    if( err == RT_EOK )
        return (os_queue_handle_t)buf->control;
    else
        return NULL;
}


void os_queue_delete( os_queue_handle_t queue )
{
    rt_mq_delete( queue );
}


void os_queue_reset( os_queue_handle_t queue )
{
    rt_mq_control( queue, RT_IPC_CMD_RESET, 0 );
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = RT_WAITING_FOREVER;
    if( rt_mq_send_wait( queue, data, queue->msg_size, block_ticks ) != RT_EOK )
        return 0;
    else
        return 1;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    if( block_ticks < 0 )
        block_ticks = RT_WAITING_FOREVER;
    if( rt_mq_recv( queue, data, queue->msg_size,block_ticks ) != RT_EOK )
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
    (void)queue;
    return 0;  // FIXME
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
    return (os_mutex_handle_t)rt_mutex_create( NULL, 0 );
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    rt_err_t err;

    err = rt_mutex_init( buf, NULL, 0 );
    if( err == RT_EOK )
        return (os_mutex_handle_t)buf;    
    else
        return NULL;
}


void os_mutex_delete( os_mutex_handle_t mutex )
{
    rt_mutex_delete( mutex );
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    rt_err_t err;

    err = rt_mutex_release( mutex );
    if( err == RT_EOK )
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
    rt_err_t err;

    if( block_ticks < 0 )
        block_ticks = RT_WAITING_FOREVER;
    err = rt_mutex_take( mutex, block_ticks );
    if( err == RT_EOK )
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
    os_semaphore_handle_t sem;

    (void)init_count;  // FIXME
    sem = rt_sem_create( NULL, max_count, 0 );
    if( sem != RT_NULL )
        return sem;
    else
        return NULL;
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
    rt_err_t err;

    (void)init_count;  // FIXME
    err = rt_sem_init( (rt_sem_t)buf, NULL, max_count, 0 );
    if( err == RT_EOK )
        return (os_semaphore_handle_t)buf;
    else
        return NULL;
}


void os_semaphore_delete( os_semaphore_handle_t semaphore )
{
    rt_sem_delete( semaphore );
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
    rt_err_t err;

    err = rt_sem_release( semaphore );
    if( err == RT_EOK )
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
    rt_err_t err;

    if( block_ticks < 0 )
        block_ticks = RT_WAITING_FOREVER;
 
    err = rt_sem_take( semaphore, block_ticks );
    if( err == RT_EOK )
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
    rt_timer_t timer;

    (void)repeat_mode;  // FIXME
    timer = rt_timer_create( name, callback, NULL, period_ticks, 0 );
    if( timer != RT_NULL )
        return timer;
    else
        return NULL;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buffer )
{
    (void)repeat_mode;  // FIXME
    rt_timer_init( buffer, name, callback, NULL, period_ticks, 0 );
    return buffer;
}


int os_timer_start( os_timer_handle_t timer )
{
    if( rt_timer_start( timer ) == RT_EOK )
        return 1;
    else
        return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    if( rt_timer_stop( timer ) == RT_EOK )
        return 1;
    else
        return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    rt_tick_t tick=0;

    if( rt_timer_control( timer, RT_TIMER_CTRL_SET_TIME, &tick ) == RT_EOK )
        return 1;
    else
        return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    rt_timer_delete( timer );
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    return rt_malloc( bytes );
}


void *os_realloc( void *old_mem, size_t bytes )
{
    return rt_realloc( old_mem, bytes );
}


void os_free( void *mem )
{
    rt_free( mem );
}



#define TASKS_LIMIT  30
void os_task_info_print(void)
{
//    /* sizeof(TaskStatus_t) = 36 bytes */
//    TaskStatus_t task_status_array[TASKS_LIMIT];  /* 30*36=1080 bytes */
//    int i, count;
//    char c;
//
//    count = uxTaskGetSystemState( task_status_array, TASKS_LIMIT, NULL );
//    for( i=0; i<count; i++ )
//    {
//        /* status */
//        switch( task_status_array[i].eCurrentState )
//        {
//        case eRunning: c = 'X'; break;
//        case eReady: c = 'R'; break;
//        case eBlocked: c = 'B'; break;
//        case eSuspended: c = 'S'; break;
//        case eDeleted: c = 'D'; break;
//        default: c = '?'; break;
//        } 
//        
//        shell_printf( "%2d %8s %c 0x%08X %d/%d 0x%08X 0x%08X (free %d)\n",
//                    task_status_array[i].xTaskNumber,
//                    task_status_array[i].pcTaskName, c, 
//                    task_status_array[i].xHandle,
//                    task_status_array[i].uxCurrentPriority, task_status_array[i].uxBasePriority, 
//                    task_status_array[i].pxStackBase,
//                    *(uint32_t*)task_status_array[i].xHandle,  /* pxTopOfStack is at the begining of TCB_t */
//                    //(uint32_t*)mcushGetTaskStackTop( task_status_array[i].xHandle ),
//                    task_status_array[i].usStackHighWaterMark * sizeof(portSTACK_TYPE) );
//    } 
}


void os_queue_info_print(void)
{
}


static void _print_kernel_item_name(const char *name)
{
    int len=strlen(name);

    shell_write_str(name);
    shell_write_char(':');
    while( len++ < 21 )
        shell_write_char(' ');
}


void os_kernel_info_print(void)
{
    _print_kernel_item_name( "OS" );
    shell_printf( "%s %d.%d.%d\n", OS_NAME, RT_VERSION, RT_SUBVERSION, RT_REVISION );
    _print_kernel_item_name( "SystemCoreClock" );
    shell_printf( "%d\n", SystemCoreClock );
    _print_kernel_item_name( "TickRate" );
    shell_printf( "%d\n", OS_TICK_RATE );
}




#endif
