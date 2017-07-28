#include "mcush.h"
#include "task_blink.h"

#define USE_LUA


extern int lua_main (int argc, char **argv);

#ifdef USE_LUA
static const shell_cmd_t cmd_tab[] = {
{   0, 'l', "lua", lua_main, 
    "run lua interpreter",
    "lua [fname]"  },
{   CMD_END  } };
#endif


int main(void)
{ 
    mcush_init();
    task_blink_init();
#ifdef USE_LUA
    shell_add_cmd_table( cmd_tab );
#endif
    mcush_start();
    while(1);
}

 
