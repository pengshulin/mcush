/* RTOS Abstract Layer for POSIX OS (eg. linux)
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_POSIX

typedef struct {
    pthread_t *thread;
    char name[16]; 
    int priority;
    int stack_bytes;
} task_reg_t;
#define TASKS_LIMIT  128
task_reg_t tasks_list[TASKS_LIMIT];
int tasks_num;

typedef struct {
    mqd_t *mqdes;
    char name[16]; 
    int queue_length;
    int item_bytes;
} queue_reg_t;
#define QUEUES_LIMIT  128
queue_reg_t queues_list[TASKS_LIMIT];
int queues_num;


struct timespec t0;  /* process base time */


void os_init(void)
{
    clock_gettime( CLOCK_REALTIME, &t0 );
    t0.tv_nsec = 0;  /* ignore nanosec part, for quicker calculation */
    memset( tasks_list, 0, sizeof(tasks_list) );
    tasks_num = 0;
}


void os_start(void)
{
    int i;
    void *ret;

    for( i=0; i<tasks_num; i++ )
    {
        pthread_join( *tasks_list[i].thread, &ret );
    }
}


int os_is_running(void)
{
    return 1;
}


os_tick_t os_tick( void )
{
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    //printf("tv_sec: %ld,  tv_nsec: %ld\n", ts.tv_sec, ts.tv_nsec);
    return (ts.tv_sec - t0.tv_sec)*OS_TICK_RATE + ts.tv_nsec/(1000000000/OS_TICK_RATE);
}


void os_enter_critical(void)
{
}


void os_exit_critical(void)
{
}


void os_disable_interrupts(void)
{
}


/* TASK */

void os_task_delay( os_tick_t ticks )
{
    usleep( ticks*1000000/OS_TICK_RATE );
}


void os_task_delay_ms( unsigned int ms )
{
    usleep( ms*1000 );
}


void os_task_delay_s( unsigned int s )
{
    usleep( s*1000*1000 );
}


void os_task_delay_until( os_tick_t *old_tick, os_tick_t inc_ticks )
{
    /* TODO: calculate the actually nano-time to delay */
    //os_tick_t t = *old_tick + inc_ticks;
    //int s, ms;
    //struct timespec ts;
 
    //s = t / OS_TICK_RATE;
    //ms = (t - s * OS_TICK_RATE) * 1000 / OS_TICK_RATE;
    //clock_gettime( CLOCK_REALTIME, &ts );

    (void)old_tick;
    (void)inc_ticks;
}



os_task_handle_t os_task_create( const char *name, os_task_function_t entry, void *parm, size_t stack_bytes, int priority )
{
    int ret;
    pthread_t *thread;
   
    thread = (pthread_t*)malloc(sizeof(pthread_t));
    if( thread == NULL )
        return NULL;
    ret = pthread_create( thread, NULL, (pthread_function_t)entry, parm );
    if( ret != 0 )
    {
        free( thread );
        return NULL;
    }
    printf("os_task_create %s %p\n", name, thread);
    /* register */
    tasks_list[tasks_num].thread = thread;
    tasks_list[tasks_num].priority = priority;
    tasks_list[tasks_num].stack_bytes = stack_bytes;
    strcpy( tasks_list[tasks_num].name, name );
    tasks_num++;
    return thread;
}


os_task_handle_t os_task_create_static( const char *name, os_task_function_t entry,
         void *parm, size_t stack_bytes, int priority,
         const static_task_buffer_t *buf )
{
    (void)buf;
    return os_task_create( name, entry, parm, stack_bytes, priority );
}


void os_task_delete( os_task_handle_t task )
{
    void *ret=NULL;
    if( task == NULL )
        pthread_exit( ret );
    else
        pthread_cancel( *task );
    free(task);
}


void os_task_suspend( os_task_handle_t task )
{
    (void)task;
}


void os_task_resume( os_task_handle_t task )
{
    (void)task;
}


void os_task_switch( void )
{
    sched_yield();
}


void os_task_priority_set( os_task_handle_t task, int new_priority )
{
    pthread_setschedprio( (pthread_t)task, new_priority );
}


int os_task_priority_get( os_task_handle_t task )
{
    (void)task;
    return 0;
}


/* QUEUE */

os_queue_handle_t os_queue_create( const char *name, size_t queue_length, size_t item_bytes )
{
    mqd_t *mqdes;
    char mqueue[16];
    struct mq_attr attr;

    mqdes= (mqd_t*)malloc(sizeof(mqd_t));
    if( mqdes == NULL )
        return NULL;
    sprintf( mqueue, "/%s", name );
    mq_unlink( mqueue );
    attr.mq_maxmsg = queue_length;
    attr.mq_msgsize = item_bytes;
    *mqdes = mq_open(mqueue, O_RDWR | O_CREAT, 0666, &attr);
    printf("os_queue_create %s, len=%ld, size=%ld, ret=%d\n", mqueue, queue_length, item_bytes, *mqdes);
    if( *mqdes == -1 )
    {
        free( mqdes );
        return NULL;
    }
    /* register */
    queues_list[queues_num].mqdes = mqdes;
    queues_list[queues_num].queue_length = queue_length;
    queues_list[queues_num].item_bytes = item_bytes;
    strcpy( queues_list[queues_num].name, name );
    queues_num++;
    return mqdes;
}


