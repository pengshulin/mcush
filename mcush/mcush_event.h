/* event bits configuration
 * must be compiled with MCUSH_NON_OS defined
 * MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_EVENT_H__
#define __MCUSH_EVENT_H__
#if defined(MCUSH_NON_OS)

typedef unsigned int event_t;

enum BITS_DEF {
    BIT_DEF_INIT = 0,
    BIT_DEF_TIMER,
    BIT_DEF_POWER_ON,
    BIT_DEF_POWER_OFF,
    BIT_DEF_NUM,
};


enum BITS_MCUSH {
    BIT_MCUSH_CHAR = BIT_DEF_NUM,
    BIT_MCUSH_LINE,
};



/* convert bits name to events name */
#define EVT_INIT   (1l<<(BIT_DEF_INIT))
#define EVT_TIMER  (1l<<(BIT_DEF_TIMER))
#define EVT_MCUSH_CHAR (1l<<(BIT_MCUSH_CHAR))
#define EVT_MCUSH_LINE (1l<<(BIT_MCUSH_LINE))
#define EVT(tsk,evt)   (1l<<(BIT_##tsk##_##evt))

#define set_event(dst, evt)  dst |= (evt)

extern event_t event_mcush;

#endif
#endif

