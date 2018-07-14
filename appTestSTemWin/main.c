#include "mcush.h"
#include "task_blink.h"
#include "GUI.h"
#include "timers.h"

#define WHITE  0xFFFF
#define BLACK  0x0000

extern void GUIDEMO_Main(void);

static TimerHandle_t timer_touch;
#define TOUCH_SCAN_MS  50

static const GUI_COLOR colorBW[] = {
    //0xFFFFFF, 0x000000
    //0x000000, 0xFFFFFF
    //0xFF0000, 0x000000
    GUI_WHITE, GUI_RED
};

/************************** B/W PALETTE DEFINATION **************************/
static const GUI_LOGPALETTE paletteBW = {
    2,  /* number of entries */
    0,  /* No transparency */
    colorBW,
};

static const unsigned char img_caution_data[] = {
    ________,________,________,
    ________,XXX_____,________,
    _______X,XXXX____,________,
    _______X,X_XX____,________,
    ______XX,___XX___,________,
    _____XXX,___XXX__,________,
    _____XX_,XXX_XX__,________,
    ____XX__,XXX__XX_,________,
    ___XXX__,XXX__XXX,________,
    ___XX___,XXX___XX,________,
    __XX____,XXX____X,X_______,
    __XX____,XXX____X,X_______,
    _XX_____,________,XX______,
    XXX_____,XXX_____,XXX_____,
    XX______,XXX_____,_XX_____,
    XX______,________,_XX_____,
    _XXXXXXX,XXXXXXXX,XX______,
    __XXXXXX,XXXXXXXX,X_______,
    ________,________,________,
};

const GUI_BITMAP img_caution = {
    19, 19,  /* width, height */
    3, 1,  /* bytes_per_line, bits_per_pixel */
    img_caution_data, &paletteBW
};

void test_gui(void)
{
    GUI_RECT rect;

    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
    GUI_UC_SetEncodeUTF8();

    rect.x0 = 10;
    rect.y0 = 10;
    rect.x1 = 100;
    rect.y1 = 100;
    GUI_SetColor(GUI_RED);
    GUI_FillRect(rect.x0, rect.y0, rect.x1, rect.y1);
    rect.x0 += 5;
    rect.y0 += 5;
    rect.x1 -= 5;
    rect.y1 -= 5;
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(rect.x0, rect.y0, rect.x1, rect.y1);
    rect.x0 += 5;
    rect.y0 += 5;
    rect.x1 -= 5;
    rect.y1 -= 5;
    GUI_SetColor(GUI_GREEN);
    GUI_FillRect(rect.x0, rect.y0, rect.x1, rect.y1);

    GUI_SetColor(GUI_BLUE);
    GUI_SetBkColor(GUI_CYAN);
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_DispStringInRect( "Test", &rect, GUI_TA_VCENTER | GUI_TA_HCENTER );
 
    //GUI_SetDrawMode(GUI_TM_TRANS);
    GUI_SetDrawMode(GUI_TM_XOR);
    GUI_DrawBitmap( &img_caution, 100, 100 );
    GUI_DrawBitmap( &img_caution, 70, 70 );
    GUI_SetDrawMode(GUI_TM_NORMAL);
}


void timer_touch_hook(TimerHandle_t *xTimer)
{
    GUI_TOUCH_Exec();
}


int cmd_lcd( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'l', "loop", 0, "loop mode" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'i', "init", 0, "init" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          't', "test", 0, "test" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'd', "demo", 0, "demo" },
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED, 
          'h', "hal_test", 0, "hal api test" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    uint8_t loop=0, init=0, test=0, hal_test=0, demo=0;
    int i;
    
    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );

    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "loop" ) == 0 )
                loop=1;
            if( strcmp( opt.spec->name, "init" ) == 0 )
                init=1;
            if( strcmp( opt.spec->name, "test" ) == 0 )
                test=1;
            if( strcmp( opt.spec->name, "hal_test" ) == 0 )
                hal_test=1;
            if( strcmp( opt.spec->name, "demo" ) == 0 )
                demo = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT  
    }; 

    if( init )
    {

        GUI_Init();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_WHITE);
        GUI_UC_SetEncodeUTF8();

        if( ! timer_touch )
        {
            timer_touch = xTimerCreate( "touch", TOUCH_SCAN_MS*configTICK_RATE_HZ/1000, pdTRUE, 0, 
                                        (TimerCallbackFunction_t)timer_touch_hook );
            xTimerStart( timer_touch, 0 );
        }
    }

    if( hal_test )
    {
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
    }

    if( test )
    {
        test_gui();
    }

    if( demo )
        GUIDEMO_Main();


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

 
