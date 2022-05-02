#include "mcush.h"
#include "task_blink.h"
#include "scheme-private.h"

int test_tinyscheme(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    scheme sc;

    if( scheme_init(&sc) )
    {
        scheme_deinit(&sc);
    }
    return 0;
}

int stat(void)
{
    return 1;
}

extern int tinyscheme_main (int argc, char **argv);

static shell_cmd_t cmd_tab[] = {
{   0, 't', "ts", test_tinyscheme, 
//{   0, 't', "ts", tinyscheme_main, 
    "run tinyscheme interpreter",
    "ts <fname>"  },
{   CMD_END  } };



int main(void)
{ 
    mcush_init();
    task_blink_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
