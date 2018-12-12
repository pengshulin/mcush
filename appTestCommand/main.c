#include "mcush.h"
#include "task_blink.h"
#include "task_command.h"


int main(void)
{
    mcush_init();
    task_blink_init(); 
    task_command_init(); 
    mcush_start();
    while(1);
}

 
