////////////////////////////////////////////////////////////////////////////////
/// @file    STARTUP_MM32.H
/// @author  AE TEAM
/// @version V2.0.0
/// @date    2018-08-01
/// @brief   THIS FILE PROVIDES ALL THE STARTUP FIRMWARE FUNCTIONS.
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
#ifndef __STARTUP_MM32_H
#define __STARTUP_MM32_H
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "mm32.h"


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURCE
/// @brief MM32 Examples resource modules
/// @{



typedef void (*intfunc)(void);

typedef union {
    intfunc __fun;
    void * __ptr;
} intvec_elem;

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_struct
/// @{

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Device Vectors struct definition.
////////////////////////////////////////////////////////////////////////////////
typedef struct _DeviceVectors {
// Stack pointer
    void* pvStack;

// Cortex-M handlers
    void* pfnReset_Handler;                         ///< Reset Handler
    void* pfnNMI_Handler;                           ///< NMI Handler
    void* pfnHardFault_Handler;                     ///< Hard Fault Handler
    void* pfnMemManage_Handler;                     ///< MPU Fault Handler
    void* pfnBusFault_Handler;                      ///< Bus Fault Handler
    void* pfnUsageFault_Handler;                    ///< Usage Fault Handler
    void* pfnReserved1_Handler;                     ///< Reserved
    void* pfnReserved2_Handler;                     ///< Reserved
    void* pfnReserved3_Handler;                     ///< Reserved
    void* pfnReserved4_Handler;                     ///< Reserved
    void* pfnSVC_Handler;                           ///< SVCall Handler
    void* pfnDebugMon_Handler;                      ///< Debug Monitor Handler
    void* pfnReserved5_Handler;                     ///< Reserved
    void* pfnPendSV_Handler;                        ///< PendSV Handler
    void* pfnSysTick_Handler;                       ///< SysTick Handler

// Peripheral handlers
    void* pfnWWDG_IRQHandler;                       ///< Window Watchdog
    void* pfnPVD_IRQHandler ;                       ///< PVD through EXTI Line detect
    void* pfnTAMPER_IRQHandler;                     ///< Tamper
    void* pfnRTC_IRQHandler;                        ///< RTC
    void* pfnFLASH_IRQHandler;                      ///< Flash
    void* pfnRCC_IRQHandler;                        ///< RCC
    void* pfnEXTI0_IRQHandler;                      ///< EXTI Line 0
    void* pfnEXTI1_IRQHandler;                      ///< EXTI Line 1
    void* pfnEXTI2_IRQHandler;                      ///< EXTI Line 2
    void* pfnEXTI3_IRQHandler;                      ///< EXTI Line 3
    void* pfnEXTI4_IRQHandler;                      ///< EXTI Line 4
    void* pfnDMA1_Channel1_IRQHandler;              ///< DMA1 Channel 1
    void* pfnDMA1_Channel2_IRQHandler;              ///< DMA1 Channel 2
    void* pfnDMA1_Channel3_IRQHandler;              ///< DMA1 Channel 3
    void* pfnDMA1_Channel4_IRQHandler;              ///< DMA1 Channel 4
    void* pfnDMA1_Channel5_IRQHandler;              ///< DMA1 Channel 5
    void* pfnDMA1_Channel6_IRQHandler;              ///< DMA1 Channel 6
    void* pfnDMA1_Channel7_IRQHandler;              ///< DMA1 Channel 7
    void* pfnADC1_2_IRQHandler;                     ///< ADC1 & ADC2
    void* pfnUSB_HP_CAN1_TX_IRQHandler;             ///< USB High Priority or CAN1 TX
    void* pfnReserved6_Handler;
    void* pfnCAN1_RX1_IRQHandler;                   ///< CAN1 RX1
    void* pfnReserved7_Handler;
    void* pfnEXTI9_5_IRQHandler;                    ///< EXTI Line 9..5
    void* pfnTIM1_BRK_IRQHandler;                   ///< TIM1 Break
    void* pfnTIM1_UP_IRQHandler;                    ///< TIM1 Update
    void* pfnTIM1_TRG_COM_IRQHandler;               ///< TIM1 Trigger and Commutation
    void* pfnTIM1_CC_IRQHandler;                    ///< TIM1 Capture Compare
    void* pfnTIM2_IRQHandler;                       ///< TIM2
    void* pfnTIM3_IRQHandler;                       ///< TIM3
    void* pfnTIM4_IRQHandler;                       ///< TIM4
    void* pfnI2C1_EV_IRQHandler;                    ///< I2C1 Event
    void* pfnReserved8_Handler;
    void* pfnI2C2_EV_IRQHandler;                    ///< I2C2 Event
    void* pfnReserved9_Handler;
    void* pfnSPI1_IRQHandler;                       ///< SPI1
    void* pfnSPI2_IRQHandler;                       ///< SPI2
    void* pfnUART1_IRQHandler;                      ///< UART1
    void* pfnUART2_IRQHandler;                      ///< UART2
    void* pfnUART3_IRQHandler;                      ///< UART3
    void* pfnEXTI15_10_IRQHandler;                  ///< EXTI Line 15..10
    void* pfnRTCAlarm_IRQHandler;                   ///< RTC Alarm through EXTI Line
    void* pfnUSBWakeUp_IRQHandler;                  ///< USB Wakeup from suspend
    void* pfnReserved10_Handler;
    void* pfnReserved11_Handler;
    void* pfnAES_IRQHandler;
} DeviceVectors;
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0N1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Device Vectors struct definition.
////////////////////////////////////////////////////////////////////////////////
typedef struct _DeviceVectors {
// Stack pointer
    void* pvStack;
    void* pfnReset_Handler;                         ///< Reset Handler
    void* pfnNMI_Handler;                           ///< NMI Handler
    void* pfnHardFault_Handler;                     ///< Hard Fault Handler
    void* pfnReserved1_Handler;                     ///< Reserved
    void* pfnReserved2_Handler;                     ///< Reserved
    void* pfnReserved3_Handler;                     ///< Reserved
    void* pfnReserved4_Handler;                     ///< Reserved
    void* pfnReserved5_Handler;                     ///< Reserved
    void* pfnReserved6_Handler;                     ///< Reserved
    void* pfnReserved7_Handler;                     ///< Reserved
    void* pfnSVC_Handler;                           ///< SVCall Handler
    void* pfnReserved8_Handler;                     ///< Reserved
    void* pfnReserved9_Handler;                     ///< Reserved
    void* pfnPendSV_Handler;                        ///< PendSV Handler
    void* pfnSysTick_Handler;                       ///< SysTick Handler

    void* pfnWWDG_IRQHandler;                       ///< Window Watchdog
    void* pfnPVD_IRQHandler;                        ///< PVD through EXTI Line detect
    void* pfnReserved10_Handler;                    ///< Reserved
    void* pfnFLASH_IRQHandler;                      ///< FLASH
    void* pfnRCC_CRS_IRQHandler;                    ///< RCC & CRS
    void* pfnEXTI0_1_IRQHandler;                    ///< EXTI Line 0 and 1
    void* pfnEXTI2_3_IRQHandler;                    ///< EXTI Line 2 and 3
    void* pfnEXTI4_15_IRQHandler;                   ///< EXTI Line 4 to 15
    void* pfnReserved11_Handler;                    ///< Reserved
    void* pfnDMA1_Channel1_IRQHandler;              ///< DMA1 Channel 1
    void* pfnDMA1_Channel2_3_IRQHandler;            ///< DMA1 Channel 2 and Channel 3
    void* pfnDMA1_Channel4_5_IRQHandler;            ///< DMA1 Channel 4 and Channel 5
    void* pfnADC1_COMP_IRQHandler;                  ///< ADC1 & COMP
    void* pfnTIM1_BRK_UP_TRG_COM_IRQHandler;        ///< TIM1 Break, Update, Trigger and Commutation
    void* pfnTIM1_CC_IRQHandler;                    ///< TIM1 Capture Compare
    void* pfnTIM2_IRQHandler;                       ///< TIM2
    void* pfnTIM3_IRQHandler;                       ///< TIM3
    void* pfnReserved12_Handler;                    ///< Reserved
    void* pfnReserved13_Handler;                    ///< Reserved
    void* pfnTIM14_IRQHandler;                      ///< TIM14
    void* pfnReserved14_Handler;                    ///< Reserved
    void* pfnTIM16_IRQHandler;                      ///< TIM16
    void* pfnTIM17_IRQHandler;                      ///< TIM17
    void* pfnI2C1_IRQHandler;                       ///< I2C1
    void* pfnReserved15_Handler;                    ///< Reserved
    void* pfnSPI1_IRQHandler;                       ///< SPI1
    void* pfnSPI2_IRQHandler;                       ///< SPI2
    void* pfnUART1_IRQHandler;                      ///< UART1
    void* pfnUART2_IRQHandler;                      ///< UART2
    void* pfnAES_IRQHandler;                        ///< AES
    void* pfnCAN_IRQHandler;                        ///< CAN
    void* pfnUSB_IRQHandler;                        ///< USB
} DeviceVectors;
#endif


