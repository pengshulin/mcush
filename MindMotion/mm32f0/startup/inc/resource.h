////////////////////////////////////////////////////////////////////////////////
/// @file    RESOURCE.H
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
#ifndef __RESOURCE_H
#define __RESOURCE_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "drv.h"


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURE
/// @brief MM32 Examples resource modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Constants
/// @{

#define VK_K0           0
#define VK_K1           1
#define VK_K2           2
#define VK_K3           3



////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
#ifdef _RESOURCE_C_
#define GLOBAL


GLOBAL u32 keyStatus = 0;

#else
#define GLOBAL extern

GLOBAL u32 keyStatus;


#endif

//#if defined(__SYS_RESOURCE_IT) || defined(__SYS_RESOURCE_POLLING)
    GLOBAL HANDLE hDisplay;
    GLOBAL HANDLE hKeyboard;
    GLOBAL u8 vkKey;
    GLOBAL u8 vdLED;
//#endif


GLOBAL u32* AppTickPtr;

#undef GLOBAL

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{

void UserResource(EM_SYSTICK tickEn, u32* pCallback);
void SysResource(EM_SYSTICK tickEn, u32* pCallback);

bool SysKeyboard(u8* vk);
void SysDisplay(u8* vd);

void KeyProcess_Key0(void);
void KeyProcess_Key1(void);
void KeyProcess_Key2(void);
void KeyProcess_Key3(void);



/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif /* __RESOURCE_H */
////////////////////////////////////////////////////////////////////////////////




