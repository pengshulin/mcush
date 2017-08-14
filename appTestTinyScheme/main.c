#include "mcush.h"
#include "task_blink.h"
#include "scheme-private.h"

void test_tinyscheme(int argc, char **argv)
{
    scheme sc;
    if( scheme_init(&sc) )
    {
        scheme_deinit(&sc);
    }
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
{   0,  0,  0,  0  } };



int main(void)
{ 
    mcush_init();
    task_blink_init();
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