////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0P1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Device Vectors struct definition.
////////////////////////////////////////////////////////////////////////////////
typedef struct _DeviceVectors {
// Stack pointer
    void* pvStack;
    void* pfnReset_Handler;                         ///< Reset Handler
    void* pfnNMI_Handler;                           ///< NMI Handler
    void* pfnHardFault_Handler;                     ///< Hard Fault Handler
    void* pfnMemManage_Handler;                     ///< MPU Fault Handler
    void* pfnBusFault_Handler;                      ///< Bus Fault Handler
    void* pfnUsageFault_Handler;                    ///< Usage Fault Handler
    void* pfnReserved1_Handler;                     ///< Reserved
    void* pfnReserved2_Handler;                     ///< Reserved
    void* pfnReserved3_Handler;                     ///< Reserved
    void* pfnReserved4_Handler;                     ///< Reserved
    void* pfnSVC_Handler;                           ///< SVCall Handler
    void* pfnDebugMon_Handler;                      ///< Debug Monitor Handler
    void* pfnReserved5_Handler;                     ///< Reserved
    void* pfnPendSV_Handler;                        ///< PendSV Handler
    void* pfnSysTick_Handler;                       ///< SysTick Handler

    void* pfnWWDG_IWDG_IRQHandler;                  ///< Window Watchdog
    void* pfnPVD_IRQHandler;                        ///< PVD through EXTI Line detect
    void* pfnPWM_IRQHandler;                        ///< Commutation and input error interrupt
    void* pfnFLASH_IRQHandler;                      ///< FLASH
    void* pfnRCC_IRQHandler;                        ///< RCC
    void* pfnEXTI0_1_IRQHandler;                    ///< EXTI Line 0 and 1
    void* pfnEXTI2_3_IRQHandler;                    ///< EXTI Line 2 and 3
    void* pfnEXTI4_15_IRQHandler;                   ///< EXTI Line 4 to 15
    void* pfnHWDIV_IRQHandler;                      ///< HWDIV
    void* pfnDMA1_Channel1_IRQHandler;                  ///< DMA1 Channel 1
    void* pfnDMA1_Channel2_3_IRQHandler;                ///< DMA1 Channel 2 and Channel 3
    void* pfnDMA1_Channel4_5_IRQHandler;                ///< DMA1 Channel 4 and Channel 5
    void* pfnADC1_IRQHandler;                       ///< ADC1
    void* pfnTIM1_BRK_UP_TRG_COM_IRQHandler;        ///< TIM1 Break, Update, Trigger and Commutation
    void* pfnTIM1_CC_IRQHandler;                    ///< TIM1 Capture Compare
    void* pfnTIM2_IRQHandler;                       ///< TIM2
    void* pfnTIM3_IRQHandler;                       ///< TIM3
    void* pfnTIM8_BRK_UP_TRG_COM_IRQHandler;        ///< TIM8 Break, Update, Trigger and Commutation
    void* pfnTIM8_CC_IRQHandler;                    ///< TIM8 Capture Compare
    void* pfnTIM14_IRQHandler;                      ///< TIM14
    void* pfnADC2_IRQHandler;                       ///< ADC2
    void* pfnTIM16_IRQHandler;                      ///< TIM16
    void* pfnTIM17_IRQHandler;                      ///< TIM17
    void* pfnI2C1_IRQHandler;                       ///< I2C1
    void* pfnCOMP_1_2_3_4_5_IRQHandler;             ///< COMP
    void* pfnSPI1_IRQHandler;                       ///< SPI1
    void* pfnSPI2_IRQHandler;                       ///< SPI2
    void* pfnUART1_IRQHandler;                      ///< UART1
    void* pfnUART2_IRQHandler;                      ///< UART2
    void* pfnCache_IRQHandler;                      ///< Cache
} DeviceVectors;
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0Q1)
////////////////////////////////////////////////////////////////////////////////
/// @brief  Device Vectors struct definition.
////////////////////////////////////////////////////////////////////////////////
typedef struct _DeviceVectors {
// Stack pointer
    void* pvStack;
    void* pfnReset_Handler;                         ///< Reset Handler
    void* pfnNMI_Handler;                           ///< NMI Handler
    void* pfnHardFault_Handler;                     ///< Hard Fault Handler
    void* pfnMemManage_Handler;                     ///< MPU Fault Handler
    void* pfnBusFault_Handler;                      ///< Bus Fault Handler
    void* pfnUsageFault_Handler;                    ///< Usage Fault Handler
    void* pfnReserved1_Handler;                     ///< Reserved
    void* pfnReserved2_Handler;                     ///< Reserved
    void* pfnReserved3_Handler;                     ///< Reserved
    void* pfnReserved4_Handler;                     ///< Reserved
    void* pfnSVC_Handler;                           ///< SVCall Handler
    void* pfnDebugMon_Handler;                      ///< Debug Monitor Handler
    void* pfnReserved5_Handler;                     ///< Reserved
    void* pfnPendSV_Handler;                        ///< PendSV Handler
    void* pfnSysTick_Handler;                       ///< SysTick Handler

    void* pfnWWDG_IWDG_IRQHandler;                  ///< Window Watchdog
    void* pfnPVD_IRQHandler;                        ///< PVD through EXTI Line detect
    void* pfnPWM_IRQHandler;                        ///< RTC through EXTI Line & Tamper
    void* pfnFLASH_IRQHandler;                      ///< FLASH
    void* pfnRCC_IRQHandler;                        ///< RCC & CRS
    void* pfnEXTI0_1_IRQHandler;                    ///< EXTI Line 0 and 1
    void* pfnEXTI2_3_IRQHandler;                    ///< EXTI Line 2 and 3
    void* pfnEXTI4_15_IRQHandler;                   ///< EXTI Line 4 to 15
    void* pfnHWDIV_IRQHandler;                      ///< Reserved
    void* pfnDMA1_Channel1_IRQHandler;              ///< DMA1 Channel 1
    void* pfnDMA1_Channel2_3_IRQHandler;            ///< DMA1 Channel 2 and Channel 3
    void* pfnDMA1_Channel4_5_IRQHandler;            ///< DMA1 Channel 4 and Channel 5
    void* pfnADC_COMP_IRQHandler;                   ///< ADC1 & COMP
    void* pfnTIM1_BRK_UP_TRG_COM_IRQHandler;        ///< TIM1 Break, Update, Trigger and Commutation
    void* pfnTIM1_CC_IRQHandler;                    ///< TIM1 Capture Compare
    void* pfnTIM2_IRQHandler;                       ///< TIM2
    void* pfnTIM3_IRQHandler;                       ///< TIM3
    void* pfnReserved6_Handler;                     ///< Reserved
    void* pfnReserved7_Handler;                     ///< Reserved
    void* pfnTIM14_IRQHandler;                      ///< TIM14
    void* pfnReserved8_Handler;                     ///< Reserved
    void* pfnTIM16_IRQHandler;                      ///< TIM16
    void* pfnTIM17_IRQHandler;                      ///< TIM17
    void* pfnI2C1_IRQHandler;                       ///< I2C1
    void* pfnReserved9_Handler;                     ///< Reserved
    void* pfnSPI1_IRQHandler;                       ///< SPI2
    void* pfnSPI2_IRQHandler;                       ///< SPI2
    void* pfnUART1_IRQHandler;                      ///< UART1
    void* pfnUART2_IRQHandler;                      ///< UART2
} DeviceVectors;
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
// Cortex-M core handlers
    #pragma weak NMI_Handler                        =   Dummy_Handler
    #pragma weak HardFault_Handler                  =   Dummy_Handler
    #pragma weak MemManage_Handler                  =   Dummy_Handler
    #pragma weak BusFault_Handler                   =   Dummy_Handler
    #pragma weak UsageFault_Handler                 =   Dummy_Handler
    #pragma weak SVC_Handler                        =   Dummy_Handler
    #pragma weak DebugMon_Handler                   =   Dummy_Handler
    #pragma weak PendSV_Handler                     =   Dummy_Handler
    #pragma weak SysTick_Handler                    =   Dummy_Handler

