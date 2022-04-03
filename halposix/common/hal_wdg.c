/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#ifndef WDG_TIMEOUT_MS
    #define WDG_TIMEOUT_MS  2000
#endif

static pthread_t hal_wdg_thread;
static os_queue_handle_t hal_wdg_queue=NULL;

void hal_wdg_enable(void);

void hal_wdg_init(void)
{
    //printf("hal_wdg_init\n");
    /* enable if necessary */
#if HAL_WDG_ENABLE
    hal_wdg_enable();
#endif
}


int hal_wdg_is_enable(void)
{
    return (hal_wdg_queue==NULL) ? 0 : 1;
}


void *thread_wdg(void *arg)
{
    char c;

    (void)arg;
    while(1)
    {
        if( os_queue_get( hal_wdg_queue, &c, OS_TICKS_MS(WDG_TIMEOUT_MS) ) == 0 )
        {
            /* stop */
            printf("wdg expired\n");
            exit(1);
        }
    }
}


void hal_wdg_enable(void)
{
    hal_wdg_queue = os_queue_create( "wdgQ", 1, 1 );
    if( hal_wdg_queue == NULL )
        return;

    if( pthread_create( &hal_wdg_thread, NULL, thread_wdg, NULL ) != 0 )
        return;

    printf("watch dog task started\n");
}


void hal_wdg_disable(void)
{

}


void hal_wdg_clear(void)
{
    if( hal_wdg_queue != NULL )
    {
        os_queue_put( hal_wdg_queue, "\x00", 0 );
        printf("feed watch dog\n");
    }
    else
    {
        printf("feed watch dog (not started)\n");
    }
}


