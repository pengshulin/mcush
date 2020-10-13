/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


#if USE_CMD_UPTIME
int cmd_uptime( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char buf[16];

    mcush_opt_parser_init(&parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( !opt.spec )
            STOP_AT_INVALID_ARGUMENT 
    }

    shell_write_line( get_uptime_str(buf, 1) );
    return 0;
}
#endif


#if USE_CMD_SYSTEM
void task_idle_counter_entry(void *p)
{
    volatile uint32_t *cnt = (uint32_t*)p;
        
    while(1)
        (*cnt)++;
}


extern void os_task_info_print(void);
extern void os_queue_info_print(void);
extern void os_kernel_info_print(void);


int cmd_system( int argc, char *argv[] )
{
    static const mcush_opt_spec opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_type, 0, "(t)ask"
#if USE_CMD_SYSTEM_QUEUE
            "|(q)ueue"
#endif
#if USE_CMD_SYSTEM_KERNEL
            "|(k)ern"
#endif
#if USE_CMD_SYSTEM_HEAP
            "|heap"
#endif
#if USE_CMD_SYSTEM_STACK
            "|stack"
#endif
#if USE_CMD_SYSTEM_IDLE
            "|(i)dle"
#endif
#if MCUSH_VFS_STATISTICS
            "|v(f)s"
#endif
             },
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    const char *type=0;
#if USE_CMD_SYSTEM_IDLE
    uint32_t idle_counter, idle_counter_last, idle_counter_max;
    os_task_handle_t task_idle_counter;
#endif
#if USE_CMD_SYSTEM_STACK
    char *p;
#endif
    
    mcush_opt_parser_init( &parser, opt_spec, argv+1, argc-1 );
    while( mcush_opt_parser_next( &opt, &parser ) )
    {
        if( opt.spec )
        {
            if( STRCMP( opt.spec->name, shell_str_type ) == 0 )
                type = opt.value;
        }
        else
            STOP_AT_INVALID_ARGUMENT 
    }

    if( !type )
    {
        STOP_AT_INVALID_ARGUMENT 
    }
    else if( (strcmp( type, "t" ) == 0) || (strcmp( type, "task" ) == 0) )
    {
        os_task_info_print();
    }
#if USE_CMD_SYSTEM_QUEUE
    else if( (strcmp( type, "q" ) == 0 ) || (strcmp( type, "queue" ) == 0 ) )
    {
        os_queue_info_print();
    }
#endif
#if USE_CMD_SYSTEM_KERNEL
    else if( (strcmp( type, "k" ) == 0 ) || (strcmp( type, "kern" ) == 0 ) )
    {
        os_kernel_info_print();
    }
#endif
#if USE_CMD_SYSTEM_HEAP
    else if( strcmp( type, "heap" ) == 0 )
    {
        extern char *heap_end, _sheap, _eheap;
        shell_printf( "%s: 0x%08X\n%s: 0x%08X\n%s: 0x%08X\n", 
                        shell_str_start,  &_sheap, 
                        shell_str_current, heap_end,
                        shell_str_end, &_eheap ); 
    }
#endif
#if USE_CMD_SYSTEM_STACK
    else if( strcmp( type, "stack" ) == 0 )
    {
        extern char _sstack, _estack;
        for( p=&_sstack, i=0; p<&_estack && (*p == 0xA5); p++, i++ );
        shell_printf( "%s: 0x%08X\n%s: 0x%08X\n%s: %d\n", shell_str_start, &_sstack, shell_str_end, &_estack, shell_str_free, i );
    }
#endif
#if USE_CMD_SYSTEM_IDLE
    else if( (strcmp( type, "i" ) == 0 ) || (strcmp( type, "idle" ) == 0) )
    {
        char c;
        int i;
        /* create counter task to check the maximum count value available */
        /* NOTE: the task runs at top priority and may involve side-effects:
           some tasks that are are responsable for clearing the watchdog
           are also stopped, so shell task takes the responsibility before 
           they come back                                                 */
        idle_counter = 0;
        os_task_priority_set( NULL, OS_PRIORITY_HIGHEST );
        hal_wdg_clear();
        task_idle_counter = os_task_create( "idleCntT",
                task_idle_counter_entry, &idle_counter,
                OS_STACK_SIZE_MIN, OS_PRIORITY_HIGHEST );
        if( task_idle_counter == NULL )
        {
            os_task_priority_set( NULL, MCUSH_PRIORITY );
            return 1; 
        }
        /* check for continuous 0.5 second */
        idle_counter_last = idle_counter;
        hal_wdg_clear();
        while( shell_driver_read_char_blocked(&c, OS_TICKS_MS(500)) != -1 )
        {
            hal_wdg_clear();
            if( c == 0x03 ) /* Ctrl-C for stop */
            {
                os_task_delete( task_idle_counter );
                os_task_priority_set( NULL, MCUSH_PRIORITY );
                return 0; 
            }
            else
                idle_counter_last = idle_counter;  /* interrupted, reset */
        }
        hal_wdg_clear();
        idle_counter_max = idle_counter - idle_counter_last;
        idle_counter_max *= 2;
        os_task_priority_set( task_idle_counter, TASK_IDLE_PRIORITY );
        os_task_priority_set( NULL, MCUSH_PRIORITY );

        /* loop check cpu idle rate at low priority */
        idle_counter_last = idle_counter;
        while( 1 )
        {
            while( shell_driver_read_char_blocked(&c, OS_TICK_RATE) != -1 )
            {
                if( c == 0x03 ) /* Ctrl-C for stop */
                {
                    os_task_delete( task_idle_counter );
                    return 0; 
                }
            }
            i = idle_counter - idle_counter_last;
            idle_counter_last = idle_counter;
            shell_printf( "%d %%\n", i * 100 / idle_counter_max );
        }
    }
#endif
#if MCUSH_VFS_STATISTICS
    else if( (strcmp( type, "f" ) == 0 ) || (strcmp( type, "vfs" ) == 0) )
    {
        extern mcush_vfs_statistics_t vfs_stat;
        mcush_vfs_statistics_t stat;  /* take snapshot */
        memcpy((void*)&stat, (const void*)&vfs_stat, sizeof(mcush_vfs_statistics_t)); 
        shell_printf( "mount: %u\n", stat.count_mount );
        shell_printf( "umount: %u\n", stat.count_umount );
        shell_printf( "open:  %u / %u\n", stat.count_open, stat.count_open_err );
        shell_printf( "close: %u / %u\n", stat.count_close, stat.count_close_err );
        shell_printf( "read:  %u / %u\n", stat.count_read, stat.count_read_err );
        shell_printf( "write: %u / %u\n", stat.count_write, stat.count_write_err );
        shell_printf( "flush: %u / %u\n", stat.count_flush, stat.count_flush_err );
    }
#endif
    else
        STOP_AT_INVALID_ARGUMENT 
    return 0;
}
#endif