// Peripherals handlers
    #pragma weak WWDG_IRQHandler                    =   Dummy_Handler
    #pragma weak PVD_IRQHandler                     =   Dummy_Handler
    #pragma weak TAMPER_IRQHandler                  =   Dummy_Handler
    #pragma weak RTC_IRQHandler                     =   Dummy_Handler
    #pragma weak FLASH_IRQHandler                   =   Dummy_Handler
    #pragma weak RCC_IRQHandler                     =   Dummy_Handler
    #pragma weak EXTI0_IRQHandler                   =   Dummy_Handler
    #pragma weak EXTI1_IRQHandler                   =   Dummy_Handler
    #pragma weak EXTI2_IRQHandler                   =   Dummy_Handler
    #pragma weak EXTI3_IRQHandler                   =   Dummy_Handler
    #pragma weak EXTI4_IRQHandler                   =   Dummy_Handler
    #pragma weak DMA1_Channel1_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel2_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel3_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel4_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel5_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel6_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel7_IRQHandler           =   Dummy_Handler
    #pragma weak ADC1_2_IRQHandler                  =   Dummy_Handler
    #pragma weak USB_HP_CAN1_TX_IRQHandler          =   Dummy_Handler
    #pragma weak USB_LP_CAN1_RX0_IRQHandler         =   Dummy_Handler
    #pragma weak CAN1_RX1_IRQHandler                =   Dummy_Handler
    #pragma weak CAN1_SCE_IRQHandler                =   Dummy_Handler
    #pragma weak EXTI9_5_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM1_BRK_IRQHandler                =   Dummy_Handler
    #pragma weak TIM1_UP_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM1_TRG_COM_IRQHandler            =   Dummy_Handler
    #pragma weak TIM1_CC_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM2_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM3_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM4_IRQHandler                    =   Dummy_Handler
    #pragma weak I2C1_EV_IRQHandler                 =   Dummy_Handler
    #pragma weak I2C1_ER_IRQHandler                 =   Dummy_Handler
    #pragma weak I2C2_EV_IRQHandler                 =   Dummy_Handler
    #pragma weak I2C2_ER_IRQHandler                 =   Dummy_Handler
    #pragma weak SPI1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI2_IRQHandler                    =   Dummy_Handler
    #pragma weak UART1_IRQHandler                   =   Dummy_Handler
    #pragma weak UART2_IRQHandler                   =   Dummy_Handler
    #pragma weak UART3_IRQHandler                   =   Dummy_Handler
    #pragma weak EXTI15_10_IRQHandler               =   Dummy_Handler
    #pragma weak RTCAlarm_IRQHandler                =   Dummy_Handler
    #pragma weak USBWakeUp_IRQHandler               =   Dummy_Handler
    #pragma weak AESEncrupt_IRQHandler              =   Dummy_Handler
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0N1)
// Cortex-M core handlers
    #pragma weak NMI_Handler                        =   Dummy_Handler
    #pragma weak HardFault_Handler                  =   Dummy_Handler
    #pragma weak SVC_Handler                        =   Dummy_Handler
    #pragma weak PendSV_Handler                     =   Dummy_Handler
    #pragma weak SysTick_Handler                    =   Dummy_Handler

