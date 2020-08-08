/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_LOGGER_H_
#define _TASK_LOGGER_H_


/* file write option default state (power-up state),
   no matter it's disabled or not, real-time shell log monitoring
   is always available  */
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

/* logger_buffer api will dump memory in both hex/ascii format,
   this option defines the memory bytes in single line
   do not define it too small, that will involve too many lines
   and make the message queue overload */
#ifndef LOGGER_BUFFER_LINE_BYTES
    #define LOGGER_BUFFER_LINE_BYTES  32
#endif

/* use command 'log -t' to print the last N lines in log file
   note this only check the default log file, older history files
   are all ignored */
#ifndef LOGGER_TAIL_NUM
    #define LOGGER_TAIL_NUM  10
#endif

/* default stack size */
#ifndef TASK_LOGGER_STACK_SIZE
    #define TASK_LOGGER_STACK_SIZE  (3*1024)
#endif

/* default priority is one level higher than the idle task, so if you 
   found the logger queue blocked, check if some other tasks refuse to
   give back the cpu */
#ifndef TASK_LOGGER_PRIORITY
    #define TASK_LOGGER_PRIORITY    (TASK_IDLE_PRIORITY+1)
#endif

/* default queue size */
#ifndef TASK_LOGGER_QUEUE_SIZE
    #define TASK_LOGGER_QUEUE_SIZE  (20)
#endif

/* default shell monitor queue size, events from logger queue will be
   forwarded here, as shell writing is often very slow, this queue cache
   may be set larger, or some events will not be obseleted if the queue
   is full */
#ifndef TASK_LOGGER_MONITOR_QUEUE_SIZE
    #define TASK_LOGGER_MONITOR_QUEUE_SIZE  (20)
#endif

/* if any queue event is got and has been written to file, file will not
   be closed immediately, lazy close policy will reduce the open/close
   cycles */
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
    const char *module;
    void *mem;
} logger_event_t;

void task_logger_init(void);
void task_logger_entry(void *p);

/* disabling logger module will only stop writing to logger files,
   shell monitor still works */
void logger_enable(void);
void logger_disable(void);
int logger_is_enabled(void);
int logger_is_busy(void);

/* these apis will malloc new buffer and copy from the original str,
   so they are safe to use, not mater whether the buffer is in stack or
   will be destroied later, but not the most optimized */
int logger_module_str( int type, const char *module, const char *str );
int logger_module_debug( const char *module, const char *str );
int logger_module_info( const char *module, const char *str );
int logger_module_warn( const char *module, const char *str );
int logger_module_error( const char *module, const char *str );
/* apis without module name */
int logger_str2( int type, const char *str );
int logger_debug2( const char *str );
int logger_info2( const char *str );
int logger_warn2( const char *str );
int logger_error2( const char *str );
/* macro wrapper */
#define logger_str(type, str)   logger_module_str(type, logger_module_name, str)
#define logger_debug(str)       logger_module_debug(logger_module_name, str)
#define logger_info(str)        logger_module_info(logger_module_name, str)
#define logger_warn(str)        logger_module_warn(logger_module_name, str)
#define logger_error(str)       logger_module_error(logger_module_name, str)

/* these apis will directly use the original str (no malloc/free needed),
   very suitable for const string in flash memory.
   if you want to use buffer in stacks/heaps/sram, you must take care that 
   the string should be still alive before they are written to file and 
   forwarded to shell monitor and printed out */
int logger_module_const_str( int type, const char *module, const char *str );
int logger_module_const_debug( const char *module, const char *str );
int logger_module_const_info( const char *module, const char *str );
int logger_module_const_warn( const char *module, const char *str );
int logger_module_const_error( const char *module, const char *str );
/* apis without module name */
int logger_const_str2( int type, const char *str );
int logger_const_debug2( const char *str );
int logger_const_info2( const char *str );
int logger_const_warn2( const char *str );
int logger_const_error2( const char *str );
/* macro wrapper */
#define logger_const_str(type, str)   logger_module_const_str(type, logger_module_name, str)
#define logger_const_debug(str)       logger_module_const_debug(logger_module_name, str)
#define logger_const_info(str)        logger_module_const_info(logger_module_name, str)
#define logger_const_warn(str)        logger_module_const_warn(logger_module_name, str)
#define logger_const_error(str)       logger_module_const_error(logger_module_name, str)

/* apis called from isr (only const str is available) */
int logger_module_const_str_isr( int type, const char *module, const char *str );
int logger_module_const_debug_isr( const char *module, const char *str );
int logger_module_const_info_isr( const char *module, const char *str );
int logger_module_const_warn_isr( const char *module, const char *str );
int logger_module_const_error_isr( const char *module, const char *str );
/* apis without module name */
int logger_const_str_isr2( int type, const char *module, const char *str );
int logger_const_debug_isr2( const char *str );
int logger_const_info_isr2( const char *str );
int logger_const_warn_isr2( const char *str );
int logger_const_error_isr2( const char *str );
/* macro wrapper */
#define logger_const_str_isr(type, str)   logger_module_const_str_isr(type, logger_module_name, str)
#define logger_const_debug_isr(str)       logger_module_const_debug_isr(logger_module_name, str)
#define logger_const_info_isr(str)        logger_module_const_info_isr(logger_module_name, str)
#define logger_const_warn_isr(str)        logger_module_const_warn_isr(logger_module_name, str)
#define logger_const_error_isr(str)       logger_module_const_error_isr(logger_module_name, str)

/* printf apis,
   note that sprintf will be firstly called, the line buffer is located
   in current task stack, the stack should be large enough */
int logger_module_printf( int type, const char *module, char *fmt, ... );
int logger_module_printf_debug( const char *module, char *fmt, ... );
int logger_module_printf_info( const char *module, char *fmt, ... );
int logger_module_printf_warn( const char *module, char *fmt, ... );
int logger_module_printf_error( const char *module, char *fmt, ... );
/* apis without module name */
int logger_printf2( int type, char *fmt, ... );
int logger_printf_debug2( char *fmt, ... );
int logger_printf_info2( char *fmt, ... );
int logger_printf_warn2( char *fmt, ... );
int logger_printf_error2( char *fmt, ... );
/* macro wrapper */
#define logger_printf(type, fmt, ...)  logger_module_printf(type, logger_module_name, fmt, __VA_ARGS__)
#define logger_printf_debug(fmt, ...)  logger_module_printf_debug(logger_module_name, fmt, __VA_ARGS__)
#define logger_printf_info(fmt, ...)   logger_module_printf_info(logger_module_name, fmt, __VA_ARGS__)
#define logger_printf_warn(fmt, ...)   logger_module_printf_warn(logger_module_name, fmt, __VA_ARGS__)
#define logger_printf_error(fmt, ...)  logger_module_printf_error(logger_module_name, fmt, __VA_ARGS__)

/* memory blocks hexlify/ascii log */
void logger_module_buffer( const char *module, const char *buf, int len );
/* macro wrapper */
#define logger_buffer(buf, len)  logger_module_buffer( logger_module_name, buf, len )

/* module name define macro, declare this at the top of each module */
#define LOGGER_MODULE_NAME( name )  static const char logger_module_name[] = name

/* backup policy: rename /?/logger -> /?/logger.bak, /?/logger.N -> /?/logger.N.bak */
int backup_all_log_files( void );

/* remove /?/logger(.?) (and /?/logger(.?).bak) */
int delete_all_log_files( int delete_backup );

#endif

