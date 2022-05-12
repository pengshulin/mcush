/* RTOS Abstract Layer for ThreadX
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_THREADX
#include "tx_initialize.h"

static TX_INTERRUPT_SAVE_AREA


void os_init(void)
{
    _tx_initialize_kernel_setup();
}


void os_start(void)
{
    tx_kernel_enter();
}


int os_is_running(void)
{
    if( _tx_thread_system_state == TX_INITIALIZE_IS_FINISHED )
        return 1;
    else
        return 0;
}


os_tick_t os_tick( void )
{
    return tx_time_get();
}


void os_enter_critical(void)
{
    TX_DISABLE
}


void os_exit_critical(void)
{
    TX_RESTORE
}


void os_disable_interrupts(void)
{
    __disable_irq();
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    tx_thread_sleep(ticks);
}


void os_task_delay_ms( unsigned int ms )
{
    tx_thread_sleep( OS_TICKS_MS(ms) );
}


void os_task_delay_s( unsigned int s )
{
    tx_thread_sleep( OS_TICKS_S(s) );
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    os_tick_t dt = *old_tick + inc_ticks - tx_time_get();

    if( (LONG)dt <= 0 )
        return;
    tx_thread_sleep(dt);
    *old_tick += inc_ticks;
}



os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority )
{
    size_t recalc_stack_bytes = sizeof(long)*((stack_bytes+sizeof(long)-1)/sizeof(long));
    void *mem;
    os_task_handle_t task;
    static_task_buffer_t buf;

    mem = os_malloc( recalc_stack_bytes + sizeof(TX_THREAD));
    if( mem == NULL )
        return NULL;
    buf.stack = mem;
    buf.control = mem + sizeof(TX_THREAD);
    task = os_task_create_static( name, entry, parm, recalc_stack_bytes, priority,
                                  (const static_task_buffer_t *)&buf );
    if( task == NULL )
    {
        os_free( mem );
        return NULL;
    }
    task->tx_thread_need_free = mem; 
    return task;
}


os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
    UINT status;

    status = tx_thread_create( buf->control, (CHAR *)name, (_os_task_entry_t)entry, (ULONG)parm,
                               buf->stack, sizeof(long)*((stack_bytes+sizeof(long)-1)/sizeof(long)),
                               priority, priority, 1, TX_AUTO_START );
    if( status == TX_SUCCESS )
        return buf->control;
    return NULL;
}


void os_task_delete( os_task_handle_t task )
{
    void *mem = task->tx_thread_need_free;

    tx_thread_delete( task );
    if( mem != NULL )
        os_free( mem );
}


void os_task_suspend( os_task_handle_t task )
{
    tx_thread_suspend( task );
}


void os_task_resume( os_task_handle_t task )
{
    tx_thread_resume( task );
}


void os_task_switch( void )
{
    tx_thread_relinquish();
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    UINT old;

    if( task == NULL )
        task = _tx_thread_current_ptr;
    tx_thread_priority_change( task, (UINT)new_priority, &old );
}


int os_task_priority_get( os_task_handle_t task )
{
    if( task == NULL )
        task = _tx_thread_current_ptr;
    return task->tx_thread_user_priority;
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    size_t recalc_queue_bytes = (ULONG)sizeof(ULONG)*queue_length*((item_bytes+sizeof(ULONG)-1)/sizeof(ULONG));
    void *mem;
    os_queue_handle_t queue;
    static_queue_buffer_t buf;

    mem = os_malloc( recalc_queue_bytes + sizeof(TX_QUEUE));
    if( mem == NULL )
        return NULL;
    buf.data = mem;
    buf.control = mem + sizeof(TX_QUEUE);
    queue = os_queue_create_static( name, recalc_queue_bytes, item_bytes, (const static_queue_buffer_t *)&buf );
    if( queue == NULL )
    {
        os_free( mem );
        return NULL;
    }
    queue->tx_queue_need_free = mem; 
    return queue;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    UINT status;

    status = tx_queue_create( buf->control, (CHAR *)name, (UINT)(item_bytes+sizeof(ULONG)-1)/sizeof(ULONG), 
                              (VOID*)buf->data, (ULONG)sizeof(ULONG)*queue_length*((item_bytes+sizeof(ULONG)-1)/sizeof(ULONG)) );
    if( status == TX_SUCCESS )
    {
        buf->control->tx_queue_message_size_in_bytes = item_bytes;
        if( item_bytes < 4 )
            buf->control->tx_queue_message_size_align_convert = item_bytes;
        return buf->control;
    }
    return NULL;
}


void os_queue_delete( os_queue_handle_t queue )
{
    void *mem = queue->tx_queue_need_free;

    tx_queue_delete( queue );
    if( mem != NULL )
        os_free( mem );

}


void os_queue_reset( os_queue_handle_t queue )
{
    (void)queue;
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    ULONG wait = (block_ticks < 0) ? TX_WAIT_FOREVER : block_ticks;
    ULONG _data;

    if( queue->tx_queue_message_size_align_convert )
    {
        memcpy( (void*)&_data, (const void*)data, queue->tx_queue_message_size_align_convert );
        return (tx_queue_send( queue, &_data, wait ) == TX_SUCCESS) ? 1 : 0;
    }
    else
        return (tx_queue_send( queue, data, wait ) == TX_SUCCESS) ? 1 : 0;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    ULONG wait = (block_ticks < 0) ? TX_WAIT_FOREVER : block_ticks;
    ULONG _data;
 
    if( queue->tx_queue_message_size_align_convert )
    {
        if( (tx_queue_receive( queue, &_data, wait ) == TX_SUCCESS) )
        {
            memcpy( data, (const void*)&_data, queue->tx_queue_message_size_align_convert );
            return 1;
        }
        else
            return 0;
    }
    else
        return (tx_queue_receive( queue, data, wait ) == TX_SUCCESS) ? 1 : 0;
}


int os_queue_peek( os_queue_handle_t queue, void *data, int block_ticks )
{
    (void)queue;
    (void)data;
    (void)block_ticks;
    return 0;
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
    ULONG ret;

    if( tx_queue_info_get( queue, NULL, &ret, NULL, NULL, NULL, NULL ) == TX_SUCCESS )
        return ret; 
    return -1;
}


int os_queue_count_isr( os_queue_handle_t queue )
{
    return os_queue_count(queue);
}


int os_queue_is_empty( os_queue_handle_t queue )
{
    return (os_queue_count(queue)==0) ? 1 : 0;
}


int os_queue_is_empty_isr( os_queue_handle_t queue )
{
    return os_queue_is_empty( queue );
}


int os_queue_is_full( os_queue_handle_t queue )
{
    ULONG ret;

    tx_queue_info_get( queue, NULL, NULL, &ret, NULL, NULL, NULL );
    return (ret==0) ? 1 : 0;
}


int os_queue_is_full_isr( os_queue_handle_t queue )
{
    return os_queue_is_full( queue );
}



/* MUTEX */

