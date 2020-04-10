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

#if configSUPPORT_STATIC_ALLOCATION
StaticTask_t task_blink_buffer;
StackType_t task_blink_stack[TASK_BLINK_STACK_SIZE/sizeof(portSTACK_TYPE)];
#endif

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

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
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

const shell_cmd_t cmd_tab_blink[] = {
{   0, 'e', "error",  cmd_error,
    "error number",
    "e <num>"  },
{   CMD_END  } };


#define DELAY_A   200*configTICK_RATE_HZ/1000  /* on for 1~9 */
#define DELAY_B   500*configTICK_RATE_HZ/1000  /* off for 1~9 */
#define DELAY_C  1000*configTICK_RATE_HZ/1000  /* on for 0 */
#define DELAY_D  1000*configTICK_RATE_HZ/1000  /* delay cycle */

void blink_digit( int digit, int led )
{
    int i;

    if( digit < 0 )
        return;

    if( digit )
    {
        for( i=digit; i; i-- )
        {
            hal_led_set(led);
            vTaskDelay(DELAY_A);
            hal_led_clr(led);
            vTaskDelay(DELAY_A);
            /* NOTE:  if the blink task is the only running task, in release mode,
            cpu may reboot by the wdg (especially for long digits error code),
            so clear the wdg on every blink */
            hal_wdg_clear();
        }
    }
    else
    {
        hal_led_set(led);
        vTaskDelay(DELAY_C);
        hal_led_clr(led);
        hal_wdg_clear();
    }
}



void task_blink_entry(void *p)
{
    int i, digit, pos, skip;

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
                        vTaskDelay(DELAY_B);
                }
            }
        }
        vTaskDelay( DELAY_D );
    }
}


void task_blink_init(void)
{
    TaskHandle_t task_blink;

    shell_add_cmd_table( cmd_tab_blink );

#if configSUPPORT_STATIC_ALLOCATION
    task_blink = xTaskCreateStatic((TaskFunction_t)task_blink_entry, (const char *)"blinkT", 
                TASK_BLINK_STACK_SIZE / sizeof(portSTACK_TYPE),
                NULL, TASK_BLINK_PRIORITY, task_blink_stack, &task_blink_buffer);
#else
    (void)xTaskCreate(task_blink_entry, (const char *)"blinkT",
                      TASK_BLINK_STACK_SIZE / sizeof(portSTACK_TYPE),
                      NULL, TASK_BLINK_PRIORITY, &task_blink);
#endif
    if( task_blink == NULL )
        halt("create blink task");
}

