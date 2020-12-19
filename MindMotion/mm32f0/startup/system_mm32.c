////////////////////////////////////////////////////////////////////////////////
/// @file    SYSTEM_MM32.C
/// @author  AE TEAM
/// @version 2.0.0
/// @date    2018-08-01
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
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
#define _SYSTEM_MM32_C_

// Files includes  -------------------------------------------------------------
#include <string.h>
#include "types.h"
#include "mm32.h"

//#include "drv.h"
#include "system_mm32.h"

#include "HAL_rcc.h"
//#include "bsp.h"

//#include "resource.h"


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_RESOURCE
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Exported_Constants
/// @{

#if defined(__MM3N1)
//  #define VECT_TAB_SRAM
    #define VECT_TAB_OFFSET  0x0000
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
//void SysTick_Handler(void)
//{
//    nTimeOutCnt++;
//    sysTickFlag1mS = true;
//
//    if (sysTickCnt++ >= 20) {
//        sysTickCnt  = 0;
//        sysTickFlag = true;
//#if defined(__SYS_RESOURCE_IT)
//        SysDisplay((u8*)&vdLED);
//#endif
//    }
//
//#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)
//    u8 vk = 0xFF;
//    ReadFile(hKeyboard, 0, (u8*)&vk, 1);
//#endif
//
//    if (AppTickPtr != NULL) {
//        ((AppTick_fun)AppTickPtr)();
//    }
//}

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function get the MCU ID.
/// @param  None.
/// @retval  MCU ID.
////////////////////////////////////////////////////////////////////////////////
EM_MCUID DBGMCU_GetDEVID()
{
    // Automatic Identification
    if ((SCB->CPUID & COREID_MASK) == 0) {
        switch (((DBGMCU_TypeDef*)0x40013400)->IDCODE ) {
            case MCUID_MM0N1:
                return emMCUID_MM32L073;        // emMCUID_MM0N1;
            case MCUID_MM0P1:
            case MCUID_MM0P2:
                return emMCUID_MM32SPIN27;      // emMCUID_MM0P1;
            case MCUID_MM0Q1:
                return emMCUID_MM32F031;        // emMCUID_MM0Q1;
            case MCUID_MM0M1:
                return emMCUID_MM32F031_OLD;    // emMCUID_MM0M1;
            default: break;
        }
    }
    else if ((SCB->CPUID & COREID_MASK) == 0x0030) {
        RCC->APB1ENR = 0;
        RCC->APB1ENR |= RCC_APB1ENR_PWR;
        if (((DBGMCU_TypeDef*)0x40007080)->IDCODE == MCUID_MM3O1)
            return emMCUID_MM32L395;            // emMCUID_MM3O1;
        else {
            RCC->APB1ENR |= RCC_APB1ENR_PWR;
            if (((DBGMCU_TypeDef*)0x40007080)->IDCODE == MCUID_MM3N1)
                return emMCUID_MM32L373;        // emMCUID_MM3N1;
            else if (((DBGMCU_TypeDef*)0xE0042000)->IDCODE == MCUID_MM3M1)
                return emMCUID_MM32F103_OLD;    // emMCUID_MM3M1;
        }
    }
    return Unknown;
}

////////////////////////////////////////////////////////////////////////////////
// @brief  Setup the microcontroller system
//         Initialize the Embedded Flash Interface, the PLL and update the
//         SystemCoreClock variable.
// @note   This function should be used only after reset.
// @param  ClockSoucre : Select system clock source.
// @param  tickEn : Enable or disable the systick.
// @param  pCallback : The pointer point to the systick callback function.
// @retval  MCU ID.
////////////////////////////////////////////////////////////////////////////////
EM_MCUID SystemInit(EM_SystemClock ClockSoucre, EM_SYSTICK tickEn , u32* pCallback)
{
    (void)pCallback;

    // Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers
    // Configure the Flash Latency cycles and enable prefetch buffer

    RCCInitStruct_TypeDef  RCC_InitStructure;
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
                                                                //  SYSCLK_HSE_12x;     // HSE_96MHz@8MHz
    RCC_InitStructure.RCC_SystemClock  = ClockSoucre;           //  SYSCLK_HSE_9x;      // HSE_72MHz@8MHz
#endif
#if defined(__MM0Q1)
    RCC_InitStructure.RCC_SystemClock  = ClockSoucre;           //  SYSCLK_HSI_48MHz;
#endif

    RCC_InitStructure.RCC_PrescaleAHB  = 1;                     // Range: 0-7:8-15; div1-div512
    RCC_InitStructure.RCC_PrescaleAPB1 = 1;                     // Range: 0:15; div1:div16
    RCC_InitStructure.RCC_PrescaleAPB2 = 1;                     // Range: 0:15; div1:div16
#if defined(__MM3N1) || defined(__MM0N1)
    RCC_InitStructure.RCC_PrescaleUSB  = 1;                     // Range: 0:1;  div1:div2
#endif
    RCC_InitStructure.SysTickEN = tickEn;
    RCC_InitStructure.SysTickPeriod = 1000;

    if (exRCC_Init(&RCC_InitStructure) == ERROR) {
        while(1) {
            // error
        }
    }

#if defined(__MM3N1)
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
#endif

#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)
    SysResource(tickEn, pCallback);
#endif

#if defined(__USER_RESOURCE)
    UserResource(tickEn, pCallback);
#endif

    return DBGMCU_GetDEVID();
}


////////////////////////////////////////////////////////////////////////////////
//  @brief  System clock configuration
//  @param  enable: Enable or disable the systick.
//  @param  callbackPtr: The pointer point to the systick callback function.
//  @retval None.
////////////////////////////////////////////////////////////////////////////////
EM_MCUID SetSystemClock(EM_SYSTICK enable , u32* callbackPtr)
{
#if defined(__EX_AES)
    return SystemInit(SYSCLK_HSI_72MHz, enable, callbackPtr);
#else
    #if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
        return SystemInit(SYSCLK_HSI_96MHz, enable, callbackPtr);
//      return SystemInit(SYSCLK_HSI_72MHz, enable, callbackPtr);
    #endif

    #if defined(__MM0Q1)
        return  SystemInit(SYSCLK_HSI_48MHz, enable, callbackPtr);

    #endif

#endif
}

/// @}


/// @}

/// @}