// Peripherals handlers
    #pragma weak WWDG_IRQHandler                    =   Dummy_Handler
    #pragma weak PVD_IRQHandler                     =   Dummy_Handler
    #pragma weak FLASH_IRQHandler                   =   Dummy_Handler
    #pragma weak RCC_CRS_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI0_1_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI2_3_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI4_15_IRQHandler                =   Dummy_Handler
    #pragma weak DMA1_Channel1_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel2_3_IRQHandler         =   Dummy_Handler
    #pragma weak DMA1_Channel4_5_IRQHandler         =   Dummy_Handler
    #pragma weak ADC1_COMP_IRQHandler               =   Dummy_Handler
    #pragma weak TIM1_BRK_UP_TRG_COM_IRQHandler     =   Dummy_Handler
    #pragma weak TIM1_CC_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM2_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM3_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM14_IRQHandler                   =   Dummy_Handler
    #pragma weak TIM16_IRQHandler                   =   Dummy_Handler
    #pragma weak TIM17_IRQHandler                   =   Dummy_Handler
    #pragma weak I2C1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI2_IRQHandler                    =   Dummy_Handler
    #pragma weak UART1_IRQHandler                   =   Dummy_Handler
    #pragma weak UART2_IRQHandler                   =   Dummy_Handler
    #pragma weak AES_IRQHandler                     =   Dummy_Handler
    #pragma weak CAN_IRQHandler                     =   Dummy_Handler
    #pragma weak USB_IRQHandler                     =   Dummy_Handler
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0P1)
// Cortex-M core handlers
    #pragma weak NMI_Handler                        =   Dummy_Handler
    #pragma weak HardFault_Handler                  =   Dummy_Handler
    #pragma weak MemManage_Handler                  =   Dummy_Handler
    #pragma weak BusFault_Handler                   =   Dummy_Handler
    #pragma weak UsageFault_Handler                 =   Dummy_Handler
    #pragma weak SVC_Handler                        =   Dummy_Handler
    #pragma weak DebugMon_Handler                   =   Dummy_Handler
    #pragma weak PendSV_Handler                     =   Dummy_Handler
    #pragma weak SysTick_Handler                    =   Dummy_Handler

