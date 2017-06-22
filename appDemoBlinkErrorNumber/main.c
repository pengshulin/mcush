#include "mcush.h"
#include "task_blink.h"


int main(void)
{
#if defined(MCUSH_NON_OS)
    while(1)
    {
        task_blink_entry();
        task_mcush_entry();
    }
#else
    mcush_init();
    task_blink_init(); 
    mcush_start();
    while(1);
#endif
}

 
