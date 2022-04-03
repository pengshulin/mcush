/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHELL_DEFAULT_BAUDRATE
    #define SHELL_DEFAULT_BAUDRATE  9600
#endif
#ifndef SHELL_CMDLINE_LEN
    #define SHELL_CMDLINE_LEN    128
#endif
#ifndef SHELL_CMDLINE_HISTORY_LEN
    #define SHELL_CMDLINE_HISTORY_LEN    SHELL_CMDLINE_LEN
#endif
#ifndef SHELL_ARGV_LEN
    #define SHELL_ARGV_LEN       20
#endif
#ifndef SHELL_CMD_TABLE_LEN
    #define SHELL_CMD_TABLE_LEN  4
#endif
#ifndef SHELL_READ_LINES_ALLOC_SIZE_INC
    #define SHELL_READ_LINES_ALLOC_SIZE_INC  512
#endif
#ifndef SHELL_FLOAT_BUF_ALLOC_SIZE_INC
    #define SHELL_FLOAT_BUF_ALLOC_SIZE_INC  512
#endif
#ifndef SHELL_INPUT_SUB_PROMPT
    #define SHELL_INPUT_SUB_PROMPT  ">"
#endif
#ifndef SHELL_IGNORE_ESC_CHAR
    #define SHELL_IGNORE_ESC_CHAR  1 
#endif
#ifndef SHELL_IGNORE_LEADING_SPACE
    #define SHELL_IGNORE_LEADING_SPACE  0
#endif
#ifndef USE_SHELL_PRINTF
    #define USE_SHELL_PRINTF  1
#endif
#ifndef USE_SHELL_PRINTF2
    #define USE_SHELL_PRINTF2  1
#endif
#ifndef USE_SHELL_EVAL
    #define USE_SHELL_EVAL  1
#endif
#ifndef SHELL_QUOTE_PARSE_ENABLE
    #define SHELL_QUOTE_PARSE_ENABLE  1
#endif
#ifndef SHELL_LINE_BUF_SIZE 
    #define SHELL_LINE_BUF_SIZE  256
#endif
#ifndef SHELL_HALT_ON_ADD_CMD_TABLE_FAIL
    #define SHELL_HALT_ON_ADD_CMD_TABLE_FAIL  1
#endif
#ifndef SHELL_NO_PROMPT_AT_STARTUP
    #define SHELL_NO_PROMPT_AT_STARTUP  0
#endif

#define STOP_AT_INVALID_ARGUMENT   \
        return mcush_opt_check_invalid_argument(argv[0], &opt, opt_spec);

#define LOOP_CHECK \
        if( loop ) \
        { \
            if( loop_delay < 1000/OS_TICK_RATE )  \
                loop_delay = 1000/OS_TICK_RATE;  \
            loop_tick = os_tick(); \
            while( os_tick() < loop_tick + OS_TICKS_MS(loop_delay) ) \
            { \
                if( shell_driver_read_char_blocked(&c, OS_TICKS_MS(10)) != -1 ) \
                { \
                    if( c == 0x03 ) /* Ctrl-C for stop */ \
                    { \
                        if( shell_is_script_mode() ) \
                            shell_set_script( NULL, 0, 0 );  /* abort current script */ \
                        return 0; \
                    } \
                } \
            } \
            goto loop_start; \
        }


enum {
    CMD_NORMAL=0,
    CMD_HIDDEN,
    CMD_END,
};

typedef struct _shell_cmd {
    uint8_t flag;
    char sname;
    const char *name;
    int (*cmd)(int argc, char *argv[]);
    const char *description;
    const char *usage;
} shell_cmd_t;


typedef struct _shell_control_block_t {
    int errnum;
    char *argv[SHELL_ARGV_LEN];
    uint8_t argc;
    uint8_t cmdline_len;
    uint8_t cmdline_cursor;
    uint8_t history_index;
    char cmdline[SHELL_CMDLINE_LEN+1];
    char cmdline_history[SHELL_CMDLINE_HISTORY_LEN+1];
    const char *(*prompt_hook)(void);
    //const shell_cmd_t *cmd_table[SHELL_CMD_TABLE_LEN];
    const char *script;
    const char *script_free;
    uint8_t script_error_stop;
    int (*write_sniffer)( const char *buf, int len );
    //uint8_t write_sniffer_block_mode;
} shell_control_block_t;


/* APIs */
int  shell_init( const shell_cmd_t *cmd_table, const char *init_script );
void shell_run( void *p );
void shell_set_prompt_hook( const char *(*hook)(void) );
//#if NO_SHELL
//#define shell_add_cmd_table( cmd_table) 
//#else
int  shell_add_cmd_table( const shell_cmd_t *cmd_table );
//#endif
int  shell_print_help( const char *cmd, int show_hidden );
int  shell_set_script( const char *script, int need_free, int error_stop );
int  shell_is_script_mode( void );
void shell_set_errnum( int errnum );
int  shell_get_errnum( void );
char *shell_get_buf( void );
int  shell_read_char( char *c );
int  shell_read( char *buf, int len );
int  shell_read_line( char *c, const char *prompt );
char *shell_read_multi_lines( const char *prompt );
int  shell_read_feed( char *buf, int len );
void shell_write_char( char c );
void shell_write( const char *buf, int len );
void shell_write_str( const char *str );
void shell_write_line( const char *str );
void shell_write_err( const char *str );
void shell_write_set_sniffer( int (*hook)( const char *buf, int len ) );
void shell_newline( void );
void shell_write_int( int i );
void shell_write_int_line( int val );
void shell_write_int01_line( int val );
void shell_write_float( float val );
void shell_write_float_line( float val );
void shell_write_hex( int val );
void shell_write_hex_line( int val );
void shell_write_mem( const char *mem, int length );
int  shell_printf( char *fmt, ... );
const char *shell_get_prompt( void );
int  shell_make_16bits_data_buffer( void **pbuf, int *len );
int  shell_make_float_data_buffer( void **pbuf, int *len );
int  (*shell_get_cmd_by_name( const char *name ))(int argc, char *argv[]);
char shell_get_short_name( const char *name );
int  shell_call( const char *cmd_name, ... );
int  shell_call_line( char *cmd_line );

/* driver APIs needed */
extern int  shell_driver_init( void );
extern void shell_driver_reset( void );
extern int  shell_driver_read( char *buffer, int len );
extern int  shell_driver_read_char( char *c );
extern int  shell_driver_read_char_blocked( char *c, int block_ticks );
extern int  shell_driver_read_is_empty( void );
extern int  shell_driver_read_feed( char *buffer, int len );
extern int  shell_driver_write( const char *buffer, int len );
extern void shell_driver_write_char( char c );
extern void shell_driver_write_flush( void );


#ifdef __cplusplus
}
#endif

#endif

