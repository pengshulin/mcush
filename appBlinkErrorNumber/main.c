#include "mcush.h"
#include "task_blink.h"

__attribute__((section(".init_script")))
const char init_script[] = "";


int main(void)
{
    mcush_init();
    //test_delay_us();
    task_blink_init(); 
    mcush_start();
    while(1);
}

 
