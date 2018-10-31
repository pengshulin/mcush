/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

static char _wdg_enable=0;

void hal_wdg_init(void)
{
    LL_IWDG_EnableWriteAccess( IWDG );
    LL_IWDG_SetReloadCounter( IWDG, 0xFFF );
    //LL_IWDG_SetPrescaler( IWDG, LL_IWDG_PRESCALER_4 );  /* 4 * 0xfff / 32768 -> 0.5s */
    //LL_IWDG_SetPrescaler( IWDG, LL_IWDG_PRESCALER_8 );  /* 8 * 0xfff / 32768 -> 1s */
    LL_IWDG_SetPrescaler( IWDG, LL_IWDG_PRESCALER_16 );  /* 16 * 0xfff / 32768 -> 2s */
    //LL_IWDG_SetPrescaler( IWDG, L_IWDG_PRESCALER_32 );  /* 32 * 0xfff / 32768 -> 4s */
    //LL_IWDG_SetPrescaler( IWDG, L_IWDG_PRESCALER_64 );  /* 64 * 0xfff / 32768 -> 8s */
    //LL_IWDG_SetPrescaler( IWDG, L_IWDG_PRESCALER_128 );  /* 128 * 0xfff / 32768 -> 16s */
    //LL_IWDG_SetPrescaler( IWDG, L_IWDG_PRESCALER_256 );  /* 256 * 0xfff / 32768 -> 32s */

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
    LL_IWDG_ReloadCounter(IWDG);
    LL_IWDG_Enable(IWDG);
    _wdg_enable = 1;
}

void hal_wdg_disable(void)
{
    /* invalid for stm32/IWDG */ 
}

void hal_wdg_clear(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}



