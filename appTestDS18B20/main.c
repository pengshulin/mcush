#include "mcush.h"
#include "task_blink.h"
#include "task_ow.h"


int main(void)
{
    mcush_init();
    task_blink_init(); 
    task_ow_init(); 
    mcush_start();
    while(1);
}


