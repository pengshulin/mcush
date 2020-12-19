////////////////////////////////////////////////////////////////////////////////
/// @file     HAL_RCC.H
/// @author   S Yi
/// @version  v2.0.0
/// @date     2019-03-13
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE RCC
///           FIRMWARE LIBRARY.
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
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#ifndef __HAL_RCC_H
#define __HAL_RCC_H

// Includes  -------------------------------------------------------------------
#include "types.h"
#include "mm32.h"
#include "system_mm32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_HAL
/// @brief RCC HAL modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_Exported_Types
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_Exported_Constants
/// @{
#define HSE_STARTUP_TIMEOUT     (0x0500U)                                       ///< Time out for HSE start up.

#ifndef HSE_VALUE
    #define HSE_VALUE       0   /* HSE not used, HSI only */
#endif

//#ifdef CLOCK_12MHz
//    #define HSE_VALUE               (12000000U)                                     ///< Value of the External oscillator in Hz.
//#else
//    #define HSE_VALUE               (8000000U)                                      ///< Value of the External oscillator in Hz.
//#endif



#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1) || defined(__MM0P1)
#define HSI_VALUE_PLL_ON        (48000000U / 4)                                 ///< Value of the Internal oscillator in Hz.
#define HSI_DIV6                (48000000U / 6)                                 ///< Value of the Internal oscillator in Hz.
#endif

//#define HSI_VALUE_PLL_ON      (72000000 / 4)                                  ///< Value of the Internal
// oscillator in Hz. #define HSI_VALUE_PLL_OFF      (72000000 / 6)              ///<
// Value of the Internal oscillator in Hz.

#if defined(__MM0Q1)
#define HSI_72MHz               (72000000U)                                     ///< Value of the Internal oscillator in Hz.
#define HSI_48MHz               (48000000U)                                     ///< Value of the Internal oscillator in Hz.

#define HSI_72MHz_DIV6          (12000000U)                                     ///< Value of the Internal oscillator in Hz.
#define HSI_48MHz_DIV6          (8000000U)                                      ///< Value of the Internal oscillator in Hz.

#endif

#if defined(__MM0P1) || defined(__MM0Q1)
#define LSI_VALUE               (40000U)                                        ///< Value of the Internal oscillator in Hz.
#endif


/// @}



////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_Exported_Enumeration
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @brief HSE configuration
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_HSE_OFF    = 0,             // HSE OFF
    RCC_HSE_ON     = RCC_CR_HSEON,  // HSE ON
    RCC_HSE_Bypass = RCC_CR_HSEBYP  // HSE Bypass
} RCCHSE_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief Used for flags
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    CR_REG_INDEX   = 1,     //
    BDCR_REG_INDEX = 2,     //
    CSR_REG_INDEX  = 3,     //
    RCC_FLAG_MASK  = 0x1FU  //
} RCC_RegisterFlag_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RCC Flag
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    // Flags in the CR register
    RCC_FLAG_HSIRDY = ((u8)((CR_REG_INDEX << 5U) | RCC_CR_HSIRDY_Pos)),         ///< Internal High Speed clock ready flag
    RCC_FLAG_HSERDY = ((u8)((CR_REG_INDEX << 5U) | RCC_CR_HSERDY_Pos)),         ///< External High Speed clock ready flag

#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)                   //|| defined(__MM3O1)
    RCC_FLAG_PLLRDY = ((u8)((CR_REG_INDEX << 5U) | RCC_CR_PLLRDY_Pos)),         ///< PLL clock ready flag
#else
    RCC_FLAG_PLLRDY     = 0,
#endif

    // Flags in the CSR register
    RCC_FLAG_LSIRDY  = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_LSIRDY_Pos)),      ///< Internal Low Speed oscillator Ready
    RCC_FLAG_PINRST  = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_PINRSTF_Pos)),     ///< PIN reset flag
    RCC_FLAG_PORRST  = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_PORRSTF_Pos)),     ///< POR/PDR reset flag
    RCC_FLAG_SFTRST  = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_SFTRSTF_Pos)),     ///< Software Reset flag
    RCC_FLAG_IWDGRST = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_IWDGRSTF_Pos)),    ///< Independent Watchdog reset flag
    RCC_FLAG_WWDGRST = ((u8)((CSR_REG_INDEX << 5U) | RCC_CSR_WWDGRSTF_Pos)),    ///< Window watchdog reset flag

