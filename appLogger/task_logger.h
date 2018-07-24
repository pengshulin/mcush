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

#define TASK_LOGGER_STACK_SIZE  (2048 / sizeof(portSTACK_TYPE))
#define TASK_LOGGER_PRIORITY    (tskIDLE_PRIORITY+1)
#define TASK_LOGGER_QUEUE_SIZE  (20)
#define TASK_LOGGER_MONITOR_QUEUE_SIZE  (20)

#define LOG_DEBUG   0x01
#define LOG_INFO    0x02
#define LOG_WARN    0x04
#define LOG_ERROR   0x08



typedef struct _logger_event_t
{
    uint32_t time; 
    uint8_t type;
    char *str;
} logger_event_t;

void task_logger_init(void);

int logger_str( int type, const char *str );
int logger_debug( const char *str );
int logger_info( const char *str );
int logger_warn( const char *str );
int logger_error( const char *str );

int logger_str_isr( int type, const char *str );
int logger_debug_isr( const char *str );
int logger_info_isr( const char *str );
int logger_warn_isr( const char *str );
int logger_error_isr( const char *str );

int logger_printf( int type, char *fmt, ... );

#endif

