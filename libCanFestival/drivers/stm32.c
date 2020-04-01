#include "mcush.h"
#include "canfestival.h"


void initTimer(void)
{

}

TickType_t t0;
TIMEVAL tmr;

void setTimer(TIMEVAL value)
{
    t0 = xTaskGetTickCount();
    tmr = value;
#if DEBUG_WAR_CONSOLE_ON
    canopen_debug_msg( 0, 0, "setTimer : ", value );
#endif
}


TIMEVAL getElapsedTime(void)
{
    unsigned int dt;
    dt = xTaskGetTickCount() - t0;
#if DEBUG_WAR_CONSOLE_ON
    canopen_debug_msg( 0, 0, "getElapsedTime : ", dt );
#endif
    return dt;
}


unsigned char canSend(CAN_PORT canx, Message *m)
{
    can_message_t msg;
    
    msg.id = m->cob_id;
    msg.ext = 0;
    msg.remote = m->rtr;
    msg.len = m->len;
    memcpy( msg.data, m->data, m->len );

    return hal_can_write( &msg );
}