#if defined(__MM3N1)
    // Flags in the BDCR register
    RCC_FLAG_LSERDY = ((u8)((BDCR_REG_INDEX << 5U) | RCC_BDCR_LSERDY_Pos))      ///< External Low Speed oscillator Ready
#endif
} RCC_FLAG_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief System clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_HSI = 0,  // Set HSI as systemclock
    RCC_HSE = 1,  // Set HSE as systemclock
#if defined(__MM3N1) || defined(__MM0N1) || defined (__MM0P1)
    RCC_PLL = 2,   // Set PLL as systemclock
#endif
#if defined (__MM0P1) || defined (__MM0Q1)
    RCC_LSI = 3  // Set LSI as systemclock
#endif
} SYSCLK_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief PLL entry clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_HSI_Div4 = 0,
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)  //|| defined(__MM3O1)
    RCC_HSE_Div1 = RCC_CFGR_PLLSRC,
    RCC_HSE_Div2 = (RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC)
#else
    RCC_HSE_Div1        = 0,
    RCC_HSE_Div2        = 0
#endif
} RCC_PLLSource_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief PLL multiplication factor
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
typedef enum {
    RCC_PLLMul_2  = 0x00000000U,
    RCC_PLLMul_3  = 0x00040000U,
    RCC_PLLMul_4  = 0x00080000U,
    RCC_PLLMul_5  = 0x000C0000U,
    RCC_PLLMul_6  = 0x00100000U,
    RCC_PLLMul_7  = 0x00140000U,
    RCC_PLLMul_8  = 0x00180000U,
    RCC_PLLMul_9  = 0x001C0000U,
    RCC_PLLMul_10 = 0x00200000U,
    RCC_PLLMul_11 = 0x00240000U,
    RCC_PLLMul_12 = 0x00280000U,
    RCC_PLLMul_13 = 0x002C0000U,
    RCC_PLLMul_14 = 0x00300000U,
    RCC_PLLMul_15 = 0x00340000U,
    RCC_PLLMul_16 = 0x00380000U
} RCC_PLLMul_TypeDef;
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief AHB clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_SYSCLK_Div1   = RCC_CFGR_HPRE_DIV1,
    RCC_SYSCLK_Div2   = RCC_CFGR_HPRE_DIV2,
    RCC_SYSCLK_Div4   = RCC_CFGR_HPRE_DIV4,
    RCC_SYSCLK_Div8   = RCC_CFGR_HPRE_DIV8,
    RCC_SYSCLK_Div16  = RCC_CFGR_HPRE_DIV16,
    RCC_SYSCLK_Div64  = RCC_CFGR_HPRE_DIV64,
    RCC_SYSCLK_Div128 = RCC_CFGR_HPRE_DIV128,
    RCC_SYSCLK_Div256 = RCC_CFGR_HPRE_DIV256,
    RCC_SYSCLK_Div512 = RCC_CFGR_HPRE_DIV512
} RCC_AHB_CLK_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief APB1 and APB2clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_HCLK_Div1  = RCC_CFGR_PPRE1_DIV1,
    RCC_HCLK_Div2  = RCC_CFGR_PPRE1_DIV2,
    RCC_HCLK_Div4  = RCC_CFGR_PPRE1_DIV4,
    RCC_HCLK_Div8  = RCC_CFGR_PPRE1_DIV8,
    RCC_HCLK_Div16 = RCC_CFGR_PPRE1_DIV16
} RCC_APB1_APB2_CLK_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief USB Device clock source
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1) || defined(__MM0N1)
typedef enum {
    RCC_USBCLKSource_PLLCLK_Div1 = 0,
    RCC_USBCLKSource_PLLCLK_Div2 = 1,
    RCC_USBCLKSource_PLLCLK_Div3 = 2,
    RCC_USBCLKSource_PLLCLK_Div4 = 3
} RCC_USBCLKSOURCE_TypeDef;
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief ADC clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_PCLK2_Div2 = (0x00000000),
    RCC_PCLK2_Div4 = (0x00004000),
    RCC_PCLK2_Div6 = (0x00008000),
    RCC_PCLK2_Div8 = (0x0000C000)
} RCC_ADCCLKSOURCE_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief LSE configuration
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)  //|| defined(__MM3O1)
typedef enum {
    RCC_LSE_OFF    = 0,               // LSE OFF
    RCC_LSE_ON     = RCC_BDCR_LSEON,  // LSE ON
    RCC_LSE_Bypass = RCC_BDCR_LSEBYP  // LSE Bypass
} RCC_LSE_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RTC clock source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_RTCCLKSource_LSE        = RCC_BDCR_RTCSEL_LSE,
    RCC_RTCCLKSource_LSI        = RCC_BDCR_RTCSEL_LSI,
    RCC_RTCCLKSource_HSE_Div128 = RCC_BDCR_RTCSEL_HSE
} RCC_RTCCLKSOURCE_TypeDef;
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief Clock source to output on MCO pin
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_MCO_NoClock = RCC_CFGR_MCO_NOCLOCK,
    RCC_MCO_LSI     = RCC_CFGR_MCO_LSI,
