/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"


TaskHandle_t  task_mcush;
//QueueHandle_t queue_mcush;
static uint8_t mcush_inited = 0;

extern shell_cmd_t CMD_TAB[];


void mcush_init(void)
{
    if( mcush_inited )
        return;

    if( !hal_init() )
        halt("hal init");

    //queue_mcush = xQueueCreate(MCUSH_QUEUE_SIZE, (unsigned portBASE_TYPE)sizeof(mcush_message_t));
    //if( !queue_mcush )
    //    halt("mcush queue create");
    if( !shell_init( &CMD_TAB[0] ) )
        halt("shell init");

    xTaskCreate((TaskFunction_t)shell_run, (const char *)"mcush", MCUSH_STACK_SIZE / sizeof(portSTACK_TYPE), NULL, MCUSH_PRIORITY, &task_mcush);
    if( !task_mcush )
        halt("mcush task create");

    mcush_inited = 1;
}


void mcush_start(void)
{
    if( !mcush_inited )
        mcush_init();
    vTaskStartScheduler();
    halt("mcush_stopped");
}

