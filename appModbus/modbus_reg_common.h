/* General purpose Modbus registers */
/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MODBUS_HOLD_REG_COMMON_H__
#define __MODBUS_HOLD_REG_COMMON_H__


/* hold regsters kept at higher address */

/* model string, eg: "ShellLab-???" */
#define HOLD_REG_PRODUCT_MODEL_STRING_LEN        60000
#define HOLD_REG_PRODUCT_MODEL_BEGIN             60001
#define HOLD_REG_PRODUCT_MODEL_END               60019

/* firmware version string, eg: "2.0pre" */
#define HOLD_REG_VERSION_STRING_LEN              60020
#define HOLD_REG_VERSION_BEGIN                   60021
#define HOLD_REG_VERSION_END                     60039

/* serial number string, eg: "Eng-test-001" */
#define HOLD_REG_SERIAL_NUM_STRING_LEN           60040
#define HOLD_REG_SERIAL_NUM_BEGIN                60041
#define HOLD_REG_SERIAL_NUM_END                  60069

/* errno value for blinking task */
#define HOLD_REG_ERROR_NUMBER_LO                 60070
#define HOLD_REG_ERROR_NUMBER_HI                 60071

/* uptime counter */
#define HOLD_REG_UPTIME_LO                       60072
#define HOLD_REG_UPTIME_HI                       60073

/* reboot counter */
#define HOLD_REG_REBOOT_COUNTER_LO               60074
#define HOLD_REG_REBOOT_COUNTER_HI               60075

/* reboot command */
#define HOLD_REG_REBOOT_COMMAND                  60076

/* RTC */
#define HOLD_REG_RTC_SYNCED                      60080
#define HOLD_REG_RTC_UPDATE                      60081
#define HOLD_REG_RTC_YEAR                        60082
#define HOLD_REG_RTC_MONTH                       60083
#define HOLD_REG_RTC_DAY                         60084
#define HOLD_REG_RTC_HOUR                        60085
#define HOLD_REG_RTC_MINUTE                      60086
#define HOLD_REG_RTC_SECOND                      60087



#endif

