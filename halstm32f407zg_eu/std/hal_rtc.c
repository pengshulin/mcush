#include "hal.h"
#include "mcush.h"

#if HAL_RTC

/* R:0x52 T:0x54 C:0x43 M:0x4d */
#define RTC_MARK  0x4D435452   


int hal_rtc_get( struct tm *t )
{
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;

    if( RTC_ReadBackupRegister(RTC_BKP_DR0 ) != RTC_MARK )
        return 0; 
    
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);   
  
    t->tm_sec  = RTC_TimeStructure.RTC_Seconds;
    t->tm_min  = RTC_TimeStructure.RTC_Minutes;
    t->tm_hour = RTC_TimeStructure.RTC_Hours;
    if( RTC_TimeStructure.RTC_H12 == RTC_H12_PM )
        t->tm_hour += 12;
    t->tm_mday = RTC_DateStructure.RTC_Date;
    t->tm_wday = RTC_DateStructure.RTC_WeekDay;
    t->tm_mon  = RTC_DateStructure.RTC_Month;
    t->tm_year = RTC_DateStructure.RTC_Year + 2000;

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
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;

    RTC_DateStructure.RTC_Year = t->tm_year - 2000;
    RTC_DateStructure.RTC_Month = t->tm_mon;
    RTC_DateStructure.RTC_Date = t->tm_mday;
    RTC_DateStructure.RTC_WeekDay = t->tm_wday;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
    
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = t->tm_hour;
    RTC_TimeStructure.RTC_Minutes = t->tm_min;
    RTC_TimeStructure.RTC_Seconds = t->tm_sec;
    
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);   
    return 1;
}


static void hal_rtc_reset(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;
    uint32_t uwAsynchPrediv;
    uint32_t uwSynchPrediv;
 
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
    uwSynchPrediv = 0xFF;
    uwAsynchPrediv = 0x7F;

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
    
    /* Set the date: 2017-1-1 */
    RTC_DateStructure.RTC_Year = 0x17;
    RTC_DateStructure.RTC_Month = RTC_Month_January;
    RTC_DateStructure.RTC_Date = 0x1;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Sunday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
    
    /* Set the time to 00:00:00 */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x00;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00; 
    
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
}

void hal_rtc_init(void)
{
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    if( RTC_ReadBackupRegister(RTC_BKP_DR0 ) != RTC_MARK )
    {
        hal_rtc_reset();
        RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_MARK);
    }
    else
    {
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
    }
}


#endif

