/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _TASK_LOGGER_H_
#define _TASK_LOGGER_H_


#ifndef LOGGER_ENABLE
    #define LOGGER_ENABLE  1
#endif

#ifndef LOGGER_FNAME
    #define LOGGER_FNAME  "/s/logger"
#endif

#ifndef LOGGER_FSIZE_LIMIT
    #define LOGGER_FSIZE_LIMIT  20000u
#endif

#ifndef LOGGER_ROTATE_LEVEL
    #define LOGGER_ROTATE_LEVEL  2
#endif

#ifndef LOGGER_LINE_BUF_SIZE
    #define LOGGER_LINE_BUF_SIZE  1024
#endif

#ifndef LOGGER_TAIL_NUM
    #define LOGGER_TAIL_NUM  10
#endif

#ifndef TASK_LOGGER_STACK_SIZE
    #define TASK_LOGGER_STACK_SIZE  (3*1024)
#endif

#ifndef TASK_LOGGER_PRIORITY
    #define TASK_LOGGER_PRIORITY    (TASK_IDLE_PRIORITY+1)
#endif

#ifndef TASK_LOGGER_QUEUE_SIZE
    #define TASK_LOGGER_QUEUE_SIZE  (20)
#endif

#ifndef TASK_LOGGER_MONITOR_QUEUE_SIZE
    #define TASK_LOGGER_MONITOR_QUEUE_SIZE  (20)
#endif

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


/* these apis will malloc new buffer and copy from the original str */
int logger_str( int type, const char *str );
int logger_debug( const char *str );
int logger_info( const char *str );
int logger_warn( const char *str );
int logger_error( const char *str );

/* these apis will directly use the original str */
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

/* printf apis */
int logger_printf( int type, char *fmt, ... );

int logger_debug_printf( char *fmt, ... );
int logger_info_printf( char *fmt, ... );
int logger_warn_printf( char *fmt, ... );
int logger_error_printf( char *fmt, ... );


/* copy /?/logger -> /?/logger.bak, /?/logger.N -> /?/logger.N.bak */
int backup_all_log_files( void );


#endif

