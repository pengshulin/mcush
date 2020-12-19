////////////////////////////////////////////////////////////////////////////////
/// @file    RESOURCE.C
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
#define _RESOURCE_C_

// Files includes  -------------------------------------------------------------
#include <string.h>
#include "types.h"

#include "drv.h"

#include "hal_rcc.h"
#include "bsp.h"

#include "resource.h"


////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_RESOURCE
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Exported_Functions
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief  Key 0 call back process function.
/// @param : None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void KeyProcess_Key0()
{
    keyStatus = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Key 1 call back process function.
/// @param : None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void KeyProcess_Key1()
{
    keyStatus = 1;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Key 2 call back process function.
/// @param : None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void KeyProcess_Key2()
{
    keyStatus = 2;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Key 3 call back process function.
/// @param : None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void KeyProcess_Key3()
{
    keyStatus = 3;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Init the user resource.
/// @param  tickEn :Enable or disable the systick.
/// @param  pCallback : The pointer point to the systick callback function.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void UserResource(EM_SYSTICK tickEn, u32* pCallback)
{
    if (tickEn) {
        sysTickFlag = false;
        sysTickCnt  = 0;
        AppTickPtr = pCallback;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Init the system resource.
/// @param  tickEn :Enable or disable the systick.
/// @param  pCallback : The pointer point to the systick callback function.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SysResource(EM_SYSTICK tickEn, u32* pCallback)
{
#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)           // || defined(__USER_RESOURCE)
    if (tickEn) {
        sysTickFlag = false;
        sysTickCnt  = 0;
        AppTickPtr = pCallback;

        /*  System Display */
        hDisplay = CreateFile((EM_IP)emIP_DISPLAY);
        if (hDisplay == NULL)       while(1);

        tAPP_DISPLAY_DCB dcbDisp = {
            .hSub       = 0                     //
//          .status     = 0
        };
        if (!OpenFile(hDisplay, (void*)&dcbDisp)) while(1);

        /*  System Keyboard */
        hKeyboard = CreateFile((EM_IP)emIP_KEYBOARD);
        if (hKeyboard == NULL)      while(1);

        tAPP_KEYBOARD_DCB dcbKey = {
            .hSub       = 0                     //
//          .attrib     = false                 //
        };
        if (!OpenFile(hKeyboard, (void*)&dcbKey)) while(1);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  System LED display funtion.
/// @param  vd : The pointer point to LED variable.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SysDisplay(u8* vd)
{
#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)           // || defined(__USER_RESOURCE)
    WriteFile(hDisplay, 0, vd, 1);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  System keyboard get funtion.
/// @param  vk : The pointer point to keyboard variable.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
bool SysKeyboard(u8* vk)
{
#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)           // || defined(__USER_RESOURCE)
    *vk = 0x00;
    return ReadFile(hKeyboard, 0, vk, 1);
#else
    return false;
#endif
}


/// @}


/// @}

/// @}



