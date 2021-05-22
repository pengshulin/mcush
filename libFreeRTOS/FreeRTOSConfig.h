/* config template for mcush
 * MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __FREERTOS_CONFIG_H__
#define __FREERTOS_CONFIG_H__
#include <stdint.h>

extern uint32_t SystemCoreClock;

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     1
#define configCPU_CLOCK_HZ                      ((unsigned long)SystemCoreClock)
#ifndef configTICK_RATE_HZ
    /* uptime counter overflow period:
       @100 Hz:  (4*1024*1024*1024) * (1/100) / (24*60*60)  = 497.10 days = 16.6 months
       @200 Hz:  (4*1024*1024*1024) * (1/200) / (24*60*60)  = 248.55 days =  8.3 months
       @250 Hz:  (4*1024*1024*1024) * (1/250) / (24*60*60)  = 198.84 days =  6.6 months
       @500 Hz:  (4*1024*1024*1024) * (1/500) / (24*60*60)  =  99.42 days =  3.3 months
       @1000 Hz: (4*1024*1024*1024) * (1/1000) / (24*60*60) =  47.71 days =  1.7 months
     */
    #define configTICK_RATE_HZ                  250
#endif

#ifndef configMAX_PRIORITIES
    #define configMAX_PRIORITIES                7
#endif

#ifndef configMINIMAL_STACK_SIZE
    /* NOTE:
       idle task stack memory == MINIMAL_STACK_SIZE by default,
       the task is also responsible to free TCB/Stack left by deleted tasks,
       calling vPortFree may cause StackOverflow exception */
    #define configMINIMAL_STACK_SIZE            (256/sizeof(portSTACK_TYPE))
#endif

#ifndef configSUPPORT_STATIC_ALLOCATION
    #define configSUPPORT_STATIC_ALLOCATION     1
#endif

#ifndef configAPPLICATION_ALLOCATED_HEAP
    #define configAPPLICATION_ALLOCATED_HEAP    1  /* for heap_4 */
#endif

#ifndef configTOTAL_HEAP_SIZE
    #define configTOTAL_HEAP_SIZE               ((size_t)(4*1024))  /* for heap_4 */
#endif

#ifndef configMAX_TASK_NAME_LEN 
    #define configMAX_TASK_NAME_LEN             8
#endif

#define configUSE_16_BIT_TICKS                  0

#define configIDLE_SHOULD_YIELD                 1

#define configIDLE_TASK_NAME                    "idleT"

#ifndef configUSE_TIMERS
    #define configUSE_TIMERS                    1
	#define configTIMER_SERVICE_TASK_NAME       "tmrSvrT"
    #define configTIMER_TASK_PRIORITY           (configMAX_PRIORITIES - 1)
    #ifndef configTIMER_TASK_STACK_DEPTH
        #define configTIMER_TASK_STACK_DEPTH    (320/sizeof(portSTACK_TYPE))
    #endif
    #define configTIMER_QUEUE_LENGTH            16
#endif

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         2

#ifndef configUSE_MUTEXES
    #define configUSE_MUTEXES                   1
#endif

#ifndef configUSE_RECURSIVE_MUTEXES
    #if configUSE_MUTEXES
        #define configUSE_RECURSIVE_MUTEXES     1
    #else
        #define configUSE_RECURSIVE_MUTEXES     0
    #endif
#endif

#ifndef configUSE_COUNTING_SEMAPHORES
    #define configUSE_COUNTING_SEMAPHORES       1
#endif

#ifndef configUSE_ALTERNATIVE_API
    #define configUSE_ALTERNATIVE_API           0
#endif

#ifndef configCHECK_FOR_STACK_OVERFLOW
    #define configCHECK_FOR_STACK_OVERFLOW      2
#endif

#ifndef configQUEUE_REGISTRY_SIZE
    #define configQUEUE_REGISTRY_SIZE           8
#endif



#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_vQueueDelete                    1

#ifndef configUSE_TRACE_FACILITY
    #define configUSE_TRACE_FACILITY            1
#endif

#ifndef configUSE_STATS_FORMATTING_FUNCTIONS 
    #define configUSE_STATS_FORMATTING_FUNCTIONS    1
#endif

#ifndef configGENERATE_RUN_TIME_STATS
    #define configGENERATE_RUN_TIME_STATS       0
#endif

//#define configUSE_PORT_OPTIMISED_TASK_SELECTION   0

#ifdef CORTEX_M0
    /* nothing to do */
#else
    #ifndef __NVIC_PRIO_BITS
        #define __NVIC_PRIO_BITS  4
    #endif
   
    #ifndef configPRIO_BITS 
        #define configPRIO_BITS  __NVIC_PRIO_BITS 
    #endif

    #ifndef configLIBRARY_LOWEST_INTERRUPT_PRIORITY
        #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   ((1<<configPRIO_BITS)-1)
    #endif
    
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