#if defined(__MM3N1)
    RCC_MCO_LSE = RCC_CFGR_MCO_LSE,
#else
    RCC_MCO_LSE         = 0,
#endif
    RCC_MCO_SYSCLK = RCC_CFGR_MCO_SYSCLK,
    RCC_MCO_HSI    = RCC_CFGR_MCO_HSI,
    RCC_MCO_HSE    = RCC_CFGR_MCO_HSE,
#if defined(__MM3N1) || defined(__MM0N1)|| defined(__MM0P1)
    RCC_MCO_PLLCLK_Div2 = RCC_CFGR_MCO_PLL
#else
    RCC_MCO_PLLCLK_Div2 = 0
#endif
} RCC_MCO_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RCC Interrupt source
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    RCC_IT_LSIRDY = RCC_CIR_LSIRDYF,
#if defined(__MM3N1)  //|| defined(__MM3O1)
    RCC_IT_LSERDY = RCC_CIR_LSERDYF,
#else
    RCC_IT_LSERDY       = 0,
#endif
    RCC_IT_HSIRDY = RCC_CIR_HSIRDYF,
    RCC_IT_HSERDY = RCC_CIR_HSERDYF,
#if defined(__MM3N1) || defined(__MM0N1)|| defined(__MM0P1)
    RCC_IT_PLLRDY = RCC_CIR_PLLRDYF,
#else
    RCC_IT_PLLRDY       = 0,
#endif
    RCC_IT_CSS = RCC_CIR_CSSF
} RCC_IT_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RCC clock frequency type definition
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    u32 SYSCLK_Frequency;                                                       ///< returns SYSCLK clock frequency.
    u32 HCLK_Frequency;                                                         ///< returns HCLK clock frequency.
    u32 PCLK1_Frequency;                                                        ///< returns PCLK1 clock frequency.
    u32 PCLK2_Frequency;                                                        ///< returns PCLK2 clock frequency.
    u32 ADCCLK_Frequency;                                                       ///< returns ADCCLK clock frequency.
} RCC_ClocksTypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RCC InitStruct Types
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    EM_SystemClock              RCC_SystemClock;                                ///< RCC system clock
    u8                          RCC_PrescaleAHB;                                ///< AHB prescale
    u8                          RCC_PrescaleAPB1;                               ///< APB1 prescale
    u8                          RCC_PrescaleAPB2;                               ///< APB2 prescale
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM3O1)
    u8 RCC_PrescaleUSB;                                                         ///< USB prescale
#endif
    bool                        SysTickEN;                                      ///< Systick enable
    u32                         SysTickPeriod;                                  ///< Systick period
} RCCInitStruct_TypeDef;

////////////////////////////////////////////////////////////////////////////////
/// @brief RCC Current configuration
////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
typedef enum {
    RCC_CONFIG_OSC_ITRIM_2mA = (0x00000000U),
    RCC_CONFIG_OSC_ITRIM_4mA = (0x00000800U),
    RCC_CONFIG_OSC_ITRIM_6mA = (0x00001000U),
    RCC_CONFIG_OSC_ITRIM_8mA = (0x00001800U)
} RCC_CONFIGOSCITRIM_TypeDef;
#endif

