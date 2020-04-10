/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

extern shell_cmd_t CMD_TAB[];
extern char _isdata;

__attribute__((section(".mcush_signature")))
__attribute__((used))
static const char mcush_signature[] = "<mcush>";


#if MCUSH_STACK_ALLOC_STATIC
StaticTask_t task_mcush_buffer;
StackType_t task_mcush_stack[MCUSH_STACK_SIZE/sizeof(portSTACK_TYPE)];
StaticTask_t task_idle_buffer;
StackType_t task_idle_stack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &task_idle_buffer;
    *ppxIdleTaskStackBuffer = task_idle_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif

#if configAPPLICATION_ALLOCATED_HEAP
/* TODO: Fix this, place ucHeap into .userheap area.
         Currently, the objcopy will generate incorrect 
         binary file attached with unecessary zero data */
//__attribute__((section(".userheap")))
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];  /* for heap_4 */
#endif

TaskHandle_t  task_mcush;
static uint8_t mcush_inited = 0;

void mcush_init(void)
{
    if( mcush_inited )
        return;

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

#if MCUSH_STACK_ALLOC_STATIC
    task_mcush = xTaskCreateStatic((TaskFunction_t)shell_run, (const char *)"mcushT", 
                MCUSH_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, MCUSH_PRIORITY, task_mcush_stack, &task_mcush_buffer);
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
    vTaskStartScheduler();
    halt("stopped");
}