// Peripherals handlers
    #pragma weak WWDG_IWDG_IRQHandler               =   Dummy_Handler
    #pragma weak PVD_IRQHandler                     =   Dummy_Handler
    #pragma weak PWM_IRQHandler                     =   Dummy_Handler
    #pragma weak FLASH_IRQHandler                   =   Dummy_Handler
    #pragma weak RCC_IRQHandler                     =   Dummy_Handler
    #pragma weak EXTI0_1_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI2_3_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI4_15_IRQHandler                =   Dummy_Handler
    #pragma weak HWDIV_IRQHandler                   =   Dummy_Handler
    #pragma weak DMA1_Channel1_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel2_3_IRQHandler         =   Dummy_Handler
    #pragma weak DMA1_Channel4_5_IRQHandler         =   Dummy_Handler
    #pragma weak ADC1_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM1_BRK_UP_TRG_COM_IRQHandler     =   Dummy_Handler
    #pragma weak TIM1_CC_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM2_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM3_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM8_BRK_UP_TRG_COM_IRQHandler     =   Dummy_Handler
    #pragma weak TIM8_CC_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM14_IRQHandler                   =   Dummy_Handler
    #pragma weak ADC2_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM16_IRQHandler                   =   Dummy_Handler
    #pragma weak TIM17_IRQHandler                   =   Dummy_Handler
    #pragma weak I2C1_IRQHandler                    =   Dummy_Handler
    #pragma weak COMP_1_2_3_4_5_IRQHandler          =   Dummy_Handler
    #pragma weak SPI1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI2_IRQHandler                    =   Dummy_Handler
    #pragma weak UART1_IRQHandler                   =   Dummy_Handler
    #pragma weak UART2_IRQHandler                   =   Dummy_Handler
    #pragma weak Cache_IRQHandler                   =   Dummy_Handler
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0Q1)
// Cortex-M core handlers
    #pragma weak NMI_Handler                        =   Dummy_Handler
    #pragma weak HardFault_Handler                  =   Dummy_Handler
    #pragma weak MemManage_Handler                  =   Dummy_Handler
    #pragma weak BusFault_Handler                   =   Dummy_Handler
    #pragma weak UsageFault_Handler                 =   Dummy_Handler
    #pragma weak SVC_Handler                        =   Dummy_Handler
    #pragma weak DebugMon_Handler                   =   Dummy_Handler
    #pragma weak PendSV_Handler                     =   Dummy_Handler
    #pragma weak SysTick_Handler                    =   Dummy_Handler

