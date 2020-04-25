/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"


#if USE_CMD_UPTIME
int cmd_uptime( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_NONE } };
    mcush_opt_parser parser;
    mcush_opt opt;
    char buf[16];

    mcush_opt_parser_init(&parser, opt_spec, (const char **)(argv+1), argc-1 );
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


#if USE_CMD_SYSTEM_KERNEL
static void _print_kernel_item_name(const char *name)
{
    int len=strlen(name);

    shell_write_str(name);
    shell_write_char(':');
    while( len++ < 21 )
        shell_write_char(' ');
}
#endif


int cmd_system( int argc, char *argv[] )
{
    static const mcush_opt_spec const opt_spec[] = {
        { MCUSH_OPT_ARG, MCUSH_OPT_USAGE_REQUIRED, 
          0, shell_str_type, 0, "(t)ask|(q)ueue"
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
    QueueHandle_t xQueue;
    mcush_queue_info_t qinfo;
    mcush_kern_info_t kinfo;
    const char *name=0;
    int i, j;
    const char *type=0;
    char c;
    TaskStatus_t *task_status_array;
#if USE_CMD_SYSTEM_IDLE
    uint32_t idle_counter, idle_counter_last, idle_counter_max;
    TaskHandle_t task_idle_counter;
#if configSUPPORT_STATIC_ALLOCATION
    StaticTask_t task_idle_counter_data;
    StackType_t task_idle_counter_buffer[configMINIMAL_STACK_SIZE];
#endif
#endif
#if USE_CMD_SYSTEM_STACK
    char *p;
#endif
    char buf[1024];
    
    mcush_opt_parser_init( &parser, opt_spec, (const char **)(argv+1), argc-1 );
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
        mcushGetKernInfo(&kinfo);
        task_status_array = (TaskStatus_t *)buf;
        i = uxTaskGetSystemState( task_status_array, kinfo.uxCurrentNumberOfTasks, NULL );
        for( j=0; j<i; j++ )
        {
            /* status */
            switch( task_status_array[j].eCurrentState )
            {
            case eRunning: c = 'X'; break;
            case eReady: c = 'R'; break;
            case eBlocked: c = 'B'; break;
            case eSuspended: c = 'S'; break;
            case eDeleted: c = 'D'; break;
            default: c = '?'; break;
            } 
            
            shell_printf( "%2d %8s %c 0x%08X %d/%d 0x%08X 0x%08X (free %d)\n",
                        task_status_array[j].xTaskNumber,
                        task_status_array[j].pcTaskName, c, 
                        task_status_array[j].xHandle,
                        task_status_array[j].uxCurrentPriority, task_status_array[j].uxBasePriority, 
                        task_status_array[j].pxStackBase,
                        (uint32_t*)mcushGetTaskStackTop( task_status_array[j].xHandle ),
                        task_status_array[j].usStackHighWaterMark * sizeof(portSTACK_TYPE) );
        } 
    }
    else if( (strcmp( type, "q" ) == 0 ) || (strcmp( type, "queue" ) == 0 ) )
    {
        for( i=0; i<configQUEUE_REGISTRY_SIZE; i++ )
        {
            if( mcushGetQueueRegistered( i, (void**)&xQueue, &name ) )
            {
                if( mcushGetQueueInfo( xQueue, &qinfo ) )
                {
                    shell_printf( "%8s 0x%08X  %5d %4d %4d  0x%08X - 0x%08X (0x%08X)\n", name, (int)xQueue, 
                        qinfo.uxLength, qinfo.uxItemSize, qinfo.uxMessagesWaiting, 
                        (int)qinfo.pcHead, (int)qinfo.pcTail, ((int)qinfo.pcTail-(int)qinfo.pcHead) );
                }
            }
        }
    }
#if USE_CMD_SYSTEM_KERNEL
    else if( (strcmp( type, "k" ) == 0 ) || (strcmp( type, "kern" ) == 0 ) )
    {
        _print_kernel_item_name( "SystemCoreClock" );
        shell_printf( "%d\n", SystemCoreClock );
        _print_kernel_item_name( "TickRate" );
        shell_printf( "%d\n", configTICK_RATE_HZ );
        mcushGetKernInfo(&kinfo);
        _print_kernel_item_name( "CurrentNumberOfTasks" );
        shell_printf( "%d\n", kinfo.uxCurrentNumberOfTasks );
        _print_kernel_item_name( "TopReadyPriority" );
        shell_printf( "%d\n", kinfo.uxTopReadyPriority );
        _print_kernel_item_name( "PendedTicks" );
        shell_printf( "%d\n", kinfo.uxPendedTicks );
        _print_kernel_item_name( "NumOfOverflows" );
        shell_printf( "%d\n", kinfo.uxNumOfOverflows );
        _print_kernel_item_name( "CurrentTCB" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxCurrentTCB, mcushGetTaskNameFromTCB(kinfo.pxCurrentTCB) );
        for( i=configMAX_PRIORITIES-1; i>=0; i-- )
        {
            shell_printf( "ReadyTaskLists[%d]:    0x%08X %s\n", i, (uint32_t)kinfo.pxReadyTaskLists[i], mcushGetTaskNamesFromTaskList(kinfo.pxReadyTaskLists[i], buf) );
        }
        _print_kernel_item_name( "DelayedTaskList1" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList1, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList1, buf) );
        _print_kernel_item_name( "DelayedTaskList2" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList2, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList2, buf) );
        _print_kernel_item_name( "DelayedTaskList" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxDelayedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxDelayedTaskList, buf) );
        _print_kernel_item_name( "OverflowDelayedTList" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxOverflowDelayedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxOverflowDelayedTaskList, buf) );
        _print_kernel_item_name( "PendingReadyList" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxPendingReadyList, mcushGetTaskNamesFromTaskList(kinfo.pxPendingReadyList, buf) );
