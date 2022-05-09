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

    (void)p;
    if( hal_can_init() == 0 )
        os_task_delete(NULL);
    //set_errno(-1);

    while( 1 )
    {
        if( hal_can_read( &msg, -1 ) )
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
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( can, TASK_CAN_STACK_SIZE );
    task = os_task_create_static( "canT", task_can_entry, NULL,
                TASK_CAN_STACK_SIZE, TASK_CAN_PRIORITY, 
                &static_task_buffer_can );
#else
    task = os_task_create( "canT", task_can_entry, NULL,
                TASK_CAN_STACK_SIZE, TASK_CAN_PRIORITY );
#endif
    if( task == NULL )
        halt("create test task");
}