os_mutex_handle_t os_mutex_create( void )
{
    void *mem;
    static_mutex_buffer_t *buf;
    os_mutex_handle_t mutex;

    mem = os_malloc( sizeof(TX_MUTEX) );
    if( mem == NULL )
        return NULL;
    buf = (static_mutex_buffer_t*)mem;
    mutex = os_mutex_create_static( buf );
    if( mutex == NULL )
    {
        os_free( mem );
        return NULL;
    }
    mutex->tx_mutex_need_free = mem; 
    return mutex;
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    UINT status;

    status = tx_mutex_create( (TX_MUTEX*)buf, NULL, TX_INHERIT );
    if( status == TX_SUCCESS )
        return (TX_MUTEX*)buf;
    return NULL;
}


void os_mutex_delete( os_mutex_handle_t mutex )
{
    void *mem = mutex->tx_mutex_need_free;

    tx_mutex_delete( mutex );
    if( mem != NULL )
        os_free( mem );
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    return (tx_mutex_put( mutex ) == TX_SUCCESS) ? 1 : 0;
}


int os_mutex_put_isr( os_mutex_handle_t mutex )
{
    return os_mutex_put( mutex );
}


int os_mutex_get( os_mutex_handle_t mutex, int block_ticks )
{
    ULONG wait = (block_ticks < 0) ? TX_WAIT_FOREVER : block_ticks;
    
    return (tx_mutex_get( mutex, wait ) == TX_SUCCESS) ? 1 : 0;
}


