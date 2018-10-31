/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include "mcush.h"

#if HAL_RTC


/* R:0x52 T:0x54 C:0x43 M:0x4d */
#define MAGIC_CODE  0x4D435452  /* in ascii 'RTCM' */
typedef struct _rtc_mark_cb_t
{
    uint32_t magic_code;
}rtc_mark_cb_t;

rtc_mark_cb_t rtc_mark_cb  __attribute__((section(".bkpsram")));



int hal_rtc_get( struct tm *t )
{
    uint32_t time, date;

    if( rtc_mark_cb.magic_code != MAGIC_CODE )
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
    LL_RTC_DATE_SetYear( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_year - 2000) );
    LL_RTC_DATE_SetMonth( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_mon ) );
    LL_RTC_DATE_SetDay( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_mday ) );
    LL_RTC_DATE_SetWeekDay( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_wday ) );

    LL_RTC_TIME_SetFormat( RTC, LL_RTC_TIME_FORMAT_AM_OR_24 );   
    LL_RTC_TIME_SetHour( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_hour ) );   
    LL_RTC_TIME_SetMinute( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_min ) );   
    LL_RTC_TIME_SetSecond( RTC, __LL_RTC_CONVERT_BIN2BCD( t->tm_sec ) );   

    return 1;
}


static void hal_rtc_reset(void)
{
    struct tm t;
    LL_RTC_InitTypeDef rtc_init;
    uint32_t uwAsynchPrescaler;
    uint32_t uwSynchPrescaler;
 
    /* Enable the PWR clock */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_PWR );

    /* Enable the LSE OSC */
    //RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */  
    //while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    //{
    //}
    /* Select the RTC Clock Source */
    //LL_RCC_RTCCLKConfig( LL_RCC_RTCCLKSource_LSE );
    /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrescaler + 1)*(uwSynchPrescaler + 1)*/
    uwSynchPrescaler = 0xFF;
    uwAsynchPrescaler = 0x7F;

    /* Enable the RTC Clock */
    LL_RCC_EnableRTC();

    /* Wait for RTC APB registers synchronisation */
    LL_RTC_WaitForSynchro( RTC );
    
    /* Configure the RTC data register and RTC prescaler */
    rtc_init.AsynchPrescaler = uwAsynchPrescaler;
    rtc_init.SynchPrescaler = uwSynchPrescaler;
    rtc_init.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
    LL_RTC_Init( RTC, &rtc_init );
    
    /* Set new: 2018-1-1 00:00:00 */
    t.tm_year = 2018;
    t.tm_mon = 1;
    t.tm_mday = 1;
    t.tm_wday = 1;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    hal_rtc_set( &t );
}

void hal_rtc_init(void)
{
    /* Allow access to RTC */
    LL_PWR_EnableBkUpAccess();

    if( rtc_mark_cb.magic_code != MAGIC_CODE )
    {
        hal_rtc_reset();
        rtc_mark_cb.magic_code = MAGIC_CODE;
    }
    else
    {
        /* Wait for RTC APB registers synchronisation */
        LL_RTC_WaitForSynchro(RTC);
    }
}


#endif

