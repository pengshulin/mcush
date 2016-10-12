#include "mcush.h"
#include "task_background.h"

TaskHandle_t  task_background;
QueueHandle_t queue_background;

const int errled = 0;
int _errno = 0;

int set_errno(int new)
{
    int old=_errno;
    _errno = new;
    return old;
}

void blink_errorno(void)
{
    int a,b,c,i;
    a = _errno / 100;
    b = (_errno % 100) / 10;
    c = _errno % 10;

    if( a )
    {
        for( i=a; i; i-- )
        {
            hal_led_set(errled);
            vTaskDelay(100*configTICK_RATE_HZ/1000);
            hal_led_clr(errled);
            vTaskDelay(100*configTICK_RATE_HZ/1000);
        }
        vTaskDelay(500*configTICK_RATE_HZ/1000);
    }

    if( b )
    {
        for( i=b; i; i-- )
        {
            hal_led_set(errled);
            vTaskDelay(100*configTICK_RATE_HZ/1000);
            hal_led_clr(errled);
            vTaskDelay(100*configTICK_RATE_HZ/1000);
        }
        vTaskDelay(500*configTICK_RATE_HZ/1000);
    }

    for( i=c; i; i-- )
    {
        hal_led_set(errled);
        vTaskDelay(100*configTICK_RATE_HZ/1000);
        hal_led_clr(errled);
        vTaskDelay(100*configTICK_RATE_HZ/1000);
    }
}


void task_background_entry(void *p)
{
    set_errno( 123 );

    while( 1 )
    {
        blink_errorno();
        vTaskDelay(2000*configTICK_RATE_HZ/1000);
    }
}


void task_background_init(void)
{
    xTaskCreate(task_background_entry, (const char *)"background", TASK_BACKGROUND_STACK_SIZE, NULL, TASK_BACKGROUND_PRIORITY, &task_background);
    if( !task_background )
        halt("create background task");
}


