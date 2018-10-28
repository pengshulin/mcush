/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

static char _wdg_enable=0;

void hal_wdg_init(void)
{
    //IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
    //IWDG_SetReload( 0xFFF );
    //IWDG_SetPrescaler( IWDG_Prescaler_4 );  /* 4 * 0xfff / 32768 -> 0.5s */
    //IWDG_SetPrescaler( IWDG_Prescaler_8 );  /* 8 * 0xfff / 32768 -> 1s */
    //IWDG_SetPrescaler( IWDG_Prescaler_16 );  /* 16 * 0xfff / 32768 -> 2s */
    //IWDG_SetPrescaler( IWDG_Prescaler_32 );  /* 32 * 0xfff / 32768 -> 4s */
    //IWDG_SetPrescaler( IWDG_Prescaler_64 );  /* 64 * 0xfff / 32768 -> 8s */
    //IWDG_SetPrescaler( IWDG_Prescaler_128 );  /* 128 * 0xfff / 32768 -> 16s */
    //IWDG_SetPrescaler( IWDG_Prescaler_256 );  /* 256 * 0xfff / 32768 -> 32s */

    /* enable if necessary */
#if HAL_WDG_ENABLE
    hal_wdg_enable();
#endif
}

int hal_wdg_is_enable(void)
{
    return _wdg_enable;
}

void hal_wdg_enable(void)
{
    //IWDG_ReloadCounter();
    //IWDG_Enable();
    _wdg_enable = 1;
}

void hal_wdg_disable(void)
{
    /* invalid for stm32/IWDG */ 
}

void hal_wdg_clear(void)
{
    //IWDG_ReloadCounter();
}