int os_mutex_get_isr( os_mutex_handle_t mutex )
{
    return (tx_mutex_get( mutex, TX_NO_WAIT ) == TX_SUCCESS) ? 1 : 0;
}


os_task_handle_t os_mutex_holder( os_mutex_handle_t mutex )
{
    os_task_handle_t task;

    if( tx_mutex_info_get( mutex, NULL, 0, &task, NULL, NULL, NULL ) == TX_SUCCESS )
        return task;
    return NULL;
}


/* SEMAPHORE */

os_semaphore_handle_t os_semaphore_create( int max_count, int init_count )
{
    void *mem;
    static_semaphore_buffer_t *buf;
    os_semaphore_handle_t semaphore;

    mem = os_malloc( sizeof(TX_SEMAPHORE) );
    if( mem == NULL )
        return NULL;
    buf = (static_semaphore_buffer_t*)mem;
    semaphore = os_semaphore_create_static( max_count, init_count, buf );
    if( semaphore == NULL )
    {
        os_free( mem );
        return NULL;
    }
    semaphore->tx_semaphore_need_free = mem; 
    return semaphore;
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
    UINT status;
    
    (void)max_count;
    status = tx_semaphore_create( (TX_SEMAPHORE*)buf, NULL, init_count );
    if( status == TX_SUCCESS )
        return (TX_SEMAPHORE*)buf;
    return NULL;
}


void os_semaphore_delete( os_semaphore_handle_t semaphore )
{
    void *mem = semaphore->tx_semaphore_need_free;

    tx_semaphore_delete( semaphore );
    if( mem != NULL )
        os_free( mem );
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
    return (tx_semaphore_put( semaphore ) == TX_SUCCESS) ? 1 : 0;
}


int os_semaphore_put_isr( os_semaphore_handle_t semaphore )
{
    return (tx_semaphore_put( semaphore ) == TX_SUCCESS) ? 1 : 0;
}


int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks )
{
    ULONG wait = (block_ticks < 0) ? TX_WAIT_FOREVER : block_ticks;

    return (tx_semaphore_get( semaphore, wait ) == TX_SUCCESS) ? 1 : 0;
}


int os_semaphore_get_isr( os_semaphore_handle_t semaphore )
{
    return (tx_semaphore_get( semaphore, TX_NO_WAIT ) == TX_SUCCESS) ? 1 : 0;
}


int os_semaphore_count( os_semaphore_handle_t semaphore )
{
    ULONG ret;

    if( tx_semaphore_info_get( semaphore, NULL, &ret, NULL, NULL, NULL ) == TX_SUCCESS )
        return ret;
    return -1;
}


/* TIMER */

os_timer_handle_t os_timer_create( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback )
{
    void *mem;
    static_timer_buffer_t *buf;
    os_timer_handle_t timer;

    mem = os_malloc( sizeof(TX_TIMER) );
    if( mem == NULL )
        return NULL;
    buf = (static_timer_buffer_t*)mem;
    timer = os_timer_create_static( name, period_ticks, repeat_mode, callback, buf );
    if( timer == NULL )
    {
        os_free( mem );
        return NULL;
    }
    timer->tx_timer_need_free = mem; 
    return timer;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf )
{
    UINT ret;

    ret = tx_timer_create( (TX_TIMER *)buf, (CHAR *)name, (_os_timer_callback_t)callback,
                           (ULONG)buf, period_ticks, repeat_mode ? period_ticks : 0, TX_NO_ACTIVATE );
    if( ret == TX_SUCCESS )
        return buf;
    return NULL;
}


