#include "mcush.h"
#include "hal.h"



int cmd_beep( int argc, char *argv[] )
{
    mcush_opt_parser parser;
    mcush_opt opt;
    const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_VALUE, "freq", 'f', "frequency", "range 20~10000 (default 4000)", MCUSH_OPT_USAGE_REQUIRED | MCUSH_OPT_USAGE_VALUE_REQUIRED },
        { MCUSH_OPT_ARG, "ms", 0, 0, "duration (default 50ms)", MCUSH_OPT_USAGE_REQUIRED },
        { MCUSH_OPT_NONE } };
    int freq=-1, ms=-1;

    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "freq" ) == 0 )
            {
                if( ! shell_eval_int(opt.value, (int*)&freq) )
                    freq = -1;
            }
            else if( strcmp( opt.spec->name, "ms" ) == 0 )
            {
                if( ! shell_eval_int(opt.value, (int*)&ms) )
                    ms = -1;
            }
        }
        else
             STOP_AT_INVALID_ARGUMENT  
    }

    if( freq == -1 )
        freq = 4000;
    if( ms == -1 )
        ms = 50;

    if( !( (freq >= 20) && (freq <= 10000) ) )
        STOP_AT_INVALID_ARGUMENT  
    if( !( (ms > 0) && (ms <= 10000) ) )
        STOP_AT_INVALID_ARGUMENT  
   
    hal_beep_on( freq );
    vTaskDelay(ms*configTICK_RATE_HZ/1000);
    hal_beep_off();
    
    return 0;
}

shell_cmd_t cmd_tab_beep[] = {
{   0,  'b',  "beep",  cmd_beep, 
    "beep control",
    "beep -f <freq> <ms>"  },
{   CMD_END  } };




