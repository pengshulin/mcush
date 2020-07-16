/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

extern shell_cmd_t CMD_TAB[];
extern char _isdata;

__attribute__((section(".mcush_signature")))
__attribute__((used))
static const char mcush_signature[] = "<mcush>";


#if configSUPPORT_STATIC_ALLOCATION
StaticTask_t task_mcush_data;
StackType_t task_mcush_buffer[MCUSH_STACK_SIZE/sizeof(portSTACK_TYPE)];

StaticTask_t task_idle_data;
StackType_t task_idle_buffer[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &task_idle_data;
    *ppxIdleTaskStackBuffer = task_idle_buffer;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if configUSE_TIMERS
StaticTask_t task_timer_data;
StackType_t task_timer_buffer[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &task_timer_data;
    *ppxTimerTaskStackBuffer = task_timer_buffer;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif


#if configAPPLICATION_ALLOCATED_HEAP
/* TODO: Fix this, link ucHeap into .userheap area.
         Currently, the objcopy will generate incorrect 
         binary file attached with unecessary zero data */
//__attribute__((section(".userheap")))
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  /* for heap_4 */
#endif
#endif

TaskHandle_t  task_mcush;
static uint8_t mcush_inited = 0;


__attribute__((weak)) void hal_pre_init(void)
{
}


void mcush_init(void)
{
    if( mcush_inited )
        return;

    hal_pre_init();
    if( !hal_init() )
        halt("hal init");

    if( !shell_init( &CMD_TAB[0], _isdata ? &_isdata : 0 ) )
        halt("shell init");

#if MCUSH_ROMFS
    mcush_mount( "r", &mcush_romfs_driver );
#endif
#if MCUSH_FCFS
    mcush_mount( "c", &mcush_fcfs_driver );
#endif
#if MCUSH_SPIFFS
    mcush_mount( "s", &mcush_spiffs_driver );
#endif
#if MCUSH_FATFS
    mcush_mount( "f", &mcush_fatfs_driver );
#endif

#if configSUPPORT_STATIC_ALLOCATION
    task_mcush = xTaskCreateStatic((TaskFunction_t)shell_run, (const char *)"mcushT", 
                MCUSH_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, MCUSH_PRIORITY, task_mcush_buffer, &task_mcush_data);
#else
    xTaskCreate((TaskFunction_t)shell_run, (const char *)"mcushT", 
                MCUSH_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, MCUSH_PRIORITY, &task_mcush);
#endif
    if( !task_mcush )
        halt("mcush task create");
    mcush_inited = 1;
}


void mcush_start(void)
{
    if( !mcush_inited )
        mcush_init();
    os_start(); //vTaskStartScheduler();
    halt("stopped");
}
