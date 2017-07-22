#include "mcush.h"
#include "ds18b20.h"
#include "owhal.h"
#include "task_blink.h"

TaskHandle_t  task_ow;
extern device_t devices[NUM_DEVICES];

void task_ow_entry(void *p)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the GPIO Clock for OW */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Configure the GPIO pin for OW */
    GPIO_InitStructure.GPIO_Pin = (1 << OW_PIN);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(OW_GPIO, &GPIO_InitStructure);

    ds18b20_init();

    while (1)
    {
        taskENTER_CRITICAL();
        ds18b20_work();
        taskEXIT_CRITICAL();
    }
}

void task_ow_init(void)
{
    xTaskCreate(task_ow_entry, (const char *)"owT", 
                1024 / sizeof(portSTACK_TYPE),
                NULL, tskIDLE_PRIORITY, &task_ow);
    if( !task_ow )
        halt("create ow task");
    mcushTaskAddToRegistered((void*)task_ow);
}

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


int main(void)
{
    mcush_init();
    //while(1)
    //{
    //    for( i=1000; i; i-- )
    //        __delay_us(1000);
    //    hal_led_toggle( 0 );
    //}

    task_blink_init(); 
    task_ow_init(); 
    shell_add_cmd_table( cmd_tab_ow );
    mcush_start();
    while(1);
}