os_queue_handle_t os_queue_create_static( const char *name, size_t queue_length, size_t item_bytes, const static_queue_buffer_t *buf )
{
    (void)buf;
    return os_queue_create( name, queue_length, item_bytes );
}


void os_queue_delete( os_queue_handle_t queue )
{
    (void)queue;
    mq_close( *queue );
}


void os_queue_reset( os_queue_handle_t queue )
{
    (void)queue;
}


static void inc_os_ticks_to_timespec( struct timespec *ts, int os_ticks )
{
    int s, ms = OS_MS_PER_TICK * os_ticks;
        
    s = ms / 1000;
    if( s > 0 )
    {
        ts->tv_sec += s;
        ms -= s*1000;
    }
    ts->tv_nsec += ms * 1000000;
    if( ts->tv_nsec > 1000000000 )
    {
        ts->tv_nsec -= 1000000000;
        ts->tv_sec += 1;
    }
}


int os_queue_put( os_queue_handle_t queue, void *data, int block_ticks )
{
    int ret;
    struct timespec ts;
    struct mq_attr attr;

    mq_getattr( *queue, &attr );
    if( block_ticks < 0 )
    {
        ret = mq_send( (mqd_t)*queue, (const char *)data, attr.mq_msgsize, 0 );
    }
    else
    {
        clock_gettime( CLOCK_REALTIME, &ts );
        inc_os_ticks_to_timespec( &ts, block_ticks );
        ret = mq_timedsend( (mqd_t)*queue, (const char *)data, attr.mq_msgsize, 0, (const struct timespec *)&ts );
    }
    if( ret == -1 )
    {
        //printf("queue_put: %p, ticks=%d, ret=%d, errno=%d\n", data, block_ticks, ret, errno );
        return 0;
    }
    //printf("queue_put: %p, 0x%X, ticks=%d, ret=%d\n", data, *(char*)data, block_ticks, ret );
    return 1;
}


int os_queue_get( os_queue_handle_t queue, void *data, int block_ticks )
{
    int ret;
    struct timespec ts;
    char buf[128];
    struct mq_attr attr;

    mq_getattr( *queue, &attr );
    if( block_ticks < 0 )
    {
        ret = mq_receive( (mqd_t)*queue, (char *)buf, 128, NULL );
    }
    else
    {
        clock_gettime( CLOCK_REALTIME, &ts );
        inc_os_ticks_to_timespec( &ts, block_ticks );
        ret = mq_timedreceive( (mqd_t)*queue, (char *)buf, 128, NULL, (const struct timespec *)&ts );
    }
    if( ret == -1 )
    {
        //printf("queue_get: %p, ticks=%d, ret=%d, errno=%d\n", data, block_ticks, ret, errno );
        return 0;
    }
    memcpy( (void*)data, (void*)buf, attr.mq_msgsize ); 
    //printf("queue_get: %p, 0x%X, ticks=%d, ret=%d\n", data, *(char*)data, block_ticks, ret );
    return 1; 
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
    struct mq_attr attr;

    mq_getattr( *queue, &attr );
    return attr.mq_curmsgs;
}


int os_queue_count_isr( os_queue_handle_t queue )
{
    return os_queue_count( queue );
}


int os_queue_is_empty( os_queue_handle_t queue )
{
    struct mq_attr attr;

    mq_getattr( *queue, &attr );
    return (attr.mq_curmsgs == 0) ? 1 : 0;
}


int os_queue_is_empty_isr( os_queue_handle_t queue )
{
    return os_queue_is_empty( queue );
}


int os_queue_is_full( os_queue_handle_t queue )
{
    struct mq_attr attr;

    mq_getattr( *queue, &attr );
    return (attr.mq_curmsgs == attr.mq_maxmsg) ? 1 : 0;
}


int os_queue_is_full_isr( os_queue_handle_t queue )
{
    return os_queue_is_full( queue );
}



/* MUTEX */

os_mutex_handle_t os_mutex_create( void )
{
    pthread_mutex_t *mutex;
    
    mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if( mutex == NULL )
        return NULL;
    if( pthread_mutex_init( mutex, NULL ) != 0 )
    {
        free( mutex );
        return NULL;
    }
    printf("os_mutex_create %p\n", mutex);
    return mutex;
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    (void)buf;
    return os_mutex_create();
}


void os_mutex_delete( os_mutex_handle_t mutex )
{
    pthread_mutex_destroy( mutex );
    free(mutex);
}


int os_mutex_put( os_mutex_handle_t mutex )
{
    pthread_mutex_unlock( mutex );
    return 1;
}


