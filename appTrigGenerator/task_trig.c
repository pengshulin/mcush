#include "mcush.h"
#include "task_trig.h"

TaskHandle_t  task_trig;
QueueHandle_t queue_trig;

int trig_width_ms=4;
int trig_delay_ms=100;
int trig_counter=-1;
int trig_running=1;

int cmd_trig( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "command", 'c', "command name", "info|start|stop|set_width|set_delay|set_counter", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_VALUE, "val", 'v', "value", "setting value", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_NONE } };
    char *command=0;
    int val=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "command" ) == 0 )
            {
                if( opt.value )
                    command = (char*)opt.value;
            }
            else if( strcmp( opt.spec->name, "val" ) == 0 )
            {
                if( ! shell_eval_int(opt.value, (int*)&val) )
                    val = -1;
            }
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( !command || strcmp( command, "info") == 0 )
    {
        shell_printf( "%s  width_ms:%d  delay_ms:%d  counter:%d\n", 
                        trig_running ? "RUNNING" : "STOPPED",
                        trig_width_ms, trig_delay_ms, trig_counter );
    }
    else if( strcmp( command, "start" ) == 0 )
    {
        if( trig_counter == 0 )
            trig_counter = -1;
        trig_running = 1;
    }
    else if( strcmp( command, "stop" ) == 0 )
    {
        trig_counter = 0;
        trig_running = 0;
    }
    else if( strcmp( command, "set_width" ) == 0 )
    {
        if( val > 0 )
            trig_width_ms = val; 
    }
    else if( strcmp( command, "set_delay" ) == 0 )
    {
        if( val > 0 )
            trig_delay_ms = val; 
    }
    else if( strcmp( command, "set_counter" ) == 0 )
    {
        trig_counter = val; 
        if( trig_counter == 0 )
            trig_running = 0;
    }
    else
        STOP_AT_INVALID_ARGUMENT  
    
    return 0;
}

static shell_cmd_t cmd_tab[] = {
{   0,  't',  "trig",  cmd_trig, 
    "trigger control",
    "trig -c <command> -v <value>"  },
{   CMD_END  } };


#define PORT  2
#define PIN   (1<<1)


void task_trig_entry(void *p)
{
    shell_add_cmd_table( cmd_tab );

    hal_gpio_set_output(PORT, PIN);
    hal_gpio_clr(PORT, PIN);

    while( 1 )
    {
        if( trig_running )
        {
            if( trig_counter != 0 )
            {
                hal_led_set(0);
                hal_gpio_set(PORT, PIN);
                vTaskDelay(trig_width_ms*configTICK_RATE_HZ/1000);
                hal_gpio_clr(PORT, PIN);
                hal_led_clr(0);
                vTaskDelay(trig_delay_ms*configTICK_RATE_HZ/1000);
                if( trig_counter > 0 )
                {
                    trig_counter -= 1;
                    if( trig_counter == 0 )
                        trig_running = 0; 
                }
            }
            else
                trig_running = 0; 
        }
        else
            vTaskDelay(200*configTICK_RATE_HZ/1000);
    }
}


void task_trig_init(void)
{
    xTaskCreate(task_trig_entry, (const char *)"trigT", TASK_TRIG_STACK_SIZE, NULL, TASK_TRIG_PRIORITY, &task_trig);
    if( !task_trig )
        halt("create trig task");
    mcushTaskAddToRegistered((void*)task_trig);
}


