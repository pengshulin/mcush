#include "mcush.h"
#include "task_background.h"


int main(void)
{ 
    mcush_init();
    task_background_init(); 
    mcush_start();
    while(1);
}

 
