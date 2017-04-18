#include "mcush.h"
#include "task_adc.h"


int main(void)
{ 
    mcush_init();
    task_adc_init(); 
    mcush_start();
    while(1);
}

 
