#include "mcush.h"

#define USE_LUA


extern int lua_main (int argc, char **argv);

#ifdef USE_LUA
static shell_cmd_t cmd_tab[] = {
{   0, 'l', "lua", lua_main, 
    "run lua interpreter",
    "lua [fname]"  },
{   0,  0,  0,  0  } };
#endif


int main(void)
{ 
    mcush_init();
#ifdef USE_LUA
    shell_add_cmd_table( cmd_tab );
#endif
    mcush_start();
    while(1);
}

 
