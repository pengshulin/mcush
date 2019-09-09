/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include "mcush.h"

#if HAL_RTC

//#define USE_BKPSRAM  1
//#define DEBUG_CLK_SRC_LED  3

#define RTC_MARK  0x4D435452    /* in ascii 'RTCM' */ 

#if USE_BKPSRAM
typedef struct _rtc_mark_cb_t
{
    uint32_t magic_code;
}rtc_mark_cb_t;
rtc_mark_cb_t rtc_mark_cb  __attribute__((section(".bkpsram")));
#endif



int hal_rtc_get( struct tm *t )
{
    uint32_t time, date;

#if USE_BKPSRAM
    if( rtc_mark_cb.magic_code != RTC_MARK )
#else
    if( LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != RTC_MARK )
#endif
        return 0; 
    
    date = LL_RTC_DATE_Get( RTC );  /* 0xWWDDMMYY */
    time = LL_RTC_TIME_Get( RTC );  /* 0x00HHMMSS */
  
    t->tm_year = __LL_RTC_CONVERT_BCD2BIN(date & 0xFF) + 2000;
    t->tm_mday = __LL_RTC_CONVERT_BCD2BIN((date>>16) & 0xFF);
    t->tm_wday = __LL_RTC_CONVERT_BCD2BIN((date>>24) & 0xFF);
    t->tm_mon  = __LL_RTC_CONVERT_BCD2BIN((date>>8) & 0xFF);
    t->tm_sec  = __LL_RTC_CONVERT_BCD2BIN(time & 0xFF);
    t->tm_min  = __LL_RTC_CONVERT_BCD2BIN((time>>8) & 0xFF);
    t->tm_hour = __LL_RTC_CONVERT_BCD2BIN((time>>16) & 0xFF);
    if( LL_RTC_TS_GetTimeFormat(RTC) == LL_RTC_TS_TIME_FORMAT_PM )
        t->tm_hour += 12;

    return 1;
}


int hal_rtc_get_tick( uint64_t *tick )
{
    struct tm t;
    if( !hal_rtc_get( &t ) )
        return 0;

    t.tm_mon -= 1;
    t.tm_year -= 1900;
    *tick = mktime( &t );
    *tick -= 8*60*60;
    return 1; 
}


int hal_rtc_set( struct tm *t )
{
    LL_RTC_DateTypeDef rtc_date;
    LL_RTC_TimeTypeDef rtc_time;
    int succ = 0;

    LL_RTC_DisableWriteProtection(RTC);
    if( LL_RTC_EnterInitMode(RTC) != ERROR )
    {
        rtc_date.Year = t->tm_year - 2000;
        rtc_date.Month = t->tm_mon;
        rtc_date.Day = t->tm_mday;
        rtc_date.WeekDay = t->tm_wday;
        LL_RTC_DATE_Init( RTC, LL_RTC_FORMAT_BIN, &rtc_date );

        rtc_time.Hours = t->tm_hour;
        rtc_time.Minutes = t->tm_min;
        rtc_time.Seconds = t->tm_sec;
        rtc_time.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24;
        LL_RTC_TIME_Init( RTC, LL_RTC_FORMAT_BIN, &rtc_time );
        
        succ = 1;
        LL_RTC_ExitInitMode(RTC);
    }
    LL_RTC_EnableWriteProtection(RTC);
    return succ;
}


static int hal_rtc_clk_init(void)
{
    LL_RTC_InitTypeDef rtc_init;
    int succ;
    int i;
 
    /* Enable both LSE/LSI */
    LL_RCC_LSE_Enable();
    LL_RCC_LSI_Enable();

    /* Wait for LSE ready */  
    for( i=2000; i; i-- )
    {
        if( LL_RCC_IsActiveFlag_LSERDY() )
            break;
        hal_delay_ms( 1 );
    }

    /* Select the RTC Clock Source */
    if( i )
    {
#if DEBUG_CLK_SRC_LED
        hal_led_clr( DEBUG_CLK_SRC_LED );
#endif
        LL_RCC_LSI_Disable();
        LL_RCC_SetRTCClockSource( LL_RCC_RTC_CLKSOURCE_LSE );
    }
    else
    {
#if DEBUG_CLK_SRC_LED
        hal_led_set( DEBUG_CLK_SRC_LED );
#endif
        LL_RCC_LSE_Disable();
        LL_RCC_SetRTCClockSource( LL_RCC_RTC_CLKSOURCE_LSI );
    }

    /* Enable the RTC Clock */
    LL_RCC_EnableRTC();

    /* Wait for RTC APB registers synchronisation */
    LL_RTC_WaitForSynchro(RTC);

    LL_RTC_DisableWriteProtection(RTC);
    
    /* Init RTC clock */
    LL_RTC_StructInit(&rtc_init);
    succ = (LL_RTC_Init( RTC, &rtc_init ) == SUCCESS) ? 1 : 0;

    LL_RTC_EnableWriteProtection(RTC);

    return succ;
}


static int hal_rtc_reset(void)
{
    struct tm t;

    /* 2019-1-1 00:00:00 */
    t.tm_year = 2019;
    t.tm_mon = 1;
    t.tm_mday = 1;
    t.tm_wday = 0;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    return hal_rtc_set( &t );
}



int hal_backup_sram_init(void)
{
    int i;

    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_BKPSRAM );
    LL_PWR_EnableBkUpAccess();
    LL_PWR_EnableBkUpRegulator();

    /* Wait for backup regulator ready */
    for( i=100; i; i-- )
    {
        if( LL_PWR_IsActiveFlag_BRR() )
            break;
        hal_delay_ms( 1 );
    }
   
    LL_PWR_SetRegulVoltageScaling( LL_PWR_REGU_VOLTAGE_SCALE1 );  /* at maximum freq */
    return i == 0 ? 0 : 1;
}


void hal_rtc_init(void)
{
    hal_backup_sram_init();

    if( hal_rtc_clk_init() )
    {
#if USE_BKPSRAM
        if( rtc_mark_cb.magic_code != RTC_MARK )
        {
            if( hal_rtc_reset() )
                rtc_mark_cb.magic_code = RTC_MARK;
        }
#else
        if( LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR0) != RTC_MARK )
        {
            if( hal_rtc_reset() )
                LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, RTC_MARK );
        }
#endif
    }
}


#endif

