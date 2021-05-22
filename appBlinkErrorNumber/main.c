#include "mcush.h"
#include "task_blink.h"
#include "task_swdg.h"


int main(void)
{
    mcush_init();
    task_blink_init(); 
    task_swdg_init(); 
    mcush_start();
    while(1);
}

 
