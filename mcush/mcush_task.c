/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

extern shell_cmd_t CMD_TAB[];
extern char _isdata;

__attribute__((section(".mcush_signature")))
__attribute__((used))
static const char mcush_signature[] = "<mcush>";

os_task_handle_t task_mcush;


__attribute__((weak)) void hal_pre_init(void)
{
}


void mcush_init(void)
{
    if( task_mcush != NULL )
        return;
    
    os_init();

    hal_pre_init();
    if( !hal_init() )
        halt("hal init");

    if( !shell_init( &CMD_TAB[0], _isdata ? &_isdata : 0 ) )
        halt("shell init");


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


void mcush_start(void)
{
    if( task_mcush == NULL )
        mcush_init();

    os_start();
    halt("stopped");
}
