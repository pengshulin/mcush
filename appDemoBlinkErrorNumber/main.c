#include "mcush.h"
#include "task_blink.h"

//#if USE_CMD_BEEP
//__attribute__((section(".init_script")))
//const char init_script[] = "beep\n";
//#endif


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

 
