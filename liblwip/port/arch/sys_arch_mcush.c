/* sys_arch interfaces port for MCUSH platform */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "lwip/sys.h"
#include "lwip/stats.h"


int sys_mbox_valid(sys_mbox_t *mbox)
{
    return (*mbox != NULL);
}


void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = NULL;
}


void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = NULL;
}


int sys_sem_valid(sys_sem_t* sem)
{
    return (*sem != NULL);
}


err_t sys_mbox_new( sys_mbox_t *mbox, int size )
{
    *mbox = os_queue_create( NULL, size, sizeof(void *) );
#if SYS_STATS
    if( *mbox == NULL )
        ++lwip_stats.sys.mbox.err;
    else
    {
        ++lwip_stats.sys.mbox.used;
        if( lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used )
            lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
    }
#endif
    return (*mbox==NULL) ? ERR_MEM : ERR_OK;
}


void sys_mbox_free(sys_mbox_t* mbox)
{
    if( os_queue_is_empty( *mbox ) == 0 )
    {
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif
#if DEBUG
        halt("mbox freed while not empty");  // for debug
#endif
    }
    os_queue_delete( *mbox );
#if SYS_STATS
    --lwip_stats.sys.mbox.used;
#endif
}


void sys_mbox_post(sys_mbox_t* mbox, void *msg)
{
    os_queue_put( *mbox, msg, -1 );
}


err_t sys_mbox_trypost(sys_mbox_t* mbox, void *msg)
{
    if( os_queue_put( *mbox, &msg, 0 ) == 0 )
    {
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif
        return ERR_MEM;
    }
    return ERR_OK;
}


err_t sys_mbox_trypost_fromisr(sys_mbox_t* mbox, void *msg)
{
    if( os_queue_put_isr( *mbox, &msg ) == 0 )
    {
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif
        return ERR_MEM;
    }
    return ERR_OK;
}


u32_t sys_arch_mbox_fetch( sys_mbox_t *mbox, void **msg, u32_t timeout )
{
    os_tick_t t0 = os_tick();
    if( os_queue_get( *mbox, msg, timeout==0 ? -1 : timeout ) == 0 )
        return SYS_ARCH_TIMEOUT;
    return (os_tick() - t0)*1000ul/OS_TICK_RATE;
}


u32_t sys_arch_mbox_tryfetch( sys_mbox_t *mbox, void **msg )
{
    return (os_queue_get( *mbox, msg, 0 ) == 0) ? SYS_MBOX_EMPTY : ERR_OK;
}


err_t sys_mutex_new(sys_mutex_t* mutex)
{
    *mutex = os_mutex_create();
#if SYS_STATS
    if( *mutex == NULL )
        ++lwip_stats.sys.mutex.err;
    else
    {
        ++lwip_stats.sys.mutex.used;
        if( lwip_stats.sys.mutex.max < lwip_stats.sys.mutex.used )
            lwip_stats.sys.mutex.max = lwip_stats.sys.mutex.used;
    }
#endif
    return (*mutex == NULL) ? ERR_MEM : ERR_OK;
}


void sys_mutex_lock( sys_mutex_t *mutex )
{
    os_mutex_get( *mutex, -1 );
}


void sys_mutex_unlock( sys_mutex_t *mutex )
{
    os_mutex_put( *mutex );
}


err_t sys_sem_new( sys_sem_t *sem, u8_t count )
{
    *sem = os_semaphore_create( 1, count?1:0 );
#if SYS_STATS
    if( *sem == NULL )
        ++lwip_stats.sys.sem.err;
    else
    {
        ++lwip_stats.sys.sem.used;
        if( lwip_stats.sys.sem.max < lwip_stats.sys.sem.used )
            lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
    }
#endif
    return (*sem==NULL) ? ERR_MEM : ERR_OK;
}


u32_t sys_arch_sem_wait( sys_sem_t *sem, u32_t timeout )
{
    os_tick_t t0 = os_tick();
    if( os_semaphore_get( *sem, timeout==0 ? -1 : timeout ) == 0 )
        return SYS_ARCH_TIMEOUT;
    return (os_tick() - t0)*1000ul/OS_TICK_RATE;
}


void sys_sem_signal(sys_sem_t* sem)
{
    os_semaphore_put( *sem );
}


void sys_sem_free( sys_sem_t *sem )
{
    os_semaphore_delete( *sem );
#if SYS_STATS
    --lwip_stats.sys.sem.used;
#endif
}


void sys_init(void)
{
}


struct sys_timeouts *sys_arch_timeouts(void)
{
    return NULL;
}


sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int priority)
{
    return os_task_create( name, thread, arg, stacksize, priority );
}


sys_prot_t sys_arch_protect(void)
{
    os_enter_critical();
    return 1;
}


void sys_arch_unprotect(sys_prot_t pval)
{
    (void)pval;
    os_exit_critical();
}


u32_t sys_now(void)
{
    return (u32_t)os_tick() * 1000ul / OS_TICK_RATE; /* in ms */
}


extern sys_mutex_t lock_tcpip_core;
void lwip_assert_core_locked(void)
{
    if( lock_tcpip_core == NULL )
        return;
    LWIP_ASSERT( "tcpip core locked", os_mutex_holder(lock_tcpip_core) != NULL );
}

