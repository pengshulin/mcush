#include "mcush.h"
#include "task_disp_lcd1602.h"


int main(void)
{ 
    mcush_init();
    task_disp_lcd1602_init(); 
    mcush_start();
    while(1);
}

 
