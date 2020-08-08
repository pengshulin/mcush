#include "mcush.h"
#include "canfestival.h"


void initTimer(void)
{

}

os_tick_t t0;
TIMEVAL tmr;

void setTimer(TIMEVAL value)
{
    t0 = os_tick();
    tmr = value;
#if DEBUG_WAR_CONSOLE_ON
    canopen_debug_msg( 0, 0, "setTimer : ", value );
#endif
}


TIMEVAL getElapsedTime(void)
{
    unsigned int dt;

    dt = os_tick() - t0;
#if DEBUG_WAR_CONSOLE_ON
    canopen_debug_msg( 0, 0, "getElapsedTime : ", dt );
#endif
    return dt;
}


UNS8 canSend(CAN_PORT canx, Message *m)
{
    can_message_t msg;

    (void)canx;
    
    msg.id = m->cob_id;
    msg.ext = 0;
    msg.remote = m->rtr;
    msg.len = m->len;
    memcpy( msg.data, m->data, m->len );

    return (UNS8) hal_can_write( &msg );
}

