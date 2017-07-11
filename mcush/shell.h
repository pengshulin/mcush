/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef _SHELL_H_
#define _SHELL_H_

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
#ifndef SHELL_INPUT_SUB_PROMPT
    #define SHELL_INPUT_SUB_PROMPT  ">"
#endif
#ifndef SHELL_IGNORE_LEADING_SPACE
    #define SHELL_IGNORE_LEADING_SPACE  1
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
#ifndef USE_SHELL_EVAL_SSCANF
    #define USE_SHELL_EVAL_SSCANF  0
#endif
#ifndef SHELL_QUOTE_PARSE_ENABLE
    #define SHELL_QUOTE_PARSE_ENABLE   1
#endif

#define STOP_AT_INVALID_ARGUMENT    {  \
    if( opt.value && strcmp(opt.value, "--help")==0 ) { \
        mcush_opt_usage_print( argv[0], opt_spec ); \
        return 0;  }  \
    else  {  \
        shell_write_str( "invalid arg: " ); \
        shell_write_line( opt.value ); \
        mcush_opt_usage_print( argv[0], opt_spec ); \
        return -1; }  } 




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
    const shell_cmd_t *cmd_table[SHELL_CMD_TABLE_LEN];
    const char *script;
} shell_control_block_t;


/* APIs */
int  shell_init( const shell_cmd_t *cmd_table, const char *init_script );
void shell_run( void );
void shell_set_prompt_hook( const char *(*hook)(void) );
int  shell_add_cmd_table( const shell_cmd_t *cmd_table );
int  shell_print_help( const char *cmd, int show_hidden );
void shell_set_errnum( int errnum );
int  shell_get_errnum( void );
char *shell_get_buf( void );
int  shell_read_char( char *c );
int  shell_read_line( char *c, const char *prompt );
char *shell_read_multi_lines( const char *prompt );
void shell_write_char( char c );
void shell_write( const char *buf, int len );
void shell_write_str( const char *str );
void shell_write_line( const char *str );
void shell_write_int( int i );
void shell_write_float( float f );
void shell_write_hex( int x );
int  shell_printf( char *fmt, ... );
int  shell_eval_int( const char *str, int *i );
int  shell_eval_float( const char *str, float *f );
const char *shell_get_prompt( void );
#if defined(MCUSH_NON_OS)
void shell_proc_event_char(void);
#endif
int shell_make_16bits_data_buffer( void **pbuf, int *len );

/* driver APIs needed */
extern int  shell_driver_init( void );
extern void shell_driver_reset( void );
extern int  shell_driver_read( char *buffer, int len );
extern int  shell_driver_read_char( char *c );
extern int  shell_driver_read_char_blocked( char *c, int block_time );
extern int  shell_driver_read_is_empty( void );
extern int  shell_driver_write( const char *buffer, int len );
extern void shell_driver_write_char( char c );


#endif


