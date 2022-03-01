/* RTOS Abstract Layer for RTThread
 * only most frequently used APIs are ported, including
 * task/queue/mutex/semaphore/timer/memory
 */ 
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if MCUSH_OS == OS_RTTHREAD

#ifndef THREAD_SLICE_TICKS
    #define THREAD_SLICE_TICKS  1
#endif

extern char _sheap, _sstack;
extern uint32_t SystemCoreClock;

void os_init(void)
{
    rt_system_heap_init( (void*)&_sheap, (void*)&_sstack-1 );
    rt_system_scheduler_init();
}


void os_start(void)
{
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );
    rt_system_timer_init();
    rt_system_timer_thread_init();
    rt_thread_idle_init();
    rt_system_scheduler_start();
}


int os_is_running(void)
{
    if( rt_thread_self() != NULL )
        return 1;
    else
        return 0;
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

    task = rt_thread_create( name, entry, parm, stack_bytes, priority, THREAD_SLICE_TICKS );
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

    err = rt_thread_init( buf->control, name, entry, parm, buf->stack, stack_bytes, priority, THREAD_SLICE_TICKS );
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
    if( task == NULL )
        task = rt_thread_self();
    rt_thread_delete( task );
}


void os_task_suspend( os_task_handle_t task )
{
    if( task == NULL )
        task = rt_thread_self();
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

    if( task == NULL )
        task = rt_thread_self(); 
    rt_thread_control( task, RT_THREAD_CTRL_CHANGE_PRIORITY, (void*)&priority ); 
}


int os_task_priority_get( os_task_handle_t task )
{
    if( task == NULL )
        task = rt_thread_self(); 
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

    err = rt_mq_init( buf->control, name, buf->data, item_bytes, queue_length*(RT_ALIGN(item_bytes,RT_ALIGN_SIZE)+RT_ALIGN_SIZE), 0 );
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
    if( rt_mq_recv( queue, data, queue->msg_size, block_ticks ) != RT_EOK )
        return 0;
    else
        return 1;
}


int os_queue_put_isr( os_queue_handle_t queue, void *data )
{
    int ret;
    rt_interrupt_enter();
    ret = os_queue_put( queue, data, 0 );
    rt_interrupt_leave();
    return ret;
}


int os_queue_get_isr( os_queue_handle_t queue, void *data )
{
    int ret;
    rt_interrupt_enter();
    ret = os_queue_get( queue, data, 0 );
    rt_interrupt_leave();
    return ret;
}


//int os_queue_count( os_queue_handle_t queue )
//{
//    (void)queue;
//    return 0;
//}
//
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
    return (os_mutex_handle_t)rt_mutex_create( shell_str_null, 0 );
}


os_mutex_handle_t os_mutex_create_static( static_mutex_buffer_t *buf )
{
    rt_err_t err;

    err = rt_mutex_init( buf, shell_str_null, 0 );
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
    sem = rt_sem_create( shell_str_null, max_count, 0 );
    if( sem != RT_NULL )
        return sem;
    else
        return NULL;
}


os_semaphore_handle_t os_semaphore_create_static( int max_count, int init_count, static_semaphore_buffer_t *buf )
{
    rt_err_t err;

    (void)init_count;  // FIXME
    err = rt_sem_init( (rt_sem_t)buf, shell_str_null, max_count, 0 );
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
    timer = rt_timer_create( name, (void*)callback, NULL, period_ticks, 0 );
    if( timer != RT_NULL )
    {
        timer->parameter = (void*)timer;
        return timer;
    }
    else
        return NULL;
}


