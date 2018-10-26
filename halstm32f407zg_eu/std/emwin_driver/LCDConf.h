#ifndef __LCDCONF_H__
#define __LCDCONF_H__

#include "hal.h"

#ifndef LCD_DIR
    #define  LCD_DIR  0  // 1-竖 0-横
#endif

#if LCD_DIR
    #define LCD_XSIZE  240
    #define LCD_YSIZE  320
#else
    #define LCD_XSIZE  320
    #define LCD_YSIZE  240
#endif
    
   
#define LCD_BITSPERPIXEL  16
#define LCD_FIXEDPALETTE  565
 
#define LCD_MIRROR_X  1
#define LCD_MIRROR_Y  0
#define LCD_SWAP_XY   0

#endif
