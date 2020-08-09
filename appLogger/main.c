/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_logger.h"
#include "task_blink.h"


#define DEMO_EVENT_TIMER_MS  50

static os_timer_handle_t timer_event_generator;

LOGGER_MODULE_NAME("test");

static void cb_timer_event_generator( os_timer_handle_t timer )
{
    static int msgid=0;
    char buf[50];

    (void)timer;
    //hal_led_toggle(1);
    sprintf( buf, "This is a demo log event %d!", msgid++ );
    logger_info( buf );
}


int main(void)
{
    mcush_init();
    task_logger_init();
    task_blink_init();
    timer_event_generator = os_timer_create( "evtTmr", OS_TICKS_MS(DEMO_EVENT_TIMER_MS), 1, cb_timer_event_generator );
    os_timer_start( timer_event_generator );
    mcush_start();
    while(1);
}