int os_mutex_put_isr( os_mutex_handle_t mutex )
{
    pthread_mutex_unlock( mutex );
    return 1;
}


int os_mutex_get( os_mutex_handle_t mutex, int block_ticks )
{
    (void)block_ticks;
    pthread_mutex_lock( mutex );
    return 1;
}


int os_mutex_get_isr( os_mutex_handle_t mutex )
{
    pthread_mutex_lock( mutex );
    return 1;
}


/* SEMAPHORE */

os_semaphore_handle_t os_semaphore_create( int max_count, int init_count )
{
    (void)max_count;
    (void)init_count;
    return NULL;
    //return xSemaphoreCreateCounting( max_count, init_count );
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
//#if OS_SUPPORT_STATIC_ALLOCATION
//    return xSemaphoreCreateCountingStatic( max_count, init_count, (StaticSemaphore_t*)buf );
//#else
//    return NULL;
//#endif
    (void)max_count;
    (void)init_count;
    (void)buf;
    return NULL;
}


void os_semaphore_delete( os_semaphore_handle_t semaphore )
{
    sem_destroy( semaphore );
}


int os_semaphore_put( os_semaphore_handle_t semaphore )
{
//    if( xSemaphoreGive( semaphore ) == pdFAIL )
//        return 0;
//    else
//        return 1;
    (void)semaphore;
    return 0;
}


int os_semaphore_put_isr( os_semaphore_handle_t semaphore )
{
//    if( xSemaphoreGiveFromISR( semaphore, NULL ) == pdTRUE )
//        return 1;
//    else
//        return 0;
    (void)semaphore;
    return 0;
}


int os_semaphore_get( os_semaphore_handle_t semaphore, int block_ticks )
{
//    if( block_ticks < 0 )
//        block_ticks = portMAX_DELAY;
//    if( xSemaphoreTake( semaphore, block_ticks ) == pdFAIL )
//        return 0;
//    else
//        return 1;
    (void)semaphore;
    (void)block_ticks;
    return 0;
}


int os_semaphore_get_isr( os_semaphore_handle_t semaphore )
{
//    if( xSemaphoreTakeFromISR( semaphore, NULL ) == pdFAIL )
//        return 0;
//    else
//        return 1;
    (void)semaphore;
    return 0;
}


/* TIMER */

os_timer_handle_t os_timer_create( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback )
{
    (void)name;
    (void)period_ticks;
    (void)repeat_mode;
    (void)callback;
    //return xTimerCreate( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback );
    return NULL;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf )
{
    (void)name;
    (void)period_ticks;
    (void)repeat_mode;
    (void)callback;
    (void)buf;
    return NULL;
//#if OS_SUPPORT_STATIC_ALLOCATION
//    return xTimerCreateStatic( name, period_ticks, repeat_mode, 0, (TimerCallbackFunction_t)callback, (StaticTimer_t *)buf );
//#else
//    return NULL;
//#endif
}


int os_timer_start( os_timer_handle_t timer )
{
    //if( xTimerStart( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    (void)timer;
    return 0;
}


int os_timer_stop( os_timer_handle_t timer )
{
    //if( xTimerStop( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    (void)timer;
    return 0;
}


int os_timer_reset( os_timer_handle_t timer )
{
    //if( xTimerReset( timer, 0 ) == pdPASS )
    //    return 1;
    //else
    //    return 0;
    (void)timer;
    return 0;
}


void os_timer_delete( os_timer_handle_t timer )
{
    //xTimerDelete( timer, 0 );
    (void)timer;
}


/* MEMORY */

void *os_malloc( size_t bytes )
{
    return malloc( bytes );
}


void *os_realloc( void *old_mem, size_t bytes )
{
    return realloc( old_mem, bytes );
}


void os_free( void *mem )
{
    free( mem );
}




void os_task_info_print(void)
{
    int i;

    for( i=0; i<tasks_num; i++ )
    {
        shell_printf( "%2d %8s %c %p %d %d\n",
                    i+1,
                    tasks_list[i].name, '?',
                    tasks_list[i].thread,
                    tasks_list[i].priority,
                    tasks_list[i].stack_bytes );
    }
}


void os_queue_info_print(void)
{
    int i;

    for( i=0; i<queues_num; i++ )
    {
        shell_printf( "%8s %p %4d %4d\n",
                    queues_list[i].name,
                    queues_list[i].mqdes,
                    queues_list[i].queue_length,
                    queues_list[i].item_bytes );
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
    shell_printf( "%s\n", "POSIX" );
    _print_kernel_item_name( "SysClock" );
    shell_printf( "%d\n", CLOCKS_PER_SEC );
    _print_kernel_item_name( "TickRate" );
    shell_printf( "%d\n", OS_TICK_RATE );
}


void _halt(void)
{
    printf("HALT\n");
    exit(1);
}


void _halt_with_message(const char *message)
{
    printf("HALT: %s\n", message);
    exit(1);
}

#endif
