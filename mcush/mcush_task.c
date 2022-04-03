/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"

extern shell_cmd_t CMD_TAB[];

__signature const char mcush_signature[] = "<mcush>";

os_task_handle_t task_mcush;


__weak void _halt(void)
{
    int i, led_num=hal_led_get_num();

    os_disable_interrupts();
    for( i=0; i<led_num; i++ )
        hal_led_clr(i);
    while(1)
    {
        for( i=0; i<led_num; i++ )
            hal_led_toggle(i);
        hal_delay_ms( 500 );
    }
}

char *halt_message;
__weak void _halt_with_message(const char *message)
{
    halt_message = (char*)message;
    _halt();
}


static int _errno;

__weak int get_errno(void)
{
    return _errno;
}


__weak int set_errno(int num)
{
    int old = _errno;
    _errno = num;
    return old;
}


__weak void hal_pre_init(void)
{
}


void task_mcush_init(void)
{
#if OS_SUPPORT_STATIC_ALLOCATION
    DEFINE_STATIC_TASK_BUFFER( mcush, MCUSH_STACK_SIZE );
    task_mcush = os_task_create_static( "mcushT", shell_run, NULL,
                MCUSH_STACK_SIZE, MCUSH_PRIORITY, 
                &static_task_buffer_mcush );
#else
    task_mcush = os_task_create( "mcushT", shell_run, NULL,
                MCUSH_STACK_SIZE, MCUSH_PRIORITY );
#endif
    if( task_mcush == NULL )
        halt("mcush task create");
}


void mcush_init(void)
{
    if( task_mcush != NULL )
        return;
   
    os_init();
    hal_pre_init();
    if( !hal_init() )
        halt("hal init");

#ifndef NO_SHELL
    shell_init( &CMD_TAB[0], 0 );
    task_mcush_init();
#endif

#if MCUSH_ROMFS
    mcush_mount( "r", &mcush_romfs_driver );
#endif
#if MCUSH_FCFS
    mcush_mount( "c", &mcush_fcfs_driver );
#endif
#if MCUSH_SPIFFS
    mcush_mount( "s", &mcush_spiffs_driver );
#endif
#if MCUSH_FATFS
    mcush_mount( "f", &mcush_fatfs_driver );
#endif
}


void mcush_start(void)
{
    if( task_mcush == NULL )
        mcush_init();

    os_start();
    halt("stopped");
}
