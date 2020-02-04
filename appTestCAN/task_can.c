/* Peng Shulin <trees_peng@163.com> 2020
 * Shanghai Linkong Software Technologies Co., Ltd.
 */
#include "mcush.h"
#include "task_blink.h"
#include "task_can.h"
#include "task_logger.h"


void task_can_entry(void *p)
{
    //can_filter_t filter;
    can_message_t msg;

    if( hal_can_init() == 0 )
        vTaskDelete(0);

    /* reset filter #0 to receive only std_id==0 */
    //filter.std_id = 0;
    //filter.std_id_mask = 0x3FF;
    //filter.ext = 0;
    //filter.remote = 0;
    //hal_can_filter_set( 0, &filter, 1 );
        
    //set_errno(-1);

    while( 1 )
    {
        vTaskDelay( configTICK_RATE_HZ/2 );

        while( hal_can_read( &msg, portMAX_DELAY ) )
        {
            /* TODO: process received message */
            /* LED set/clr command in two bytes */
            if( (msg.id == 0x1) && (msg.len == 2) )
            {
               if( msg.data[0] )
                   hal_led_set( msg.data[1] );
               else
                   hal_led_clr( msg.data[1] );
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

