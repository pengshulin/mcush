#include "mcush.h"
#include "task_blink.h"



extern shell_cmd_t cmd_tab_beep[];



int main(void)
{ 
    mcush_init();
    shell_add_cmd_table( cmd_tab_beep );
    task_blink_init(); 
    mcush_start();
    while(1);
}

 
