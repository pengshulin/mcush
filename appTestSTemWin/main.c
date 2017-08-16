#include "mcush.h"
#include "task_blink.h"

#define WHITE  0xFFFF
#define BLACK  0x0000

int cmd_lcd( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, "loop", 'l', 0, "loop mode", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int loop=0, loop_delay=-1;
    TickType_t tick;
    char c;
    int channel=-1;
    int i;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "loop" ) == 0 )
                loop=1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }; 

    hal_lcd_backlight(1);
    hal_lcd_clrscr(WHITE);

    hal_lcd_circle( 100, 100, 20, BLACK );
    hal_lcd_rectangle( 100, 200, 120, 230, BLACK );
    for(i=0; i<200; i++)
    {
        hal_lcd_pixel( i, i, BLACK);
        hal_lcd_pixel( i, 200-i, BLACK);
    }

    for( i=10; i<110; i+=4 )
    {
        hal_lcd_vline( i, 10, i, BLACK );
        hal_lcd_hline( i, 10, i, BLACK );
    }

    hal_lcd_rectangle( 300, 220, 319, 239, BLACK );
    for( i=130; i<240; i+=5 )
    {
        hal_lcd_vline( i, 10, i/2, BLACK );
    }
    hal_lcd_rectangle( 30, 40, 180, 60, BLACK );

    return 0;
}


static const shell_cmd_t cmd_tab[] = {
{   0, 0, "lcd", cmd_lcd, 
    "lcd test",
    "lcd "  },
{   CMD_END  } };


int main(void)
{
    mcush_init();
    task_blink_init(); 
    shell_add_cmd_table( cmd_tab );
    mcush_start();
    while(1);
}

 