/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_Exported_Variables
/// @{
#ifdef _HAL_RCC_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup RCC_Exported_Functions
/// @{
void RCC_DeInit(void);
void RCC_HSEConfig(RCCHSE_TypeDef state);
void RCC_HSICmd(FunctionalState state);
void RCC_SYSCLKConfig(SYSCLK_TypeDef sysClkSrc);
#if defined(__MM3N1) || defined(__MM0N1) || defined(__MM0P1)
void RCC_PLLDMDNConfig(u32 plldn, u32 plldm);
void RCC_PLLConfig(RCC_PLLSource_TypeDef pllSrc, RCC_PLLMul_TypeDef pllMul);
void RCC_PLLCmd(FunctionalState state);
#endif
void RCC_HCLKConfig(RCC_AHB_CLK_TypeDef sysClk);
void RCC_PCLK1Config(RCC_APB1_APB2_CLK_TypeDef HCLK);
void RCC_PCLK2Config(RCC_APB1_APB2_CLK_TypeDef HCLK);
#if defined(__MM3N1) || defined(__MM0N1)
void RCC_USBCLKConfig(RCC_USBCLKSOURCE_TypeDef usbClkSrc);
#endif
void RCC_ADCCLKConfig(RCC_ADCCLKSOURCE_TypeDef PCLK2);
void RCC_LSICmd(FunctionalState state);

#if defined(__MM3N1)  //|| defined(__MM3O1)
void RCC_RTCCLKCmd(FunctionalState state);
void RCC_LSEConfig(RCC_LSE_TypeDef state);
void RCC_RTCCLKConfig(RCC_RTCCLKSOURCE_TypeDef rtcClkSrc);
void RCC_BackupResetCmd(FunctionalState state);
#endif

void RCC_GetClocksFreq(RCC_ClocksTypeDef* clk);
void RCC_AHBPeriphClockCmd(u32 AHBPeriph, FunctionalState state);
void RCC_APB2PeriphClockCmd(u32 APB2Periph, FunctionalState state);
void RCC_APB1PeriphClockCmd(u32 APB1Periph, FunctionalState state);
void RCC_APB2PeriphResetCmd(u32 APB2Periph, FunctionalState state);
void RCC_APB1PeriphResetCmd(u32 APB1Periph, FunctionalState state);

void RCC_ClockSecuritySystemCmd(FunctionalState state);
void RCC_MCOConfig(RCC_MCO_TypeDef mcoSrc);
void RCC_ClearFlag(void);
void RCC_ITConfig(RCC_IT_TypeDef it, FunctionalState state);
void RCC_ClearITPendingBit(u8 it);

u8  RCC_GetSYSCLKSource(void);
u32 RCC_GetSysClockFreq(void);
u32 RCC_GetHCLKFreq(void);

u32 RCC_GetPCLK1Freq(void);
u32 RCC_GetPCLK2Freq(void);

FlagStatus  RCC_GetFlagStatus(RCC_FLAG_TypeDef flag);
ErrorStatus RCC_WaitForHSEStartUp(void);
ErrorStatus RCC_WaitForFlagStartUp(RCC_FLAG_TypeDef flag);
ITStatus    RCC_GetITStatus(RCC_IT_TypeDef it);

////////////////////////////////////////////////////////////////////////////////
//          Extended function interface
////////////////////////////////////////////////////////////////////////////////
ErrorStatus exRCC_Init(RCCInitStruct_TypeDef* para);
void exRCC_SystickDisable(void);
void exRCC_SystickEnable(u32 sysTickPeriod);
void exRCC_APB1PeriphReset(u32 RCC_APB1Periph);
void exRCC_APB2PeriphReset(u32 RCC_APB2Periph);
void exRCC_BackupReset(void);
#if defined(__MM3O1) || defined(__MM0P1) || defined(__MM0Q1)
void exRCC_Set_OSC_ITRIM_Config(u32 val);
#endif
/// @}

/// @}

/// @}




////////////////////////////////////////////////////////////////////////////////
#endif /* __HAL_RCC_H */
////////////////////////////////////////////////////////////////////////////////


