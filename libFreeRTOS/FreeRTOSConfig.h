/* config template for mcush
 * MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __FREERTOS_CONFIG_H_
#define __FREERTOS_CONFIG_H_
#include <stdint.h>
extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     1
#define configCPU_CLOCK_HZ                      ((unsigned long)SystemCoreClock)
#ifdef CONFIG_TICK_RATE_HZ
    #define configTICK_RATE_HZ                  ((TickType_t)CONFIG_TICK_RATE_HZ)
#else
    #define configTICK_RATE_HZ                  ((TickType_t)250)
#endif
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                ((unsigned short)64)
//#define configTOTAL_HEAP_SIZE                 ((size_t)(8*1024))
#define configMAX_TASK_NAME_LEN                 8
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TIMERS                        1

#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_TASK_STACK_DEPTH            (configMINIMAL_STACK_SIZE)
#define configTIMER_QUEUE_LENGTH                16

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         2

#define configUSE_MUTEXES                       1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_ALTERNATIVE_API               0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_RECURSIVE_MUTEXES             1
#define configQUEUE_REGISTRY_SIZE               8

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1

#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
//#define configGENERATE_RUN_TIME_STATS           0


#ifdef CORTEX_M0
    /* nothing to do */
#else
    #ifndef __NVIC_PRIO_BITS
        #define __NVIC_PRIO_BITS  4
    #endif
    
    #define configPRIO_BITS  __NVIC_PRIO_BITS 
    #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   ((1<<configPRIO_BITS)-1)
    
    #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
    
    #if configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  >= configLIBRARY_LOWEST_INTERRUPT_PRIORITY
        #error "configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY too large"
    #endif
    
    #define PRIO_MIN   configLIBRARY_LOWEST_INTERRUPT_PRIORITY
    #define PRIO_MAX   configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
    
    
    /* Interrupt priorities used by the kernel port layer itself.  These are generic
    to all Cortex-M ports, and do not rely on any particular library functions. */
    #define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
    /* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
    See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
        
#endif



#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); } 

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif

