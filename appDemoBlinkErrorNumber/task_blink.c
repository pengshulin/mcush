#include "mcush.h"
#include "task_blink.h"


const int errled = 0;
int _errno = 0;

int set_errno(int new)
{
    int old=_errno;
    _errno = new;
    return old;
}


int cmd_error( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, "errno", 0, 0, "error number from 0 to 999", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int new = -1;

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "errno" ) == 0 )
            {
                if( ! shell_eval_int(opt.value, (int*)&new) )
                    new = 0; 
            }
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( new == -1 )
    {
        shell_printf( "%d\n", _errno );
        return 0;
    }
    
    if( (new < 0) || (new > 999) )
        goto failed;
    if( (new >= 10) && ((new % 10) == 0) )
        goto failed;
    if( (new >= 100) && (((new/10) % 10) == 0) )
        goto failed;
    set_errno(new);
    return 0;
failed:
    shell_printf( "zero or range 1~999(cannot contain 0)\n" );
    return 1;
}

const shell_cmd_t cmd_tab[] = {
{   0, 'e', "error",  cmd_error, 
    "set error number",
    "error <number>"  },
{   CMD_END  } };


#if !defined(MCUSH_NON_OS)

#define BLINK_ERROR_DELAY_A  200 
#define BLINK_ERROR_DELAY_B  500 
#define BLINK_ERROR_DELAY_C  2000 
void blink_errorno(void)
{
    int a,b,c,i;
    a = (_errno % 1000) / 100;
    b = (_errno % 100) / 10;
    c = _errno % 10;

    if( a )
    {
        for( i=a; i; i-- )
        {
            hal_led_set(errled);
            vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
            hal_led_clr(errled);
            vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
        }
        vTaskDelay(BLINK_ERROR_DELAY_B*configTICK_RATE_HZ/1000);
    }

    if( b )
    {
        for( i=b; i; i-- )
        {
            hal_led_set(errled);
            vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
            hal_led_clr(errled);
            vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
        }
        vTaskDelay(BLINK_ERROR_DELAY_B*configTICK_RATE_HZ/1000);
    }

    for( i=c; i; i-- )
    {
        hal_led_set(errled);
        vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
        hal_led_clr(errled);
        vTaskDelay(BLINK_ERROR_DELAY_A*configTICK_RATE_HZ/1000);
    }
}



TaskHandle_t  task_blink;

void task_blink_entry(void *p)
{
    shell_add_cmd_table( cmd_tab );

    while( 1 )
    {
        if( _errno )
        {
            blink_errorno();
            vTaskDelay(2000*configTICK_RATE_HZ/1000);
        }
        else
        {
            hal_led_toggle(0);
            vTaskDelay(1000*configTICK_RATE_HZ/1000);
        }
    }
}


void task_blink_init(void)
{
    xTaskCreate(task_blink_entry, (const char *)"blinkT", 
                TASK_BLINK_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_BLINK_PRIORITY, &task_blink);
    if( !task_blink )
        halt("create blink task");
    mcushTaskAddToRegistered((void*)task_blink);
}
#else

event_t event_blink = EVT_INIT;




void task_blink_entry(void)
{
    if( event_blink & EVT_INIT )
    {
        shell_add_cmd_table( cmd_tab );
        event_blink &= ~EVT_INIT;
    }
    else if( event_blink & EVT_TIMER )
    {
        event_blink &= ~EVT_TIMER;
    //    if( _errno )
    //    {
    //        blink_errorno();
    //        vTaskDelay(2000*configTICK_RATE_HZ/1000);
    //    }
    //    else
    //    {
    //        hal_led_toggle(0);
    //        vTaskDelay(1000*configTICK_RATE_HZ/1000);
    //    }
    }
}



#endif