os_timer_handle_t os_timer_create_static( const char *name, int period_ticks, int repeat_mode, os_timer_callback_t callback, static_timer_buffer_t *buf )
{
    (void)repeat_mode;  // FIXME
    rt_timer_init( buf, name, (void*)callback, NULL, period_ticks, 0 );
    buf->parameter = (void*)buf;
    return buf;
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


/*************************************************************************************/
/* list operation codes borrowed from components/finsh/cmd.c in RT-Thread repository */
/*************************************************************************************/

#define LIST_FIND_OBJ_NR  8

typedef struct
{
    rt_list_t *list;
    rt_list_t **array;
    rt_uint8_t type;
    int nr;             /* input: max nr, can't be 0 */
    int nr_out;         /* out: got nr */
} list_get_next_t;

static void list_find_init(list_get_next_t *p, rt_uint8_t type, rt_list_t **array, int nr)
{
    struct rt_object_information *info;
    rt_list_t *list;

    info = rt_object_get_information((enum rt_object_class_type)type);
    list = &info->object_list;

    p->list = list;
    p->type = type;
    p->array = array;
    p->nr = nr;
    p->nr_out = 0;
}

static rt_list_t *list_get_next(rt_list_t *current, list_get_next_t *arg)
{
    int first_flag = 0;
    rt_ubase_t level;
    rt_list_t *node, *list;
    rt_list_t **array;
    int nr;

    arg->nr_out = 0;

    if (!arg->nr || !arg->type)
    {
        return (rt_list_t *)RT_NULL;
    }

    list = arg->list;

    if (!current) /* find first */
    {
        node = list;
        first_flag = 1;
    }
    else
    {
        node = current;
    }

    level = rt_hw_interrupt_disable();

    if (!first_flag)
    {
        struct rt_object *obj;
        /* The node in the list? */
        obj = rt_list_entry(node, struct rt_object, list);
        if ((obj->type & ~RT_Object_Class_Static) != arg->type)
        {
            rt_hw_interrupt_enable(level);
            return (rt_list_t *)RT_NULL;
        }
    }

    nr = 0;
    array = arg->array;
    while (1)
    {
        node = node->next;

        if (node == list)
        {
            node = (rt_list_t *)RT_NULL;
            break;
        }
        nr++;
        *array++ = node;
        if (nr == arg->nr)
        {
            break;
        }
    }
    
    rt_hw_interrupt_enable(level);
    arg->nr_out = nr;
    return node;
}


static int check_free_bytes(int *p)
{
    int bytes=0;

    while( *p==0x23232323 )  /* filled with "####" */
    {
        bytes += 4;
#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
        p--;
#else
        p++;
#endif
    }
    return bytes;
}


void os_task_info_print(void)
{
    rt_ubase_t level;
    list_get_next_t find_arg;
    rt_list_t *obj_list[LIST_FIND_OBJ_NR];
    rt_list_t *next = (rt_list_t*)RT_NULL;
    int i;
    char c;
    struct rt_object *obj;
    struct rt_thread thread_info, *thread;
    int free_bytes;
    char name[9];

    list_find_init(&find_arg, RT_Object_Class_Thread, obj_list, sizeof(obj_list)/sizeof(obj_list[0]));

    do
    {
        next = list_get_next(next, &find_arg);
        for( i = 0; i < find_arg.nr_out; i++ )
        {
            obj = rt_list_entry(obj_list[i], struct rt_object, list);
            level = rt_hw_interrupt_disable();

            if ((obj->type & ~RT_Object_Class_Static) != find_arg.type)
            {
                rt_hw_interrupt_enable(level);
                continue;
            }
            /* copy info */
            memcpy(&thread_info, obj, sizeof thread_info);
            rt_hw_interrupt_enable(level);

            thread = (struct rt_thread*)obj;
            
            /* NOTE:
             * this method is not the real-time snapshot of all tasks status, 
             * tasks are still dynamically changing,
             * more than one tasks may be displayed as "Running" status
             */
            switch( thread->stat & RT_THREAD_STAT_MASK )
            {
            case RT_THREAD_READY: c = 'R'; break;
            case RT_THREAD_SUSPEND: c = 'S'; break;
            case RT_THREAD_INIT: c = 'I'; break;
            case RT_THREAD_CLOSE: c = 'D'; break;
            case RT_THREAD_RUNNING: c = 'X'; break;
            default: c = '?'; break;
            } 

#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
            free_bytes = check_free_bytes((int*)(thread->stack_addr+thread->stack_size-4));
#else
            free_bytes = check_free_bytes((int*)thread->stack_addr);
#endif
            strncpy( name, thread->name, 8 );
            name[8] = 0;
            shell_printf("%8s %c %p %d/%d %p %p (free %d)\n", name, c, (uint32_t)thread,
                        thread->current_priority, thread->init_priority,
                        (uint32_t)thread->stack_addr, (uint32_t)thread->sp, free_bytes);
        }
    } while (next != (rt_list_t*)RT_NULL);
}


void os_queue_info_print(void)
{
    rt_ubase_t level;
    list_get_next_t find_arg;
    rt_list_t *obj_list[LIST_FIND_OBJ_NR];
    rt_list_t *next = (rt_list_t*)RT_NULL;
    int i;
    int pool_bytes;
    struct rt_object *obj;
    struct rt_messagequeue *m;
    char name[9];

    list_find_init(&find_arg, RT_Object_Class_MessageQueue, obj_list, sizeof(obj_list)/sizeof(obj_list[0]));

    do
    {
        next = list_get_next(next, &find_arg);
        {
            for( i = 0; i < find_arg.nr_out; i++ )
            {
                obj = rt_list_entry(obj_list[i], struct rt_object, list);
                level = rt_hw_interrupt_disable();
                if ((obj->type & ~RT_Object_Class_Static) != find_arg.type)
                {
                    rt_hw_interrupt_enable(level);
                    continue;
                }

                rt_hw_interrupt_enable(level);

                m = (struct rt_messagequeue *)obj;
                pool_bytes = m->max_msgs * m->msg_size;
                strncpy( name, m->parent.parent.name, 8 );
                name[8] = 0;
                shell_printf("%8s %p  %5d %4d %4d  %p - %p (%p)\n",
                            name, (uint32_t)m,
                            m->max_msgs, m->msg_size, m->entry, 
                            (int)m->msg_pool, (int)m->msg_pool+pool_bytes,
                            pool_bytes );
            }
        }
    }
    while (next != (rt_list_t*)RT_NULL);
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
    shell_printf( "%s %d.%d.%d\n", OS_NAME, RT_VERSION, RT_SUBVERSION, RT_REVISION );
    _print_kernel_item_name( "SysClock" );
    shell_printf( "%d\n", SystemCoreClock );
    _print_kernel_item_name( "TickRate" );
    shell_printf( "%d\n", OS_TICK_RATE );
}


void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}



#endif
