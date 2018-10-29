#include "mcush.h"
#include "task_blink.h"

__attribute__((section(".init_script")))
//const char init_script[] = "\nsgpio -p2 -o0x3f -f1 -l\n5 6 0 0xc 0x14 0x24\n\nsgpio -r\n";
//const char init_script[] = "\nsgpio -p2 -o0x3f -f1 -l\n1 2 4 8 16 32\n\nsgpio -r\n";
const char init_script[] = "";


int main(void)
{
    mcush_init();
    //test_delay_us();
    //test_delay_ms();
    task_blink_init(); 
    mcush_start();
    while(1);
}

 