// Peripherals handlers
    #pragma weak WWDG_IWDG_IRQHandler               =   Dummy_Handler
    #pragma weak PVD_IRQHandler                     =   Dummy_Handler
    #pragma weak PWM_IRQHandler                     =   Dummy_Handler
    #pragma weak FLASH_IRQHandler                   =   Dummy_Handler
    #pragma weak RCC_IRQHandler                     =   Dummy_Handler
    #pragma weak EXTI0_1_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI2_3_IRQHandler                 =   Dummy_Handler
    #pragma weak EXTI4_15_IRQHandler                =   Dummy_Handler
    #pragma weak HWDIV_IRQHandler                   =   Dummy_Handler
    #pragma weak DMA1_Channel1_IRQHandler           =   Dummy_Handler
    #pragma weak DMA1_Channel2_3_IRQHandler         =   Dummy_Handler
    #pragma weak DMA1_Channel4_5_IRQHandler         =   Dummy_Handler
    #pragma weak ADC_COMP_IRQHandler                =   Dummy_Handler
    #pragma weak TIM1_BRK_UP_TRG_COM_IRQHandler     =   Dummy_Handler
    #pragma weak TIM1_CC_IRQHandler                 =   Dummy_Handler
    #pragma weak TIM2_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM3_IRQHandler                    =   Dummy_Handler
    #pragma weak TIM14_IRQHandler                   =   Dummy_Handler
    #pragma weak TIM16_IRQHandler                   =   Dummy_Handler
    #pragma weak TIM17_IRQHandler                   =   Dummy_Handler
    #pragma weak I2C1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI1_IRQHandler                    =   Dummy_Handler
    #pragma weak SPI2_IRQHandler                    =   Dummy_Handler
    #pragma weak UART1_IRQHandler                   =   Dummy_Handler
    #pragma weak UART2_IRQHandler                   =   Dummy_Handler
