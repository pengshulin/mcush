/* MCUSH reserved hold registers routine for modbus hooks */
#include "mcush.h"
#include "modbus_mcush_reg.h"


extern char model_string[];
extern char version_string[];



int modbus_read_mcush_hold_register( uint16_t address, uint16_t *value )
{
    char buf[64];
#if HAL_RTC
    struct tm t;
#endif
    int i;

    switch( address )
    {
    case HOLD_REG_PRODUCT_MODEL_STRING_LEN:
        *value = strlen(model_string);
        break;
    case HOLD_REG_VERSION_STRING_LEN:
        *value = strlen(version_string);
        break;
    case HOLD_REG_SERIAL_NUM_STRING_LEN:
        hal_get_serial_number(buf);
        *value = strlen(buf);
        break;
    case HOLD_REG_ERROR_NUMBER_LO:
        *value = get_errno() & 0xFFFF;
        break;
    case HOLD_REG_ERROR_NUMBER_HI:
        *value = (get_errno() >> 16) & 0xFFFF;
        break;
    case HOLD_REG_UPTIME_LO:
        *value = os_tick() & 0xFFFF;
        break;
    case HOLD_REG_UPTIME_HI:
        *value = (os_tick() >> 16) & 0xFFFF;
        break;
    case HOLD_REG_REBOOT_COUNTER_LO:
        *value = hal_reboot_counter_get() & 0xFFFF;
        break;
    case HOLD_REG_REBOOT_COUNTER_HI:
        *value = (hal_reboot_counter_get()>>16) & 0xFFFF;
        break;
#if HAL_RTC
    case HOLD_REG_RTC_SYNCED:
        *value = hal_rtc_is_synced();
        break;
    case HOLD_REG_RTC_YEAR:
        if( hal_rtc_get(&t) )
            *value = t.tm_year;
        break;
    case HOLD_REG_RTC_MONTH:
        if( hal_rtc_get(&t) )
            *value = t.tm_mon;
        break;
    case HOLD_REG_RTC_DAY:
        if( hal_rtc_get(&t) )
            *value = t.tm_mday;
        break;
    case HOLD_REG_RTC_HOUR:
        if( hal_rtc_get(&t) )
            *value = t.tm_hour;
        break;
    case HOLD_REG_RTC_MINUTE:
        if( hal_rtc_get(&t) )
            *value = t.tm_min;
        break;
    case HOLD_REG_RTC_SECOND:
        if( hal_rtc_get(&t) )
            *value = t.tm_sec;
        break;
#endif
    default:
        if( address >= HOLD_REG_PRODUCT_MODEL_BEGIN && address <= HOLD_REG_PRODUCT_MODEL_END )
        {
            /* model */
            i = address - HOLD_REG_PRODUCT_MODEL_BEGIN;
            if( i >= strlen(model_string) )
                *value = ' ';
            else
                *value = model_string[i];
        }
        else if( address >= HOLD_REG_VERSION_BEGIN && address <= HOLD_REG_VERSION_END )
        {
            /* firmware version */
            i = address - HOLD_REG_VERSION_BEGIN;
            if( i >= strlen(version_string) )
                *value = ' ';
            else
                *value = version_string[i];
        }
        else if( address >= HOLD_REG_SERIAL_NUM_BEGIN && address <= HOLD_REG_SERIAL_NUM_END )
        {
            /* serial number */
            i = address - HOLD_REG_SERIAL_NUM_BEGIN;
            hal_get_serial_number(buf);
            if( i >= strlen(buf) )
                *value = ' ';
            else
                *value = buf[i];
        }
        else
            return 0;
    }
    return 1;
}


int modbus_write_mcush_hold_register( uint16_t address, uint16_t value )
{
    static uint16_t _buffered;
#if HAL_RTC
    static struct tm newtime;
#endif

    switch( address )
    {
    case HOLD_REG_ERROR_NUMBER_LO:
        _buffered = value;
        break;
    case HOLD_REG_ERROR_NUMBER_HI:
        set_errno( (value << 16) + _buffered );
        break;
    case HOLD_REG_REBOOT_COMMAND:
        if( value )
            hal_reboot();
        break;
#if HAL_RTC
    case HOLD_REG_RTC_UPDATE:
        hal_rtc_set( &newtime ); 
        break;
    case HOLD_REG_RTC_YEAR:
        newtime.tm_year = value;
        break;
    case HOLD_REG_RTC_MONTH:
        newtime.tm_mon = value;
        break;
    case HOLD_REG_RTC_DAY:
        newtime.tm_mday = value;
        break;
    case HOLD_REG_RTC_HOUR:
        newtime.tm_hour = value;
        break;
    case HOLD_REG_RTC_MINUTE:
        newtime.tm_min = value;
        break;
    case HOLD_REG_RTC_SECOND:
        newtime.tm_sec = value;
        break;
#endif
    default:
        return 0;
    }
    return 1;
}

