/* Peng Shulin <trees_peng@163.com> 2020
 * Shanghai Linkong Software Technologies Co., Ltd.
 */
#include "mcush.h"
#include "task_blink.h"
#include "task_can.h"
#include "task_logger.h"


void task_can_entry(void *p)
{
    can_message_t msg;

    if( hal_can_init() == 0 )
        vTaskDelete(0);
    vTaskDelete(0);

    //set_errno(-1);

    while( 1 )
    {
        if( hal_can_read( &msg, portMAX_DELAY ) )
        {
            /* TODO: process received message */
            /* LED set/clr/toggle command in two bytes */
            if( (msg.id == 0x1) && (msg.len >= 2) )
            {
                switch( msg.data[0] )
                {
                case 1:
                    hal_led_set( msg.data[1] );
                    break;
                case 0:
                    hal_led_clr( msg.data[1] );
                    break;
                default:
                    hal_led_toggle( msg.data[1] );
                    break;
                }
            }
        }
    }
}


void task_can_init(void)
{
    TaskHandle_t task;

    (void)xTaskCreate(task_can_entry, (const char *)"canT", 
                TASK_CAN_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_CAN_PRIORITY, &task);
    if( task == NULL )
        halt("create test task");
}

