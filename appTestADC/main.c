#include "mcush.h"
#include "task_adc.h"
#include "task_disp_led595.h"


int main(void)
{ 
    mcush_init();
    task_adc_init(); 
    task_disp_led595_init(); 
    mcush_start();
    while(1);
}

 
