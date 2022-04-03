/* Blinker task, blink LED to indicates a global error number
 *
 * The task runs as an auxiliary service in MCUSH micro-kerenl RTOS environment,
 * it aims to help you debug your applications.
 * To use this service task, include task_blink.h file in your application
 * and call task_blink_init() before you use it.
 *
 * The errno is a multi-digital positive integer number, it's different from
 * newlib's reentrant errno, don't get confused.
 *
 * The number blinks digital-by-digtal from left to right (up to 9 digits).
 * LED flashes quickly for 1~9 and slow for 0.
 * A longer delay time is necessary to help you sync to a new round.
 *
 * eg.
 * errno 0 (power up default):
 *     the LED is set ON for 1s and OFF for 1s (blinks slowly as 0.5Hz)
 *      ____________________                      ____________________
 * ____|                    |____________________|                    |_____
 *
 * errno 1
 *     the LED set ON for 0.2s and OFF for 1s
 *      ____                      ____                      ____            
 * ____|    |____________________|    |____________________|    |___________
 *
 * errno 3
 *     the LED set ON 3times and OFF for 1s
 *      ____      ____      ____                      ____      ____      __
 * ____|    |____|    |____|    |____________________|    |____|    |____|  
 * 
 * errno 21
 *     short delay is inserted between 2 and 1
 *      ____      ____            ____                      ____      ____  
 * ____|    |____|    |__________|    |____________________|    |____|    |_
 *
 * errno 102
 *      ____            ____________________            ____      ____      
 * ____|    |__________|                    |__________|    |____|    |_____
 *
 *
 * The task runs as an auxiliary service in a micro-kerenl RTOS environment,
 * it aims to help you debug your applications.
 * Especially when you're porting/debugging a new board, it may runs as the
 * first demo task.
 * Other tasks may call set(get)_errno when some events occur.
 * If a negative value is set, the task will stop blinking.
 * 
 * To use this service task, copy or symbolic link task_blink.c/h files to
 * your application and call task_blink_init() before you use it.
 *
 * An independent shell command 'error' (alias name 'e') can
 * modify the number in runtime, and stop the task from blinking.
 *
 * As this may be the only running task, it will periodically call 
 * hal_wdg_clear to feed the watchdog.
 * Task priority is set OS_PRIORITY_NORMAL (same as shell task), modify
 * it if * necessary.
 * 
 * The macro ERRNO_LED_NORMAL is set for errno zero, ERRNO_LED_ERROR for 
 * other non-zero numbers, normally they are both set to the same #0 led.
 * You can change them to different leds (use a RED led for warning/error
 * maybe a good ieda).
 *
 * MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "task_blink.h"

#define TICKS_NONZERO    OS_TICKS_MS(200)  /* on for 1~9 */
#define TICKS_SEPERATOR  OS_TICKS_MS(500)  /* off for 1~9 */
#define TICKS_ZERO       OS_TICKS_MS(1000)  /* on for 0 */
#define TICKS_IDLE       OS_TICKS_MS(1000)  /* delay cycle */


#if 1  //#ifndef NO_SHELL
int cmd_error( int argc, char *argv[] )
{
    const mcush_opt_spec opt_spec[] = {
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
        hal_led_clr( ERRNO_LED_ERROR );
        set_errno(-1);
        return 0;
    }

    if( new == -1 )
    {
        if( get_errno() < 0 )
            shell_write_line( shell_str_stop );
        else
            shell_printf( "%d\n", get_errno() );
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
#endif


static void blink_digit( int digit, int led, int wdg_enabled )
{
    int i;

    if( digit < 0 )
        return;

    if( digit )
    {
        for( i=digit; i; i-- )
        {
            hal_led_set(led);
            os_task_delay(TICKS_NONZERO);
            hal_led_clr(led);
            os_task_delay(TICKS_NONZERO);
            /* NOTE:
            if the blink task is the only running task, in release mode,
            cpu may reboot by the wdg (especially for long digits error 
            code), so clear the wdg on every blink */
            if( wdg_enabled )
                hal_wdg_clear();
        }
    }
    else
    {
        hal_led_set(led);
        os_task_delay(TICKS_ZERO);
        hal_led_clr(led);
        if( wdg_enabled )
            hal_wdg_clear();
    }
}


void task_blink_entry(void *arg)
{
    int i, digit, pos, skip, wdg_enabled;

    (void)arg;
    shell_add_cmd_table( cmd_tab_blink );

    while( 1 )
    {
        wdg_enabled = hal_wdg_is_enable();
        if( wdg_enabled )
            hal_wdg_clear();
        i = get_errno();
        if( i < 0 )
        {
            /* errno < 0, disable mode */
        }
        else
        {
            if( i == 0 )
            {
                /* no error */
                blink_digit( 0, ERRNO_LED_NORMAL, wdg_enabled );
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
                    blink_digit( digit, ERRNO_LED_ERROR, wdg_enabled );
                    skip = 0;
                    if( pos != 1 )
                        os_task_delay( TICKS_SEPERATOR );
                }
            }
        }
        os_task_delay( TICKS_IDLE );
    }
}


void task_blink_init(void)
{
    os_task_handle_t task;

#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( blink, TASK_BLINK_STACK_SIZE );
    task = os_task_create_static( TASK_BLINK_NAME, task_blink_entry, 0, TASK_BLINK_STACK_SIZE, TASK_BLINK_PRIORITY, &static_task_buffer_blink );
#else
    task = os_task_create( TASK_BLINK_NAME, task_blink_entry, 0, TASK_BLINK_STACK_SIZE, TASK_BLINK_PRIORITY );
#endif
    if( task == NULL )
        halt("create blink task");
}

