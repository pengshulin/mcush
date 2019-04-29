/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_LOGGER_H_
#define _TASK_LOGGER_H_


/* file write option default state (power-up state),
   no matter it's disabled or not, real-time shell log monitoring
   is always avaiilable  */
#ifndef LOGGER_ENABLE
    #define LOGGER_ENABLE  1
#endif

/* default full log file pathname */
#ifndef LOGGER_FNAME
    #define LOGGER_FNAME  "/s/logger"
#endif

/* maximium log file size, if it's oversized, all log files will
   be rotationally renamed like 'logrotate' utility in linux */
#ifndef LOGGER_FSIZE_LIMIT
    #define LOGGER_FSIZE_LIMIT  20000u
#endif

/* the depth of history log files level,
   if set as N, there will be N+1 files: 
   (current)                                    (oldest)
   logger --> logger.1 --> logger.2 --> ... --> logger.N */
#ifndef LOGGER_ROTATE_LEVEL
    #define LOGGER_ROTATE_LEVEL  2
#endif

/* Don't set line buffer size too large, this will influence all tasks
   that call printf apis, their stack sizes must all be large enough.
   If you don't want to malloc large stacks, use sprintf to generate
   the buffer first and then call logger_str apis (note sprintf function
   also needs enough stack).
   Additionally, as 'log -t' command is executed in shell task,
   this also requires shell task stack size big enough. */
#ifndef LOGGER_LINE_BUF_SIZE
    #define LOGGER_LINE_BUF_SIZE  1024
#endif

/* use command 'log -t' to print the last N lines in log file
   note this only check the default log file, not the older history files */
#ifndef LOGGER_TAIL_NUM
    #define LOGGER_TAIL_NUM  10
#endif

/* default stack size */
#ifndef TASK_LOGGER_STACK_SIZE
    #define TASK_LOGGER_STACK_SIZE  (3*1024)
#endif

/* default priority is one level higher than the idle task, so if you found
   the logger queue is blocked, check if some other tasks refuse to give back
   the cpu */
#ifndef TASK_LOGGER_PRIORITY
    #define TASK_LOGGER_PRIORITY    (TASK_IDLE_PRIORITY+1)
#endif

/* default queue size */
#ifndef TASK_LOGGER_QUEUE_SIZE
    #define TASK_LOGGER_QUEUE_SIZE  (20)
#endif

/* default shell monitor queue size, events from logger queue will be forwarded 
   here, as shell writing is often very slow, this queue cache may be set larger,
   or some events will not be obseleted if the queue is full */
#ifndef TASK_LOGGER_MONITOR_QUEUE_SIZE
    #define TASK_LOGGER_MONITOR_QUEUE_SIZE  (20)
#endif

/* if any queue event is got and has been written to file, file will not be
   closed immediately, lazy close policy will reduce the open/close cycles */
#ifndef TASK_LOGGER_LAZY_CLOSE_MS
    #define TASK_LOGGER_LAZY_CLOSE_MS  (200)
#endif


#define LOG_DEBUG   0x01
#define LOG_INFO    0x02
#define LOG_WARN    0x04
#define LOG_ERROR   0x08

#define LOG_FLAG_CONST     0x01
#define LOG_FLAG_NO_WRITE  0x02

typedef struct _logger_event_t
{
    uint32_t time; 
    uint8_t type;
    uint8_t flag;
    char *str;
} logger_event_t;

void task_logger_init(void);

/* disabling logger module will only stop writing to logger files,
   shell monitor still works */
void logger_enable(void);
void logger_disable(void);
int logger_is_enabled(void);

/* these apis will malloc new buffer and copy from the original str,
   so they are safe to use, not mater whether the buffer is in stack or
   will be destroied later */
int logger_str( int type, const char *str );
int logger_debug( const char *str );
int logger_info( const char *str );
int logger_warn( const char *str );
int logger_error( const char *str );

/* these apis will directly use the original str (no malloc/free needed),
   very suitable for const string in flash memory.
   if you want to use buffer in stacks/heaps/sram, you must take care that 
   the string should be still alive before they are written to file and 
   forwarded to shell monitor and printed out */
int logger_const_str( int type, const char *str );
int logger_const_debug( const char *str );
int logger_const_info( const char *str );
int logger_const_warn( const char *str );
int logger_const_error( const char *str );

/* apis called from isr (only const str is available) */
int logger_const_str_isr( int type, const char *str );
int logger_const_debug_isr( const char *str );
int logger_const_info_isr( const char *str );
int logger_const_warn_isr( const char *str );
int logger_const_error_isr( const char *str );

/* printf apis,
   note that sprintf will be firstly called, the line buffer is located
   in current task stack, the stack should be large enough */
int logger_printf( int type, char *fmt, ... );
int logger_debug_printf( char *fmt, ... );
int logger_info_printf( char *fmt, ... );
int logger_warn_printf( char *fmt, ... );
int logger_error_printf( char *fmt, ... );

/* backup policy:
   copy /?/logger -> /?/logger.bak, /?/logger.N -> /?/logger.N.bak */
int backup_all_log_files( void );

/* remove /?/logger(.?) or /?/logger(.?).bak */
int delete_all_log_files( int delete_backup );

#endif

