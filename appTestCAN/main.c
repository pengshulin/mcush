#include "mcush.h"
#include "task_blink.h"
#include "task_logger.h"
#include "task_can.h"


int main(void)
{
    mcush_init();
    task_blink_init(); 
    task_logger_init(); 
    task_can_init(); 
    mcush_start();
    while(1);
}

 
