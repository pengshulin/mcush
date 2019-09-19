/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_blink.h"
#include "task_logger.h"


extern int cmd_lua (int argc, char **argv);


static const shell_cmd_t cmd_tab[] = {
{   0, 0, "lua", cmd_lua, 
    "run lua interpreter",
    "lua [fname]"  },
{   CMD_END  } };



int main(void)
{ 
    mcush_init();
    task_blink_init();
    task_logger_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
