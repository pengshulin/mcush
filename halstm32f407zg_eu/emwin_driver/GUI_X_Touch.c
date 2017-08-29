/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/
#include "GUI.h"
#include "hal.h"
#include "LCDConf.h"

extern int Touch_GetSite(uint16_t *prow, uint16_t *pcol);

void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}

int GUI_TOUCH_X_MeasureX(void) {
    uint16_t row, col;
    if( Touch_GetSite(&row, &col) )
    {
#if LCD_DIR
        return col;
#else
        return (319-row);
#endif
    }
    return -1;
}

int GUI_TOUCH_X_MeasureY(void) {
    uint16_t row, col;
    if(Touch_GetSite(&row, &col))
    {
#if LCD_DIR
        return row;
#else
        return col;
#endif
    }
    return -1;
}


