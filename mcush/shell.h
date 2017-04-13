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

#define STOP_AT_INVALID_ARGUMENT    {  \
        shell_write_str( "invalid arg: " ); \
        shell_write_line( opt.value ); \
        mcush_opt_usage_fprint(stdout, argv[0], opt_spec ); \
        return -1; }


typedef struct _shell_cmd {
    const char *sname;
    const char *name;
    int (*cmd)(int argc, char *argv[]);
    const char *help;
    const char *usage;
} shell_cmd_t;


typedef struct _shell_control_block_t {
    int errnum;
    int argc;
    char *argv[SHELL_ARGV_LEN];
    char cmdline[SHELL_CMDLINE_LEN+1];
    char cmdline_history[SHELL_CMDLINE_HISTORY_LEN+1];
    int cmdline_len;
    int cmdline_cursor;
    int history_index;
    const char *(*prompt_hook)(void);
    const shell_cmd_t *cmd_table[SHELL_CMD_TABLE_LEN];
} shell_control_block_t;


/* APIs */
int  shell_init( const shell_cmd_t *cmd_table );
void shell_run( void );
void shell_set_prompt_hook( const char *(*hook)(void) );
int  shell_add_cmd_table( const shell_cmd_t *cmd_table );
int  shell_print_help( void );
void shell_set_errnum( int errnum );
int  shell_get_errnum( void );
int  shell_read_char( char *c );
int  shell_read_line( char *c );
void shell_write_char( char c );
void shell_write_str( const char *str );
void shell_write_line( const char *str );
void shell_write_int( int i );
void shell_write_float( float f );
void shell_write_hex( int x );
int  shell_printf( char *fmt, ... );


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


