#ifndef GUITOUCH_CONF_H
#define GUITOUCH_CONF_H
#include "LCDConf.h"


#define GUI_TOUCH_SWAP_XY       0
#define GUI_TOUCH_MIRROR_X      0
#define GUI_TOUCH_MIRROR_Y      0

#if LCD_DIR
    #define GUI_TOUCH_AD_LEFT       0
    #define GUI_TOUCH_AD_RIGHT      240
    #define GUI_TOUCH_AD_TOP        0
    #define GUI_TOUCH_AD_BOTTOM     320
#else
    #define GUI_TOUCH_AD_LEFT       0
    #define GUI_TOUCH_AD_RIGHT      320
    #define GUI_TOUCH_AD_TOP        0
    #define GUI_TOUCH_AD_BOTTOM     240
#endif

#endif
