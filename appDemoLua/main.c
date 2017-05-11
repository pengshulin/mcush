#include "mcush.h"


extern int lua_main (int argc, char **argv);

static shell_cmd_t cmd_tab[] = {
{   "l", "lua", lua_main, 
    "run lua interpreter",
    "lua <fname>"  },
{   0,  0,  0,  0  } };



int main(void)
{ 
    mcush_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
