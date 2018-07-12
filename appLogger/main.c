/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_logger.h"
#include "task_blink.h"
#include "timers.h"

#define DEMO_EVENT_TIMER_MS  50

static TimerHandle_t timer_event_generator;

static void cb_timer_event_generator(TimerHandle_t *xTimer)
{
    static int msgid=0;
    char buf[50];
    //hal_led_toggle(1);
    sprintf( buf, "This is a demo log event %d!", msgid++ );
    logger_str( LOG_INFO, buf );
}


int main(void)
{
    mcush_init();
    task_logger_init();
    task_blink_init();
    timer_event_generator = xTimerCreate( "GEvt", DEMO_EVENT_TIMER_MS*configTICK_RATE_HZ/1000, pdTRUE, 0, (TimerCallbackFunction_t)cb_timer_event_generator );
    xTimerStart( timer_event_generator, 0 );
    mcush_start();
    while(1);
}


