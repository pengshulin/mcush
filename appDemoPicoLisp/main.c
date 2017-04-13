#include "mcush.h"


extern int picolisp_main (int argc, char **argv);

static shell_cmd_t cmd_tab[] = {
{   "pil", "picolisp", picolisp_main, 
    "run picolisp interpreter",
    "pil <fname>"  },
{   0,  0,  0,  0  } };



int main(void)
{ 
    mcush_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
