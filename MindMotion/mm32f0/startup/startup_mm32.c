////////////////////////////////////////////////////////////////////////////////
/// @file    STARTUP_MM32.C
/// @author  AE TEAM
/// @version 2.0.0
/// @date    2018-08-01
/// @brief   THIS FILE PROVIDES ALL THE STARTUP FUNCTIONS.
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
#define _STARTUP_MM32_C_
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "startup_mm32.h"
#include "mm32.h"

extern void main(void);
extern char _sidata, _sdata, _edata, _sbss, _ebss, _sstack, _estack;

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_RESOURCE
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Exported_Constants
/// @{

////////////////////////////////////////////////////////////////////////////////
//
//   KEIL development environment and definition
//
////////////////////////////////////////////////////////////////////////////////
#if defined(__KEIL)
/*----------------------------------------------------------------------------
  User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

extern void __main     (void) __attribute__((noreturn));                        ///< PreeMain (C library entry point)
#define __initial_sp    Image$$ARM_LIB_STACK$$ZI$$Limit
#endif


////////////////////////////////////////////////////////////////////////////////
#if defined(__MM3N1)
#if defined(__KEIL)
    const DeviceVectors __Vectors __attribute__((section ("RESET")));
    const DeviceVectors __Vectors = {
        .pvStack                        = (void*)(&__initial_sp),               ///< Keil Initial Stack Pointer
#else
    #pragma section     = ".intvec"
    #pragma location    = ".intvec"
    const DeviceVectors __vector_table = {
        (void*) __sfe("CSTACK"),                                                ///< IAR Initial Stack Pointer
#endif

    .pfnReset_Handler                   = (void*)Reset_Handler,                 ///< Reset Handler
    .pfnNMI_Handler                     = (void*)NMI_Handler,                   ///< NMI Handler
    .pfnHardFault_Handler               = (void*)HardFault_Handler,             ///< Hard Fault Handler
    .pfnMemManage_Handler               = (void*)MemManage_Handler,             ///< MPU Fault Handler
    .pfnBusFault_Handler                = (void*)BusFault_Handler,              ///< Bus Fault Handler
    .pfnUsageFault_Handler              = (void*)UsageFault_Handler,            ///< Usage Fault Handler
    .pfnReserved1_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved2_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved3_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved4_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnSVC_Handler                     = (void*)SVC_Handler,                   ///< SVCall Handler
    .pfnDebugMon_Handler                = (void*)DebugMon_Handler,              ///< Debug Monitor Handler
    .pfnReserved5_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnPendSV_Handler                  = (void*)PendSV_Handler,                ///< PendSV Handler
    .pfnSysTick_Handler                 = (void*)SysTick_Handler,               ///< SysTick Handler

    // Configurable interrupts
    .pfnWWDG_IRQHandler                 = (void*)WWDG_IRQHandler,               ///< Window Watchdog
    .pfnPVD_IRQHandler                  = (void*)PVD_IRQHandler,                ///< PVD through EXTI Line detect
    .pfnTAMPER_IRQHandler               = (void*)TAMPER_IRQHandler,             ///< Tamper
    .pfnRTC_IRQHandler                  = (void*)RTC_IRQHandler,                ///< RTC
    .pfnFLASH_IRQHandler                = (void*)FLASH_IRQHandler,              ///< Flash
    .pfnRCC_IRQHandler                  = (void*)RCC_IRQHandler,                ///< RCC
    .pfnEXTI0_IRQHandler                = (void*)EXTI0_IRQHandler,              ///< EXTI Line 0
    .pfnEXTI1_IRQHandler                = (void*)EXTI1_IRQHandler,              ///< EXTI Line 1
    .pfnEXTI2_IRQHandler                = (void*)EXTI2_IRQHandler,              ///< EXTI Line 2
    .pfnEXTI3_IRQHandler                = (void*)EXTI3_IRQHandler,              ///< EXTI Line 3
    .pfnEXTI4_IRQHandler                = (void*)EXTI4_IRQHandler,              ///< EXTI Line 4
    .pfnDMA1_Channel1_IRQHandler        = (void*)DMA1_Channel1_IRQHandler,      ///< DMA1 Channel 1
    .pfnDMA1_Channel2_IRQHandler        = (void*)DMA1_Channel2_IRQHandler,      ///< DMA1 Channel 2
    .pfnDMA1_Channel3_IRQHandler        = (void*)DMA1_Channel3_IRQHandler,      ///< DMA1 Channel 3
    .pfnDMA1_Channel4_IRQHandler        = (void*)DMA1_Channel4_IRQHandler,      ///< DMA1 Channel 4
    .pfnDMA1_Channel5_IRQHandler        = (void*)DMA1_Channel5_IRQHandler,      ///< DMA1 Channel 5
    .pfnDMA1_Channel6_IRQHandler        = (void*)DMA1_Channel6_IRQHandler,      ///< DMA1 Channel 6
    .pfnDMA1_Channel7_IRQHandler        = (void*)DMA1_Channel7_IRQHandler,      ///< DMA1 Channel 7
    .pfnADC1_2_IRQHandler               = (void*)ADC1_2_IRQHandler,             ///< ADC1 & ADC2
    .pfnUSB_HP_CAN1_TX_IRQHandler       = (void*)USB_HP_CAN1_TX_IRQHandler,     ///< USB High Priority or CAN1 TX
    .pfnReserved6_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnCAN1_RX1_IRQHandler             = (void*)CAN1_RX1_IRQHandler,           ///< CAN1 RX1
    .pfnReserved7_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnEXTI9_5_IRQHandler              = (void*)EXTI9_5_IRQHandler,            ///< EXTI Line 9..5
    .pfnTIM1_BRK_IRQHandler             = (void*)TIM1_BRK_IRQHandler,           ///< TIM1 Break
    .pfnTIM1_UP_IRQHandler              = (void*)TIM1_UP_IRQHandler,            ///< TIM1 Update
    .pfnTIM1_TRG_COM_IRQHandler         = (void*)TIM1_TRG_COM_IRQHandler,       ///< TIM1 Trigger and Commutation
    .pfnTIM1_CC_IRQHandler              = (void*)TIM1_CC_IRQHandler,            ///< TIM1 Capture Compare
    .pfnTIM2_IRQHandler                 = (void*)TIM2_IRQHandler,               ///< TIM2
    .pfnTIM3_IRQHandler                 = (void*)TIM3_IRQHandler,               ///< TIM3
    .pfnTIM4_IRQHandler                 = (void*)TIM4_IRQHandler,               ///< TIM4
    .pfnI2C1_EV_IRQHandler              = (void*)I2C1_EV_IRQHandler,            ///< I2C1 Event
    .pfnReserved8_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnI2C2_EV_IRQHandler              = (void*)I2C2_EV_IRQHandler,            ///< I2C2 Event
    .pfnReserved9_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnSPI1_IRQHandler                 = (void*)SPI1_IRQHandler,               ///< SPI1
    .pfnSPI2_IRQHandler                 = (void*)SPI2_IRQHandler,               ///< SPI2
    .pfnUART1_IRQHandler                = (void*)UART1_IRQHandler,              ///< UART1
    .pfnUART2_IRQHandler                = (void*)UART2_IRQHandler,              ///< UART2
    .pfnUART3_IRQHandler                = (void*)UART3_IRQHandler,              ///< UART3
    .pfnEXTI15_10_IRQHandler            = (void*)EXTI15_10_IRQHandler,          ///< EXTI Line 15..10
    .pfnRTCAlarm_IRQHandler             = (void*)RTCAlarm_IRQHandler,           ///< RTC Alarm through EXTI Line
    .pfnUSBWakeUp_IRQHandler            = (void*)USBWakeUp_IRQHandler,          ///< USB Wakeup from suspend
    .pfnReserved10_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnReserved11_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnAES_IRQHandler                  = (void*)AESEncrupt_IRQHandler          ///< USB Wakeup from suspend
};
#endif


////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0N1)
//#if defined(__KEIL)
//    const DeviceVectors __Vectors __attribute__((section ("RESET")));
//    const DeviceVectors __Vectors = {
//        .pvStack                        = (void*)(&__initial_sp),               ///< Keil Initial Stack Pointer
//#else
//    #pragma section     = ".intvec"
//    #pragma location    = ".intvec"
//    const DeviceVectors __vector_table = {
//        (void*) __sfe("CSTACK"),                                                ///< IAR Initial Stack Pointer
//#endif
    extern char _estack;
    const DeviceVectors __vector_table __attribute__((section (".isr_vector")));
    const DeviceVectors __vector_table = {
    .pvStack                            = (void*)(&_estack),

    .pfnReset_Handler                   = (void*)Reset_Handler,                 ///< Reset Handler
    .pfnNMI_Handler                     = (void*)NMI_Handler,                   ///< NMI Handler
    .pfnHardFault_Handler               = (void*)HardFault_Handler,             ///< Hard Fault Handler
    .pfnReserved1_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved2_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved3_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved4_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved5_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved6_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved7_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnSVC_Handler                     = (void*)SVC_Handler,                   ///< SVCall Handler
    .pfnReserved8_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved9_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnPendSV_Handler                  = (void*)PendSV_Handler,                ///< PendSV Handler
    .pfnSysTick_Handler                 = (void*)SysTick_Handler,               ///< SysTick Handler

    // Configurable interrupts
    .pfnWWDG_IRQHandler                 = (void*)WWDG_IRQHandler,               ///< Window Watchdog
    .pfnPVD_IRQHandler                  = (void*)PVD_IRQHandler,                ///< PVD through EXTI Line detect
    .pfnReserved10_Handler              = (void*)(0UL),                         ///< Reserved10_Handler
    .pfnFLASH_IRQHandler                = (void*)FLASH_IRQHandler,              ///< FLASH
    .pfnRCC_CRS_IRQHandler              = (void*)RCC_CRS_IRQHandler,            ///< RCC & CRS
    .pfnEXTI0_1_IRQHandler              = (void*)EXTI0_1_IRQHandler,            ///< EXTI Line 0 and 1
    .pfnEXTI2_3_IRQHandler              = (void*)EXTI2_3_IRQHandler,            ///< EXTI Line 2 and 3
    .pfnEXTI4_15_IRQHandler             = (void*)EXTI4_15_IRQHandler,           ///< EXTI Line 4 to 15
    .pfnReserved11_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnDMA1_Channel1_IRQHandler        = (void*)DMA1_Channel1_IRQHandler,      ///< DMA1 Channel 1
    .pfnDMA1_Channel2_3_IRQHandler      = (void*)DMA1_Channel2_3_IRQHandler,    ///< DMA1 Channel 2 and Channel 3
    .pfnDMA1_Channel4_5_IRQHandler      = (void*)DMA1_Channel4_5_IRQHandler,    ///< DMA1 Channel 4 and Channel 5
    .pfnADC1_COMP_IRQHandler            = (void*)ADC1_COMP_IRQHandler,          ///< ADC1 & COMP
    .pfnTIM1_BRK_UP_TRG_COM_IRQHandler  = (void*)TIM1_BRK_UP_TRG_COM_IRQHandler,///< TIM1 Break, Update, Trigger and Commutation
    .pfnTIM1_CC_IRQHandler              = (void*)TIM1_CC_IRQHandler,            ///< TIM1 Capture Compare
    .pfnTIM2_IRQHandler                 = (void*)TIM2_IRQHandler,               ///< TIM2
    .pfnTIM3_IRQHandler                 = (void*)TIM3_IRQHandler,               ///< TIM3
    .pfnReserved12_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnReserved13_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnTIM14_IRQHandler                = (void*)TIM14_IRQHandler,              ///< TIM14
    .pfnReserved14_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnTIM16_IRQHandler                = (void*)TIM16_IRQHandler,              ///< TIM16
    .pfnTIM17_IRQHandler                = (void*)TIM17_IRQHandler,              ///< TIM17
    .pfnI2C1_IRQHandler                 = (void*)I2C1_IRQHandler,               ///< I2C1
    .pfnReserved15_Handler              = (void*)(0UL),                         ///< Reserved
    .pfnSPI1_IRQHandler                 = (void*)SPI1_IRQHandler,               ///< SPI1
    .pfnSPI2_IRQHandler                 = (void*)SPI2_IRQHandler,               ///< SPI2
    .pfnUART1_IRQHandler                = (void*)UART1_IRQHandler,              ///< UART1
    .pfnUART2_IRQHandler                = (void*)UART2_IRQHandler,              ///< UART2
    .pfnAES_IRQHandler                  = (void*)AES_IRQHandler,                ///< AES
    .pfnCAN_IRQHandler                  = (void*)CAN_IRQHandler,                ///< CAN
    .pfnUSB_IRQHandler                  = (void*)USB_IRQHandler                 ///< USB
};
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0P1)
#if defined(__KEIL)
    const DeviceVectors __Vectors __attribute__((section ("RESET")));
    const DeviceVectors __Vectors = {
        .pvStack                        = (void*)(&__initial_sp),               ///< Keil Initial Stack Pointer
#else
    #pragma section     = ".intvec"
    #pragma location    = ".intvec"
    const DeviceVectors __vector_table = {
        (void*) __sfe("CSTACK"),                                                ///< IAR Initial Stack Pointer
#endif

    .pfnReset_Handler                   = (void*)Reset_Handler,                 ///< Reset Handler
    .pfnNMI_Handler                     = (void*)NMI_Handler,                   ///< NMI Handler
    .pfnHardFault_Handler               = (void*)HardFault_Handler,             ///< Hard Fault Handler
    .pfnMemManage_Handler               = (void*)MemManage_Handler,             ///< MPU Fault Handler
    .pfnBusFault_Handler                = (void*)BusFault_Handler,              ///< Bus Fault Handler
    .pfnUsageFault_Handler              = (void*)UsageFault_Handler,            ///< Usage Fault Handler
    .pfnReserved1_Handler               = (void*)0,                             ///< Reserved
    .pfnReserved2_Handler               = (void*)0,                             ///< Reserved
    .pfnReserved3_Handler               = (void*)0,                             ///< Reserved
    .pfnReserved4_Handler               = (void*)0,                             ///< Reserved
    .pfnSVC_Handler                     = (void*)SVC_Handler,                   ///< SVCall Handler
    .pfnDebugMon_Handler                = (void*)DebugMon_Handler,              ///< Debug Monitor Handler
    .pfnReserved5_Handler               = (void*)0,                             ///< Reserved
    .pfnPendSV_Handler                  = (void*)PendSV_Handler,                ///< PendSV Handler
    .pfnSysTick_Handler                 = (void*)SysTick_Handler,               ///< SysTick Handler

    // External Interrupts
    .pfnWWDG_IWDG_IRQHandler            = (void*)WWDG_IWDG_IRQHandler,          ///< Window Watchdog and Independent Watchdog
    .pfnPVD_IRQHandler                  = (void*)PVD_IRQHandler,                ///< PVD through EXTI Line detect
    .pfnPWM_IRQHandler                  = (void*)PWM_IRQHandler,                ///< PWM Control interrupt
    .pfnFLASH_IRQHandler                = (void*)FLASH_IRQHandler,              ///< FLASH
    .pfnRCC_IRQHandler                  = (void*)RCC_IRQHandler,                ///< RCC  interrupt
    .pfnEXTI0_1_IRQHandler              = (void*)EXTI0_1_IRQHandler,            ///< EXTI Line 0 and 1
    .pfnEXTI2_3_IRQHandler              = (void*)EXTI2_3_IRQHandler,            ///< EXTI Line 2 and 3
    .pfnEXTI4_15_IRQHandler             = (void*)EXTI4_15_IRQHandler,           ///< EXTI Line 4 to 15
    .pfnHWDIV_IRQHandler                = (void*)HWDIV_IRQHandler,              ///< HWDIV
    .pfnDMA1_Channel1_IRQHandler        = (void*)DMA1_Channel1_IRQHandler,      ///< DMA1 Channel 1
    .pfnDMA1_Channel2_3_IRQHandler      = (void*)DMA1_Channel2_3_IRQHandler,    ///< DMA1 Channel 2 and Channel 3
    .pfnDMA1_Channel4_5_IRQHandler      = (void*)DMA1_Channel4_5_IRQHandler,    ///< DMA1 Channel 4 and Channel 5
    .pfnADC1_IRQHandler                 = (void*)ADC1_IRQHandler,               ///< ADC1 interrupt
    .pfnTIM1_BRK_UP_TRG_COM_IRQHandler  = (void*)TIM1_BRK_UP_TRG_COM_IRQHandler,///< TIM1 Break, Update, Trigger and Commutation
    .pfnTIM1_CC_IRQHandler              = (void*)TIM1_CC_IRQHandler,            ///< TIM1 Capture Compare
    .pfnTIM2_IRQHandler                 = (void*)TIM2_IRQHandler,               ///< TIM2
    .pfnTIM3_IRQHandler                 = (void*)TIM3_IRQHandler,               ///< TIM3
    .pfnTIM8_BRK_UP_TRG_COM_IRQHandler  = (void*)TIM8_BRK_UP_TRG_COM_IRQHandler,///< TIM8 Break, Update, Trigger and Commutation
    .pfnTIM8_CC_IRQHandler              = (void*)TIM8_CC_IRQHandler,            ///< TIM8 Capture Compare
    .pfnTIM14_IRQHandler                = (void*)TIM14_IRQHandler,              ///< TIM14
    .pfnADC2_IRQHandler                 = (void*)ADC2_IRQHandler,               ///< ADC2
    .pfnTIM16_IRQHandler                = (void*)TIM16_IRQHandler,              ///< TIM16
    .pfnTIM17_IRQHandler                = (void*)TIM17_IRQHandler,              ///< TIM17
    .pfnI2C1_IRQHandler                 = (void*)I2C1_IRQHandler,               ///< I2C1
    .pfnCOMP_1_2_3_4_5_IRQHandler       = (void*)COMP_1_2_3_4_5_IRQHandler,     ///< COMP
    .pfnSPI1_IRQHandler                 = (void*)SPI1_IRQHandler,               ///< SPI1
    .pfnSPI2_IRQHandler                 = (void*)SPI2_IRQHandler,               ///< SPI2
    .pfnUART1_IRQHandler                = (void*)UART1_IRQHandler,              ///< UART1
    .pfnUART2_IRQHandler                = (void*)UART2_IRQHandler,              ///< UART2
    .pfnCache_IRQHandler                = (void*)Cache_IRQHandler               ///< Cache
};
#endif


////////////////////////////////////////////////////////////////////////////////
#if defined(__MM0Q1)
#if defined(__KEIL)
    const DeviceVectors __Vectors __attribute__((section ("RESET")));
    const DeviceVectors __Vectors = {
        .pvStack                        = (void*)(&__initial_sp),               ///< Keil Initial Stack Pointer
#else
    #pragma section     = ".intvec"
    #pragma location    = ".intvec"
    const DeviceVectors __vector_table = {
        (void*) __sfe("CSTACK"),                                                ///< IAR Initial Stack Pointer
#endif

    .pfnReset_Handler                   = (void*)Reset_Handler,                 ///< Reset Handler
    .pfnNMI_Handler                     = (void*)NMI_Handler,                   ///< NMI Handler
    .pfnHardFault_Handler               = (void*)HardFault_Handler,             ///< Hard Fault Handler
    .pfnMemManage_Handler               = (void*)MemManage_Handler,             ///< MPU Fault Handler
    .pfnBusFault_Handler                = (void*)BusFault_Handler,              ///< Bus Fault Handler
    .pfnUsageFault_Handler              = (void*)UsageFault_Handler,            ///< Usage Fault Handler
    .pfnReserved1_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved2_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved3_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved4_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnSVC_Handler                     = (void*)SVC_Handler,                   ///< SVCall Handler
    .pfnDebugMon_Handler                = (void*)DebugMon_Handler,              ///< Debug Monitor Handler
    .pfnReserved5_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnPendSV_Handler                  = (void*)PendSV_Handler,                ///< PendSV Handler
    .pfnSysTick_Handler                 = (void*)SysTick_Handler,               ///< SysTick Handler

    // Configurable interrupts
    .pfnWWDG_IWDG_IRQHandler            = (void*)WWDG_IWDG_IRQHandler,          ///< Window Watchdog and Independent Watchdog         // Window Watchdog
    .pfnPVD_IRQHandler                  = (void*)PVD_IRQHandler,                ///< PVD through EXTI Line detect                   // PVD through EXTI Line detect
    .pfnPWM_IRQHandler                  = (void*)PWM_IRQHandler,                ///< PWM Control interrupt                          // PWM
    .pfnFLASH_IRQHandler                = (void*)FLASH_IRQHandler,              ///< FLASH                                          // FLASH
    .pfnRCC_IRQHandler                  = (void*)RCC_IRQHandler,                ///< RCC                                                // RCC & CRS
    .pfnEXTI0_1_IRQHandler              = (void*)EXTI0_1_IRQHandler,            ///< EXTI Line 0 and 1                              // EXTI Line 0 and 1
    .pfnEXTI2_3_IRQHandler              = (void*)EXTI2_3_IRQHandler,            ///< EXTI Line 2 and 3                              // EXTI Line 2 and 3
    .pfnEXTI4_15_IRQHandler             = (void*)EXTI4_15_IRQHandler,           ///< EXTI Line 4 to 15                              // EXTI Line 4 to 15
    .pfnHWDIV_IRQHandler                = (void*)HWDIV_IRQHandler,              ///< HWDIV                                          // Reserved
    .pfnDMA1_Channel1_IRQHandler         = (void*)DMA1_Channel1_IRQHandler,       ///< DMA1 Channel 1                                   // DMA1 Channel 1
    .pfnDMA1_Channel2_3_IRQHandler       = (void*)DMA1_Channel2_3_IRQHandler,     ///< DMA1 Channel 2 and Channel 3                     // DMA1 Channel 2 and Channel 3
    .pfnDMA1_Channel4_5_IRQHandler       = (void*)DMA1_Channel4_5_IRQHandler,     ///< DMA1 Channel 4 and Channel 5                     // DMA1 Channel 4 and Channel 5
    .pfnADC_COMP_IRQHandler             = (void*)ADC_COMP_IRQHandler,           ///< ADC1 and Compare   interrupt
    .pfnTIM1_BRK_UP_TRG_COM_IRQHandler  = (void*)TIM1_BRK_UP_TRG_COM_IRQHandler,///< TIM1 Break, Update, Trigger and Commutation      // TIM1 Break, Update, Trigger and Commutation
    .pfnTIM1_CC_IRQHandler              = (void*)TIM1_CC_IRQHandler,            ///< TIM1 Capture Compare                           // TIM1 Capture Compare
    .pfnTIM2_IRQHandler                 = (void*)TIM2_IRQHandler,               ///< TIM2                                           // TIM2
    .pfnTIM3_IRQHandler                 = (void*)TIM3_IRQHandler,               ///< TIM3                                           // TIM3
    .pfnReserved6_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnReserved7_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnTIM14_IRQHandler                = (void*)TIM14_IRQHandler,              ///< TIM14
    .pfnReserved8_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnTIM16_IRQHandler                = (void*)TIM16_IRQHandler,              ///< TIM16
    .pfnTIM17_IRQHandler                = (void*)TIM17_IRQHandler,              ///< TIM17
    .pfnI2C1_IRQHandler                 = (void*)I2C1_IRQHandler,               ///< I2C1
    .pfnReserved9_Handler               = (void*)(0UL),                         ///< Reserved
    .pfnSPI1_IRQHandler                 = (void*)SPI1_IRQHandler,               ///< SPI1 IRQHandler
    .pfnSPI2_IRQHandler                 = (void*)SPI2_IRQHandler,               ///< SPI2
    .pfnUART1_IRQHandler                = (void*)UART1_IRQHandler,              ///< UART1
    .pfnUART2_IRQHandler                = (void*)UART2_IRQHandler               ///< UART2
};
#endif
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Exported_Functions
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @brief  This is the code that gets called on processor reset. To initialize the
/// @        device.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void Reset_Handler(void)
{
//#if defined(__KEIL)
//  __main();                                 /* Enter PreeMain (C library entry point) */
//#else
//    __iar_program_start();
//#endif

    /* basic environment pre-init before main */
    /* patched by PengShulin <trees_peng@163.com> 2020 */
    uint32_t i, *p, *p2;
    
    /* data area init from flash */
    i = ((uint32_t)&_edata - (uint32_t)&_sdata) / 4;
    p = (uint32_t*)&_sidata;
    p2 = (uint32_t*)&_sdata;
    while( i-- )
        *p2++ = *p++;
 
    /* bss area init as zero */
    i = ((uint32_t)&_ebss - (uint32_t)&_sbss) / 4;
    p = (uint32_t*)&_sbss;
    while( i-- )
        *p++ = 0;
  
    /* fill stack with fixed pattern for watermark check */ 
    //i = ((uint32_t)&_estack - (uint32_t)&_sstack) / 4;
    //p = (uint32_t*)&_sstack;
    //while( i-- )
    //    *p++ = 0xA5A5A5A5;

    /* main app */ 
    main();
   
    /* should not run to here */ 
    while(1) {}
}


////////////////////////////////////////////////////////////////////////////////
/// @brief  This is the code that gets called on processor reset. To initialize
///         the device.
/// @param  None.
/// @retval  If return 0, the data sections will not be initialized.
////////////////////////////////////////////////////////////////////////////////
int __low_level_init(void)
{
#if defined(__KEIL)
    return 0;
#else
#if defined(__MM3N1)
    u32 *pSrc = __section_begin(".intvec");
    SCB->VTOR = ((u32) pSrc & SCB_VTOR_TBLOFF_Msk);
#endif
    return 1;       // if return 0, the data sections will not be initialized
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Default interrupt handler for unused IRQs.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void Dummy_Handler(void)
{
    while(1) {}
}


/// @}

/// @}

/// @}
