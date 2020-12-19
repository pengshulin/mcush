////////////////////////////////////////////////////////////////////////////////
/// @file    SYSTEM_MM32.H
/// @author  AE TEAM
/// @version V2.0.0
/// @date    2018-08-01
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FIRMWARE FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////


// Define to prevent recursive inclusion  --------------------------------------
#ifndef __SYSTEM_MM32_H
#define __SYSTEM_MM32_H

// Files includes  -------------------------------------------------------------
#include "types.h"
//#include "drv.h"


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURCE
/// @brief MM32 Examples resource modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Constants
/// @{


#define IDCODE_DEVID_MASK       (0x00000FFFU)
#define COREID_MASK             (0x00000070U)
#define MCUID_MM0N1             (0xCC568091U)
#define MCUID_MM0P1             (0xCC56A091U)   // Rev.C
#define MCUID_MM0P2             (0xCC56A097U)   // Rev.E
#define MCUID_MM0Q1             (0xCC4460B1U)
#define MCUID_MM0M1             (0xCC567071U)
#define MCUID_MM3O1             (0xCC9AA0A1U)
#define MCUID_MM3M1             (0xCC888045U)
#define MCUID_MM3N1             (0xCC888047U)

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Enumeration
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief MCU ID enumerate definition.
/// @anchor EM_MCUID
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    Unknown,
    emMCUID_MM32L073,
    emMCUID_MM32SPIN27,
    emMCUID_MM32F031,
    emMCUID_MM32F031_OLD,
    emMCUID_MM32L395,
    emMCUID_MM32F103_OLD,
    emMCUID_MM32L373
}EM_MCUID;

////////////////////////////////////////////////////////////////////////////////
/// @brief Systick enable enumerate definition.
/// @anchor EM_SYSTICK
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    emSYSTICK_Off,
    emSYSTICK_On = !emSYSTICK_Off
} EM_SYSTICK;


////////////////////////////////////////////////////////////////////////////////
/// @brief  System clock configuration
////////////////////////////////////////////////////////////////////////////////
typedef enum {
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
    SYSCLK_HSI_6d,     //   HSI_6d,
    SYSCLK_HSI_12MHz,  //   HSI_12MHz,
    SYSCLK_HSI_24MHz,  //   HSI_24MHz,
    SYSCLK_HSI_48MHz,  //   HSI_48MHz,
    SYSCLK_HSI_72MHz,  //   HSI_72MHz,
    SYSCLK_HSI_96MHz,  //   HSI_96MHz,
    SYSCLK_HSE_1x,     //   HSE_8MHz,
    SYSCLK_HSE_3x,     //   HSE_24MHz,
    SYSCLK_HSE_4x,     //   HSE_36MHz,
    SYSCLK_HSE_6x,     //   HSE_48MHz,
    SYSCLK_HSE_7x,     //   HSE_56MHz,
    SYSCLK_HSE_9x,     //   HSE_72MHz,
    SYSCLK_HSE_12x,    //   HSE_96MHz,
#endif
#if defined(__MM0Q1)
    SYSCLK_HSI_6d,     //   HSI_6d,
    SYSCLK_HSE_1x,     //   HSE_1x,             //  HSE_8MHz-24MHz,
    SYSCLK_HSI_48MHz,  //   HSI_48MHz,
    SYSCLK_HSI_72MHz,  //   HSI_72MHz,
    SYSCLK_LSI_40KHz   //   LSI_40kHz
#endif
} EM_SystemClock;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
#ifdef _SYSTEM_MM32_C_
#define GLOBAL





GLOBAL int nTimeOutCnt = 0;

GLOBAL bool sysTickFlag1mS = false;
GLOBAL u32 sysTickCnt1mS = 0;

GLOBAL bool sysTickFlag = false;
GLOBAL u32 sysTickCnt = 0;

GLOBAL bool tickFlag = false;
GLOBAL u32 tickCnt = 0;

#else
#define GLOBAL extern

GLOBAL bool sysTickFlag1mS;
GLOBAL u32 sysTickCnt1mS;
GLOBAL int nTimeOutCnt;
GLOBAL bool sysTickFlag;
GLOBAL u32 sysTickCnt;
GLOBAL bool tickFlag;
GLOBAL u32 tickCnt;


GLOBAL u16 g_KEY_buffer[8];




#endif

GLOBAL bool sysTickFlag;
GLOBAL u32 sysTickCnt;
GLOBAL EM_MCUID MCUID;

//GLOBAL HANDLE hSysDisplay;
//GLOBAL HANDLE hSysKeyBoard;


#undef GLOBAL

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{


typedef void (*AppTick_fun)(void);



//void PutKeyBuffer(u8 key);


void SysTickLED(void);

EM_MCUID DBGMCU_GetDEVID(void);
EM_MCUID SystemInit(EM_SystemClock ClockSoucre, EM_SYSTICK enable , u32* address);
EM_MCUID SetSystemClock(EM_SYSTICK enable , u32* callbackPtr);

/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
