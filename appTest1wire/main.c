#include "mcush.h"
#include "task_1wire.h"


int main(void)
{ 
    mcush_init();
    task_1wire_init(); 
    mcush_start();
    while(1);
}

 
