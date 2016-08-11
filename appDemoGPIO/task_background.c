#include "mcush.h"
#include "task_background.h"

TaskHandle_t  task_background;
QueueHandle_t queue_background;

void task_background_entry(void *p)
{
    const uint32_t d=1000*configTICK_RATE_HZ/1000;

    while(1)
    {
        debug_led(0, -1);
        vTaskDelay( d );
    } 
}


void task_background_init(void)
{
    xTaskCreate(task_background_entry, (const char *)"background", TASK_BACKGROUND_STACK_SIZE, NULL, TASK_BACKGROUND_PRIORITY, &task_background);
    if( !task_background )
        halt("create background task");
}