#if ( INCLUDE_vTaskSuspend == 1 )
        _print_kernel_item_name( "SuspendedTaskList" );
        shell_printf( "0x%08X %s\n", (uint32_t)kinfo.pxSuspendedTaskList, mcushGetTaskNamesFromTaskList(kinfo.pxSuspendedTaskList, buf) );
#endif
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
        /* create counter task to check the maximum count value available */
        /* NOTE: the task runs at top priority and may involve side-effects */
        idle_counter = 0;
        vTaskPrioritySet( NULL, configMAX_PRIORITIES-1 );
#if configSUPPORT_STATIC_ALLOCATION
        task_idle_counter = xTaskCreateStatic((TaskFunction_t)task_idle_counter_entry,
                (const char *)"idleCntT", configMINIMAL_STACK_SIZE,
                &idle_counter, configMAX_PRIORITIES-1,
                task_idle_counter_buffer, &task_idle_counter_data);
#else
        xTaskCreate((TaskFunction_t)task_idle_counter_entry, (const char *)"idleCntT", 
                configMINIMAL_STACK_SIZE,
                &idle_counter, configMAX_PRIORITIES-1, &task_idle_counter);
#endif
        if( task_idle_counter == NULL )
        {
            vTaskPrioritySet( NULL, MCUSH_PRIORITY );
            return 1; 
        }
        /* check for continuous 0.5 second */
        idle_counter_last = idle_counter;
        hal_wdg_clear();
        while( shell_driver_read_char_blocked(&c, configTICK_RATE_HZ / 2) != -1 )
        {
            hal_wdg_clear();
            if( c == 0x03 ) /* Ctrl-C for stop */
            {
                vTaskDelete( task_idle_counter );
                vTaskPrioritySet( NULL, MCUSH_PRIORITY );
                return 0; 
            }
            else
                idle_counter_last = idle_counter;  /* interrupted, reset */
        }
        hal_wdg_clear();
        idle_counter_max = idle_counter - idle_counter_last;
        idle_counter_max *= 2;
        vTaskPrioritySet( task_idle_counter, TASK_IDLE_PRIORITY );
        vTaskPrioritySet( NULL, MCUSH_PRIORITY );

        /* loop check cpu idle rate at low priority */
        idle_counter_last = idle_counter;
        while( 1 )
        {
            while( shell_driver_read_char_blocked(&c, configTICK_RATE_HZ) != -1 )
            {
                if( c == 0x03 ) /* Ctrl-C for stop */
                {
                    vTaskDelete( task_idle_counter );
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


