/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"
#include "mcush.h"

#if HAL_RTC

/* R:0x52 T:0x54 C:0x43 M:0x4d */
#define RTC_MARK  0x4D435452   


int hal_rtc_get( struct tm *t )
{
    RTC_DateTypeDef rtc_date;
    RTC_TimeTypeDef rtc_time;

    if( RTC_ReadBackupRegister(RTC_BKP_DR0 ) != RTC_MARK )
        return 0; 
    
    RTC_GetDate(RTC_Format_BIN, &rtc_date);
    RTC_GetTime(RTC_Format_BIN, &rtc_time);   
  
    t->tm_sec  = rtc_time.RTC_Seconds;
    t->tm_min  = rtc_time.RTC_Minutes;
    t->tm_hour = rtc_time.RTC_Hours;
    if( rtc_time.RTC_H12 == RTC_H12_PM )
        t->tm_hour += 12;
    t->tm_mday = rtc_date.RTC_Date;
    t->tm_wday = rtc_date.RTC_WeekDay;
    t->tm_mon  = rtc_date.RTC_Month;
    t->tm_year = rtc_date.RTC_Year + 2000;

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
    RTC_DateTypeDef rtc_date;
    RTC_TimeTypeDef rtc_time;
    int err1, err2;

    rtc_date.RTC_Year = t->tm_year - 2000;
    rtc_date.RTC_Month = t->tm_mon;
    rtc_date.RTC_Date = t->tm_mday;
    rtc_date.RTC_WeekDay = t->tm_wday;
    err1 = RTC_SetDate(RTC_Format_BIN, &rtc_date);
   
    rtc_time.RTC_H12     = RTC_H12_AM;
    rtc_time.RTC_Hours   = t->tm_hour;
    rtc_time.RTC_Minutes = t->tm_min;
    rtc_time.RTC_Seconds = t->tm_sec;
    err2 = RTC_SetTime(RTC_Format_BIN, &rtc_time);

    return ((err1==SUCCESS) && (err2==SUCCESS)) ? 1 : 0;
}


static int hal_rtc_clk_init(void)
{
    RTC_InitTypeDef rtc_init;
    int succ;
    int i;
 
    /* Enable the LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait for LSE ready */
    for( i=5000; i; i-- )
    {
        if( RCC_GetFlagStatus(RCC_FLAG_LSERDY) == SET )
            break;
        hal_delay_ms( 1 );
    }

    if( i )
    {
        /* Select the RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }
    else
    {
        RCC_LSEConfig(RCC_LSE_OFF);
        
        /* Switch to LSI instead */
        RCC_LSICmd(ENABLE);
        for( i=1000; i; i-- )
        {
            if( RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == SET )
                break;
            hal_delay_ms( 1 );
        }

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    }

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    RTC_WriteProtectionCmd(DISABLE);
    
    /* Init RTC clock */
    RTC_StructInit(&rtc_init);
    succ = (RTC_Init(&rtc_init) == SUCCESS ) ? 1 : 0;

    RTC_WriteProtectionCmd(ENABLE);

    return succ;
}


static int hal_rtc_reset(void)
{
    struct tm t;

    /* 2017-1-1 00:00:00 */
    t.tm_year = 2017;
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    PWR_BackupRegulatorCmd(ENABLE);

    /* Wait for backup regulator ready */
    for( i=100; i; i-- )
    {
        if( PWR_GetFlagStatus(PWR_FLAG_BRR) == SET )
            break;
        hal_delay_ms( 1 );
    }
   
    PWR_MainRegulatorModeConfig( PWR_Regulator_Voltage_Scale1 );  /* at maximum freq */
    return i == 0 ? 0 : 1;
}


void hal_rtc_init(void)
{
    hal_backup_sram_init();
    if( hal_rtc_clk_init() )
    {
        if( RTC_ReadBackupRegister(RTC_BKP_DR0 ) != RTC_MARK )
        {
            if( hal_rtc_reset() )
                RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_MARK);
        }
    }
}


int hal_rtc_is_synced(void)
{
    return 1;
}


#endif
