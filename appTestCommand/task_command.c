/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_command.h"


int cmd_led( int argc, char *argv[] );

void task_command_entry(void *p)
{
    char *arg_led_set[] = {"led", "-i", "1", "-s"};
    char *arg_led_clr[] = {"led", "-i", "1", "-c"};

    while( 1 )
    {
        cmd_led( 4, arg_led_set );
        shell_call( "led", "-i", "2", "-s", 0 );
        vTaskDelay( configTICK_RATE_HZ );
        cmd_led( 4, arg_led_clr );
        shell_call( "led", "-i", "2", "-c", 0 );
        vTaskDelay( configTICK_RATE_HZ );
    }
}


void task_command_init(void)
{
    TaskHandle_t task_command;

    //shell_add_cmd_table( cmd_tab_blink );

    (void)xTaskCreate(task_command_entry, (const char *)"cmdT", 
                TASK_COMMAND_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_COMMAND_PRIORITY, &task_command);
    if( task_command == NULL )
        halt("create command task");
    mcushTaskAddToRegistered((void*)task_command);
}

