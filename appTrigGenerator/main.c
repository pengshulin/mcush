#include "mcush.h"
#include "task_trig.h"

int main(void)
{ 
    mcush_init();
    task_trig_init(); 
    mcush_start();
    while(1);
}

 
