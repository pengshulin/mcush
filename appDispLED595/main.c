#include "mcush.h"
#include "task_disp_led595.h"
#include "task_blink.h"


int main(void)
{ 
    mcush_init();
    task_disp_led595_init(); 
    task_blink_init(); 
    mcush_start();
    while(1);
}

 
