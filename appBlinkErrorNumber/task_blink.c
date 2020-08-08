/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_blink.h"

#ifndef LED_NORMAL
    #define LED_NORMAL  0
#endif

#ifndef LED_ERROR
    #define LED_ERROR   LED_NORMAL
#endif

static int _errno = 0;

int get_errno(void)
{
    return _errno;
}

int set_errno(int num)
{
    int old = _errno;
    _errno = num;
    return old;
}


int cmd_error( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_SWITCH, MCUSH_OPT_USAGE_REQUIRED,
          's', shell_str_stop, 0, shell_str_stop },
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED,
          0, "errno", 0, "0~100000000" },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    int new = -1;
    uint8_t stop=0;

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( strcmp( opt.spec->name, "errno" ) == 0 )
            {
                if( ! parse_int(opt.value, (int*)&new) )
                    return -1;
            }
            else if( STRCMP( opt.spec->name, shell_str_stop ) == 0 )
                stop = 1;
        }
        else
            STOP_AT_INVALID_ARGUMENT
    }

    if( stop )
    {
        hal_led_clr( LED_ERROR );
        set_errno(-1);
        return 0;
    }

    if( new == -1 )
    {
        if( _errno < 0 )
            shell_write_line( "stop" );
        else
            shell_printf( "%d\n", _errno );
        return 0;
    }

    if( (new < 0) || (new > 100000000) )
    {
        shell_write_line( "out of range" );
        return 1;
    }
    set_errno(new);
    return 0;
}

static const shell_cmd_t cmd_tab_blink[] = {
{   0, 'e', "error",  cmd_error,
    "error number",
    "e <num>"  },
{   CMD_END  } };


#define DELAY_A  OS_TICKS_MS(200)  /* on for 1~9 */
#define DELAY_B  OS_TICKS_MS(500)  /* off for 1~9 */
#define DELAY_C  OS_TICKS_MS(1000)  /* on for 0 */
#define DELAY_D  OS_TICKS_MS(1000)  /* delay cycle */

static void blink_digit( int digit, int led )
{
    int i;

    if( digit < 0 )
        return;

    if( digit )
    {
        for( i=digit; i; i-- )
        {
            hal_led_set(led);
            os_task_delay(DELAY_A);
            hal_led_clr(led);
            os_task_delay(DELAY_A);
            /* NOTE:  if the blink task is the only running task, in release mode,
            cpu may reboot by the wdg (especially for long digits error code),
            so clear the wdg on every blink */
            hal_wdg_clear();
        }
    }
    else
    {
        hal_led_set(led);
        os_task_delay(DELAY_C);
        hal_led_clr(led);
        hal_wdg_clear();
    }
}


void task_blink_entry(void *p)
{
    int i, digit, pos, skip;

    (void)p;
    while( 1 )
    {
        hal_wdg_clear();
        i = _errno;
        if( i < 0 )
        {
            /* errno < 0, disable mode */
        }
        else
        {
            if( i == 0 )
            {
                /* no error */
                blink_digit( 0, LED_NORMAL );
            }
            else
            {
                /* errno from 1 ~ 100000000 */
                for( pos=9, skip=1; pos; pos-- )
                {
                    digit = (i / 100000000) % 10;
                    i -= digit * 100000000;
                    i *= 10;
                    if( skip && !digit && (pos != 1) )
                        continue;
                    blink_digit( digit, LED_ERROR );
                    skip = 0;
                    if( pos != 1 )
                        os_task_delay(DELAY_B);
                }
            }
        }
        os_task_delay( DELAY_D );
    }
}


void task_blink_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( blink, TASK_BLINK_STACK_SIZE );
    task = os_task_create_static( "blinkT", task_blink_entry, 0, TASK_BLINK_STACK_SIZE, TASK_BLINK_PRIORITY, &static_task_buffer_blink );
#else
    task = os_task_create( "blinkT", task_blink_entry, 0, TASK_BLINK_STACK_SIZE, TASK_BLINK_PRIORITY );
#endif
    if( task == NULL )
        halt("create blink task");

    shell_add_cmd_table( cmd_tab_blink );
}