int os_timer_start( os_timer_handle_t timer )
{
    return (tx_timer_activate( timer ) == TX_SUCCESS) ? 1 : 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    return (tx_timer_deactivate( timer ) == TX_SUCCESS) ? 1 : 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    (void)timer; 
    return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    void *mem = timer->tx_timer_need_free;

    tx_timer_delete( timer );
    if( mem != NULL )
        os_free( mem );
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    void *ret;

    TX_DISABLE
    ret = malloc( bytes );
    TX_RESTORE
    return ret;
}


void *os_calloc( size_t nmemb, size_t size )
{
    void *ret;

    TX_DISABLE
    ret = calloc( nmemb, size );
    TX_RESTORE
    return ret;

}



void *os_realloc( void *old_mem, size_t bytes )
{
    void *ret;

    TX_DISABLE
    ret = realloc( old_mem, bytes );
    TX_RESTORE
    return ret;
}


void os_free( void *mem )
{
    TX_DISABLE
    free( mem );
    TX_RESTORE
}


void os_task_info_print(void)
{
    TX_THREAD *thread = _tx_thread_created_ptr;
    char c;
    int cnt=1;

    while( thread != NULL )
    {
        switch(thread->tx_thread_state)
        {
        case TX_READY: c=((thread==_tx_thread_current_ptr)?'X':'R'); break;
        case TX_QUEUE_SUSP:
        case TX_MUTEX_SUSP:
        case TX_SEMAPHORE_SUSP: c='B'; break;
        case TX_SUSPENDED: c='S'; break;
        case TX_SLEEP: c='s'; break;
        case TX_COMPLETED: c='C'; break;
        case TX_TERMINATED: c='T'; break;
        default: c='?'; break;
        }

        shell_printf( "%2d %8s %c %p %2d/%2d %p %p (free %d)\n",
                    cnt++,
                    thread->tx_thread_name, c, 
                    thread,
                    thread->tx_thread_priority, thread->tx_thread_user_priority, 
                    thread->tx_thread_stack_start,
                    thread->tx_thread_stack_ptr,
                    thread->tx_thread_stack_highest_ptr - thread->tx_thread_stack_start );

        thread = thread->tx_thread_created_next;
        if( thread == _tx_thread_created_ptr )
            break;
    }
}


void os_queue_info_print(void)
{
    TX_QUEUE *queue = _tx_queue_created_ptr;
 
    while( queue != NULL )
    {
        shell_printf( "%8s %p  %5d %4d %4d  %p - %p (%p)\n", 
            queue->tx_queue_name, queue, 
            queue->tx_queue_capacity,
            queue->tx_queue_message_size_in_bytes, //queue->tx_queue_message_size, 
            queue->tx_queue_enqueued, 
            queue->tx_queue_start, queue->tx_queue_end, queue->tx_queue_read );

        queue = queue->tx_queue_created_next;
        if( queue == _tx_queue_created_ptr )
            break;
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
    shell_printf( "%s %d.%d.%d\n", OS_NAME, THREADX_MAJOR_VERSION, THREADX_MINOR_VERSION, THREADX_PATCH_VERSION );
    _print_kernel_item_name( "SysClock" );
    shell_printf( "%d\n", SystemCoreClock );
    _print_kernel_item_name( "TickRate" );
    shell_printf( "%d\n", OS_TICK_RATE );
}


extern char _sheap, _eheap, _sstack, _estack;

VOID tx_application_define(VOID *first_unused_memory)
{
    (void)first_unused_memory;
    //TX_BYTE_POOL pool;
    //size_t siz = (uintptr_t)(&_eheap) - (uintptr_t)(&_sheap);
    //tx_byte_pool_create( &pool, NULL, first_unused_memory, siz );
}



#endif