#endif

////////////////////////////////////////////////////////////////////////////////
// Exception Table
////////////////////////////////////////////////////////////////////////////////
#if (__KEIL)



#else
//#pragma language = extended
//#pragma segment = "CSTACK"
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
#ifdef _STARTUP_MM32_C_

#define GLOBAL
#else
#define GLOBAL extern
#endif

#undef GLOBAL

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{

void __iar_program_start(void);

int __low_level_init(void);
void Dummy_Handler(void);

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
    // Cortex-M core handlers
    void Reset_Handler(void);
    void NMI_Handler(void);
    void HardFault_Handler(void);
    void MemManage_Handler(void);
    void BusFault_Handler(void);
    void UsageFault_Handler(void);
    void SVC_Handler(void);
    void DebugMon_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);

    // Peripherals handlers
    void WWDG_IRQHandler(void);
    void PVD_IRQHandler(void);
    void TAMPER_IRQHandler(void);
    void RTC_IRQHandler(void);
    void FLASH_IRQHandler(void);
    void RCC_IRQHandler(void);
    void EXTI0_IRQHandler(void);
    void EXTI1_IRQHandler(void);
    void EXTI2_IRQHandler(void);
    void EXTI3_IRQHandler(void);
    void EXTI4_IRQHandler(void);
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_IRQHandler(void);
    void DMA1_Channel3_IRQHandler(void);
    void DMA1_Channel4_IRQHandler(void);
    void DMA1_Channel5_IRQHandler(void);
    void DMA1_Channel6_IRQHandler(void);
    void DMA1_Channel7_IRQHandler(void);
    void ADC1_2_IRQHandler(void);
    void USB_HP_CAN1_TX_IRQHandler(void);
    void USB_LP_CAN1_RX0_IRQHandler(void);
    void CAN1_RX1_IRQHandler(void);
    void CAN1_SCE_IRQHandler(void);
    void EXTI9_5_IRQHandler(void);
    void TIM1_BRK_IRQHandler(void);
    void TIM1_UP_IRQHandler(void);
    void TIM1_TRG_COM_IRQHandler(void);
    void TIM1_CC_IRQHandler(void);
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
    void TIM4_IRQHandler(void);
    void I2C1_EV_IRQHandler(void);
    void I2C1_ER_IRQHandler(void);
    void I2C2_EV_IRQHandler(void);
    void I2C2_ER_IRQHandler(void);
    void SPI1_IRQHandler(void);
    void SPI2_IRQHandler(void);
    void UART1_IRQHandler(void);
    void UART2_IRQHandler(void);
    void UART3_IRQHandler(void);
    void EXTI15_10_IRQHandler(void);
    void RTCAlarm_IRQHandler(void);
    void USBWakeUp_IRQHandler(void);
    void AESEncrupt_IRQHandler(void);
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0N1)
    // Cortex-M core handlers
    void Reset_Handler(void);
    void NMI_Handler(void);
    void HardFault_Handler(void);
    void SVC_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);

    // Peripherals handlers
    void WWDG_IRQHandler(void);
    void PVD_IRQHandler(void);
