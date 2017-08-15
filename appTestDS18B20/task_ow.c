#include "mcush.h"
#include "hal_ow.h"
#include "ds18b20.h"
#include "task_ow.h"
#include "task_disp_led595.h"

extern device_t devices[NUM_DEVICES];

TaskHandle_t task_ow;


int cmd_ow( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "loop", 'l', 0, "default 1s", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    uint32_t tick;
    int loop = 0;
    int i;
    char c;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "loop" ) == 0 )
                 loop = 1; 
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

loop_entry:

    for( i=0; i<NUM_DEVICES; i++ )
    {
        if( !devices[i].serial[0] )
            continue;
        shell_printf( "%02X%02X%02X%02X%02X%02X%02X%02X %.1f\n",
            devices[i].serial[0],  
            devices[i].serial[1],  
            devices[i].serial[2],  
            devices[i].serial[3],  
            devices[i].serial[4],  
            devices[i].serial[5],  
            devices[i].serial[6],  
            devices[i].serial[7],  
            devices[i].ftemp );
    }
   
    if( loop )
    {
        tick = xTaskGetTickCount();
        while( xTaskGetTickCount() < tick + 1000*configTICK_RATE_HZ/1000 )
        {
            if( shell_driver_read_char_blocked(&c, 10*configTICK_RATE_HZ/1000) != -1 )
                if( c == 0x03 ) /* Ctrl-C for stop */
                    return 0;
        }
        goto loop_entry;
    }
 
    return 0;
}


const shell_cmd_t cmd_tab_ow[] = {
{   0, 0, "ow",  cmd_ow, 
    "1wire",
    "ow [-l]" },
{   CMD_END  } };


void task_ow_entry(void *p)
{
    ds18b20_init();
    while (1)
    {
        taskENTER_CRITICAL();
        hal_led_set(1);
        ds18b20_work();
        hal_led_clr(1);
        taskEXIT_CRITICAL();
        vTaskDelay( 500*configTICK_RATE_HZ/1000 );
    }
}


void task_display_temperature_entry(void *p)
{
    char buf[16];
    while (1)
    {
        if( devices[0].serial[0] )
            sprintf( buf, "%.1fc", devices[0].ftemp );
        else
            strcpy( buf, "----" );
        update_disp_buf( buf );
        vTaskDelay( 1000*configTICK_RATE_HZ/1000 );
    }
}


void task_ow_init(void)
{
    hal_ow_init();
    shell_add_cmd_table( cmd_tab_ow );
    xTaskCreate(task_ow_entry, (const char *)"owT", 
                TASK_OW_STACK_SIZE,
                NULL, TASK_OW_PRIORITY, &task_ow);
    if( !task_ow )
        halt("create ow task");
    mcushTaskAddToRegistered((void*)task_ow);
    xTaskCreate(task_display_temperature_entry, (const char *)"dispTprT", 
                200, NULL, tskIDLE_PRIORITY, 0);
}

