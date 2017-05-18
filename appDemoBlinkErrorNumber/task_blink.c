#include "mcush.h"
#include "task_blink.h"

TaskHandle_t  task_blink;
QueueHandle_t queue_blink;

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
    int n = -1;
    if( argc == 1 )
    {
        shell_printf( "%d\n", _errno );
        return 0;
    }
    else if( argc == 2 )
    {
        n = atol(argv[1]);
        set_errno(n);
        return 0;
    } 
    else
        return -1; 
}

const shell_cmd_t cmd_tab[] = {
{   0, 'e', "error",  cmd_error, 
    "set error number",
    "error <number>"  },
{   0,  0,  0,  0  } };



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


