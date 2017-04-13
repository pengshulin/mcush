#include "mcush.h"


extern int tinyscheme_main (int argc, char **argv);

static shell_cmd_t cmd_tab[] = {
{   "ts", "tinyscheme", tinyscheme_main, 
    "run tinyscheme interpreter",
    "ts <fname>"  },
{   0,  0,  0,  0  } };



int main(void)
{ 
    mcush_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