//  void RTC_IRQHandler(void);
    void FLASH_IRQHandler(void);
    void RCC_CRS_IRQHandler(void);
    void EXTI0_1_IRQHandler(void);
    void EXTI2_3_IRQHandler(void);
    void EXTI4_15_IRQHandler(void);
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_3_IRQHandler(void);
    void DMA1_Channel4_5_IRQHandler(void);
    void ADC1_COMP_IRQHandler(void);
    void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
    void TIM1_CC_IRQHandler(void);
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
    void TIM14_IRQHandler(void);
    void TIM16_IRQHandler(void);
    void TIM17_IRQHandler(void);
    void I2C1_IRQHandler(void);
    void SPI1_IRQHandler(void);
    void SPI2_IRQHandler(void);
    void UART1_IRQHandler(void);
    void UART2_IRQHandler(void);
    void AES_IRQHandler(void);
    void CAN_IRQHandler(void);
    void USB_IRQHandler(void);
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0P1)
    // Cortex-M core handlers
    void Reset_Handler(void);
    void NMI_Handler(void);
    void HardFault_Handler(void);
    void MemManage_Handler(void);
    void BusFault_Handler(void);
    void UsageFault_Handler(void);
    void SVC_Handler(void);
    void DebugMon_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);

    // Peripherals handlers
    void WWDG_IWDG_IRQHandler(void);
    void PVD_IRQHandler(void);
    void PWM_IRQHandler(void);
    void FLASH_IRQHandler(void);
    void RCC_IRQHandler(void);
    void EXTI0_1_IRQHandler(void);
    void EXTI2_3_IRQHandler(void);
    void EXTI4_15_IRQHandler(void);
    void HWDIV_IRQHandler(void);
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_3_IRQHandler(void);
    void DMA1_Channel4_5_IRQHandler(void);
    void ADC1_IRQHandler(void);
    void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
    void TIM1_CC_IRQHandler(void);
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
    void TIM8_BRK_UP_TRG_COM_IRQHandler(void);
    void TIM8_CC_IRQHandler(void);
    void TIM14_IRQHandler(void);
    void ADC2_IRQHandler(void);
    void TIM16_IRQHandler(void);
    void TIM17_IRQHandler(void);
    void I2C1_IRQHandler(void);
    void COMP_1_2_3_4_5_IRQHandler(void);
    void SPI1_IRQHandler(void);
    void SPI2_IRQHandler(void);
    void UART1_IRQHandler(void);
    void UART2_IRQHandler(void);
    void Cache_IRQHandler(void);
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0Q1)
    // Cortex-M core handlers
    void Reset_Handler(void);
    void NMI_Handler(void);
    void HardFault_Handler(void);
    void MemManage_Handler(void);
    void BusFault_Handler(void);
    void UsageFault_Handler(void);
    void SVC_Handler(void);
    void DebugMon_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);

    // Peripherals handlers
    void WWDG_IWDG_IRQHandler(void);
    void PVD_IRQHandler(void);
    void PWM_IRQHandler(void);
    void FLASH_IRQHandler(void);
    void RCC_IRQHandler(void);
    void EXTI0_1_IRQHandler(void);
    void EXTI2_3_IRQHandler(void);
    void EXTI4_15_IRQHandler(void);
    void HWDIV_IRQHandler(void);
    void DMA1_Channel1_IRQHandler(void);
    void DMA1_Channel2_3_IRQHandler(void);
    void DMA1_Channel4_5_IRQHandler(void);
    void ADC_COMP_IRQHandler(void);
    void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
    void TIM1_CC_IRQHandler(void);
    void TIM2_IRQHandler(void);
    void TIM3_IRQHandler(void);
    void TIM14_IRQHandler(void);
    void TIM16_IRQHandler(void);
    void TIM17_IRQHandler(void);
    void I2C1_IRQHandler(void);
    void SPI1_IRQHandler(void);
    void SPI2_IRQHandler(void);
    void UART1_IRQHandler(void);
    void UART2_IRQHandler(void);
#endif


/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
