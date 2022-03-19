/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include "mcush.h"

#if DEBUG_SHELL
#define static
#endif

static shell_control_block_t scb;


int shell_read_char( char *c )
{
    int ret=-1;

    if( scb.script )
    {
        if( *scb.script )
        {
            *c = *scb.script++;
            ret = *c;
        }
        else
        {
            scb.script = 0;
            if( scb.script_free )
            {
                os_free( (void*)scb.script_free );
                scb.script_free = 0;
            }
        }
    }
    if( ret == -1 )
        ret = shell_driver_read_char( c );

    return ret;
}


int shell_read( char *buf, int len )
{
    int r=0;
    while( len )
    {
        if( shell_read_char( buf ) == -1 )
            break;
        buf++;
        r++; 
        len--;
    }
    return r;
}


int shell_read_feed( char *buf, int len )
{
    return shell_driver_read_feed( buf, len );
}


void shell_write_set_sniffer( int (*hook)( const char *buf, int len )/*, int block_mode*/ )
{
    scb.write_sniffer = hook;
    //scb.write_sniffer_block_mode = block_mode;
}


void shell_write_char( char c )
{
    if( scb.write_sniffer )
    {
        scb.write_sniffer( &c, 1 );
    }
    shell_driver_write_char( c );
}

 
void shell_write( const char *buf, int len )
{
    if( scb.write_sniffer )
    {
        scb.write_sniffer( buf, len );
    }
    shell_driver_write( (const char*)buf, len ); 
}


void shell_write_str( const char *str )
{
    if( str )
        shell_write( str, strlen(str) ); 
}


void shell_newline( void )
{
    shell_write( "\r\n", 2 );
}


void shell_write_line( const char *str )
{
    if( str )
    {
        shell_write( str, strlen(str) );
        shell_write( "\r\n", 2 );
    }
}


void shell_write_err( const char *str )
{
    if( str )
    {
        shell_write_str( str );
        shell_write_line( " error" );
    }
}

 
#if USE_SHELL_PRINTF
int shell_printf( char *fmt, ... )
{
    va_list ap;
    int n;
    char buf[SHELL_LINE_BUF_SIZE];

    va_start( ap, fmt );
    n = vsprintf( buf, fmt, ap );
    shell_write( buf, n );
    va_end( ap );
    return n;
}


void shell_write_int( int val )
{
    shell_printf( "%d", val );
}


void shell_write_int_line( int val )
{
    shell_write_int( val );
    shell_newline();
}


void shell_write_int01_line( int val )
{
    shell_write_line( val == 0 ? shell_str_0 : shell_str_1 );
}



void shell_write_float( float val )
{
    shell_printf( "%f", val );
}


void shell_write_float_line( float val )
{
    shell_write_float( val );
    shell_newline();
}


void shell_write_hex( int val )
{
    shell_printf( "0x%X", val );
}


void shell_write_hex_line( int val )
{
    shell_write_hex( val );
    shell_newline();
}


void shell_write_mem( const char *mem, int length )
{
    while( length-- )
    {
        shell_printf( "%02X", *mem++ );
    }
}
#endif


void shell_set_errnum( int errnum )
{
    scb.errnum = errnum;
}


int shell_get_errnum( void )
{
    return scb.errnum;
}


#ifndef NO_SHELL
int shell_add_cmd_table( const shell_cmd_t *cmd_table )
{
    int i;

    for( i=0; i<SHELL_CMD_TABLE_LEN; i++ )
    {
        if( ! scb.cmd_table[i] )
        {
            scb.cmd_table[i] = cmd_table;
            return 1;
        }
    }
#if SHELL_HALT_ON_ADD_CMD_TABLE_FAIL
    halt( "add cmd table" );
#endif
    return 0;
}
#endif


const char *shell_get_prompt( void )
{
    if( scb.prompt_hook )
        return (*scb.prompt_hook)();
    if( scb.errnum < 0 )
        return "?>";
    else if( scb.errnum > 0 )
        return "!>";
    else
        return "=>";
}


void shell_set_prompt_hook( const char *(*hook)(void) )
{
    scb.prompt_hook = hook;
}


char *shell_get_buf( void )
{
    return scb.cmdline;
}


#define IS_SPACE( c )            (((c)==' ')||((c)=='\t'))
#define IS_NOT_SPACE( c )        (((c)!=' ')&&((c)!='\t'))
#define IS_START_QUOTE( c )      (((c)=='\'')||((c)=='\"'))
#define IS_NOT_START_QUOTE( c )  (((c)!='\'')&&((c)!='\"'))
#define IS_END_QUOTE( c )        ((c)==quote)
#define IS_NOT_END_QUOTE( c )    ((c)!=quote)

static int shell_split_cmdline_into_argvs( char *cmd_line, uint8_t *argc, char *argv[] )
{
#if SHELL_QUOTE_PARSE_ENABLE
    char *p = cmd_line, *p2;
    char quote=0;

    *argc = 0;
    while( *p && IS_SPACE(*p) )
        p++;
    if( IS_START_QUOTE(*p) )
        quote = *p++;
    while( (*argc < SHELL_ARGV_LEN) && *p )
    {
        argv[*argc] = p;
        *argc += 1;
        if( quote )
        {
            while( *p && IS_NOT_END_QUOTE(*p) )
                p++;
            if( !*p )
                break;
            else
            {
                *p++ = 0; 
                quote = 0;
            }
        }
        else
        {
            while( *p && IS_NOT_SPACE(*p) && IS_NOT_START_QUOTE(*p) )
                p++;
            if( !*p )
                break;
            else if( IS_SPACE(*p) )
                *p++ = 0;
            else
            {
                quote = *p;
                p2 = p++;
                while( p2 > argv[*argc-1] )
                {
                    *p2 = *(p2-1);
                    p2--;
                }
                argv[*argc-1] += 1;
                while( *p && IS_NOT_END_QUOTE(*p) )
                    p++;
                if( !*p )
                    break;
                else
                {
                    *p++ = 0;
                    quote = 0;
                }
            }
        }
        while( *p && IS_SPACE(*p) )
            p++;
        if( !*p )
            break;
        if( IS_START_QUOTE(*p) )
            quote = *p++;
    }
    return *argc; 
#else
    char *p = cmd_line;

    *argc = 0;
    while( *p && IS_SPACE(*p) )
        p++;
    while( (*argc < SHELL_ARGV_LEN) && *p )
    {
        argv[*argc] = p;
        *argc += 1;
        while( *p && IS_NOT_SPACE(*p) )
            p++;
        if( !*p )
            break;
        *p++ = 0;
        while( *p && IS_SPACE(*p) )
            p++;
        if( !*p )
            break;
    }
    return *argc; 
#endif
}


static int shell_search_command( int cmdtab_index, char *cmd_name )
{
    const shell_cmd_t *ct = scb.cmd_table[cmdtab_index];
    int i, abbr;

    if( ct == 0 )
        return -1;
    if( (cmd_name == 0) || (*cmd_name == 0) )
        return -1;
    abbr = (strlen(cmd_name) == 1) ? 1 : 0;
    for( i=0; ct->name; i++, ct++ )
    {
        /* search sequence: abbreviated command first */
        if( abbr )
        {
            if( *cmd_name == ct->sname )
                return i;
        }
        else
        {
            if( strcmp( cmd_name, ct->name ) == 0 )
                return i;
        }
    }
    return -1;
}


static void shell_add_cmd_to_history( void )
{
    char *p1, *p2;

    if( scb.cmdline_len == 0 )
        return;
    if( scb.cmdline_len > SHELL_CMDLINE_HISTORY_LEN )
        return;
    p1 = &scb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN];
    p2 = p1 - scb.cmdline_len - 1;
    while( p1 >= scb.cmdline_history )
        *p1-- = *p2--;
    strcpy( scb.cmdline_history, scb.cmdline );
    p1 = &scb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN];
    while( *p1 )
        *p1-- = 0;
    scb.history_index = 0;
}


static char *shell_get_history( int index )
{
    char *p = scb.cmdline_history;

    while( index > 1 )
    {
        while(*p++);  /* move to next item */
        if( !*p || p >= &scb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN] )
            return 0;
        index--;
    }
    return *p ? p : 0;
}


static int shell_process_char( char c )
{
    int i;
    char *p;

    if( c == '\n' )
    {
        scb.errnum = 0;
        shell_write_char( '\n' );
        return 1;
    }
    else if( c == '\r' )  /* ignored */
    {
    }
#if SHELL_IGNORE_ESC_CHAR
    else if( c == 0x1B )  /* ESC ignored */
    {
    }
#endif
#if SHELL_IGNORE_LEADING_SPACE
    else if( (scb.cmdline_len == 0) && ((c == ' ') || (c == '\t')) )  /* ignore */
    {
    }
#endif
    else if( (c == '\b') || (c == 0x7F) )  /* backspace, DEL */
    {
        if( scb.cmdline_len && scb.cmdline_cursor )
        {
            if( scb.cmdline_len == scb.cmdline_cursor )
            {
                scb.cmdline[--scb.cmdline_len] = 0;
                shell_write_str( "\b \b" );
                scb.cmdline_cursor--;
            }
            else
            {
                for( i=scb.cmdline_cursor; i<scb.cmdline_len; i++ )
                    scb.cmdline[i-1] = scb.cmdline[i];
                scb.cmdline[--scb.cmdline_len] = 0;
                scb.cmdline_cursor--;
                shell_write_char( '\b' );
                shell_write_str( &scb.cmdline[scb.cmdline_cursor] );
                shell_write_char( ' ' );
                for( i=scb.cmdline_len+1; i>scb.cmdline_cursor; i-- )
                    shell_write_char( '\b' );
            }
        }
    }
    else if( c == 0x03 )  /* Ctrl-C, reset line */
    {
        shell_driver_reset();
        scb.cmdline_len = 0;
        scb.cmdline_cursor = 0;
        scb.errnum = 0;
        scb.history_index = 0;
        return -1;
    }
    else if( c == 0x1A )  /* Ctrl-Z, end of input */
    {
        scb.cmdline[scb.cmdline_cursor] = 0;
        return -2;
    }
    else if( (c == 0x10) || (c == 0x0E) )  /* Ctrl-P/N, previous/next history */
    {
        p = 0;
        if( c == 0x10 )  /* Ctrl-P */
        {
            p = shell_get_history( scb.history_index + 1 );
            if( p )
                scb.history_index += 1;
        }
        else if( c == 0x0E )  /* Ctrl-N */
        {
            if( scb.history_index > 1 )
            {
                p = shell_get_history( scb.history_index - 1 );
                if( p )
                    scb.history_index -= 1;
            }
        }
        if( p )
        {
            while( scb.cmdline_cursor < scb.cmdline_len )
            {
                shell_write_str( " " );
                scb.cmdline_cursor++;
            }
            while( scb.cmdline_len )
            {
                shell_write_str( "\b \b" );
                scb.cmdline_len--;
            }
            strcpy( scb.cmdline, p );
            scb.cmdline_cursor = scb.cmdline_len = (uint8_t)strlen(scb.cmdline);
            shell_write_str( scb.cmdline );
        }
    }
    else if( c == 0x01 )  /* Ctrl-A, begin */
    {
        while( scb.cmdline_cursor )
        {
            shell_write_char( '\b' );
            scb.cmdline_cursor--;
        }
    }
    else if( c == 0x05 )  /* Ctrl-E, end */
    {
        while( scb.cmdline_cursor < scb.cmdline_len )
            shell_write_char( scb.cmdline[scb.cmdline_cursor++] );
    }
    else if( c == 0x06 )  /* Ctrl-F, right */
    {
        if( scb.cmdline_cursor < scb.cmdline_len )
            shell_write_char( scb.cmdline[scb.cmdline_cursor++] );
    }
    else if( c == 0x02 )  /* Ctrl-B, left */
    {
        if( scb.cmdline_cursor )
        {
            shell_write_char( '\b' );
            scb.cmdline_cursor--;
        }
    }
    else if( c == 0x04 )  /* Ctrl-D, delete */
    {
        if( scb.cmdline_cursor < scb.cmdline_len )
        {
            for( i=scb.cmdline_cursor; i<scb.cmdline_len; i++ )
                scb.cmdline[i] = scb.cmdline[i+1];
            scb.cmdline[--scb.cmdline_len] = 0;
            shell_write_str( &scb.cmdline[scb.cmdline_cursor] );
            shell_write_char( ' ' );
            for( i=scb.cmdline_len+1; i>scb.cmdline_cursor; i-- )
                shell_write_char( '\b' );
        }
    }
    else if( c == 0x0B )  /* Ctrl-K, kill remaining */
    {
        if( scb.cmdline_cursor < scb.cmdline_len )
        {
            scb.cmdline[scb.cmdline_cursor] = 0;
            for( i=scb.cmdline_cursor; i<scb.cmdline_len; i++ )
                shell_write_char( ' ' );
            for( i=scb.cmdline_cursor; i<scb.cmdline_len; i++ )
                shell_write_char( '\b' );
            scb.cmdline_len = scb.cmdline_cursor;
        }
    }
    else if( c == 0x0C )  /* Ctrl-L, update, ignore */
    {
    }
    else if( c == 0 )  /* NULL, ignore */
    {
    }
    else if( scb.cmdline_len < SHELL_CMDLINE_LEN )  /* new char */
    {
        if( c == '\t')  /* convert TAB to space */
            c = ' ';
        if( scb.cmdline_cursor == scb.cmdline_len )  /* append */
        {
            scb.cmdline[scb.cmdline_len++] = c;
            scb.cmdline[scb.cmdline_len] = 0;
            scb.cmdline_cursor++;
            shell_write_char( c );
        }
        else  /* insert */
        {
            scb.cmdline[scb.cmdline_len+1] = 0;
            for( i=scb.cmdline_len; i>scb.cmdline_cursor; i-- )
                scb.cmdline[i] = scb.cmdline[i-1];
            scb.cmdline_len++;
            scb.cmdline[scb.cmdline_cursor++] = c;
            shell_write_str( &scb.cmdline[scb.cmdline_cursor-1] );
            for( i=scb.cmdline_len; i>scb.cmdline_cursor; i-- )
                shell_write_char( '\b' );
        }
    }
    return 0;
}            


static int shell_process_command( void )
{
    int (*cmd)(int argc, char *argv[]);
    int i=-1, j;

    if( shell_split_cmdline_into_argvs( scb.cmdline, &scb.argc, &scb.argv[0] ) )
    {
        for( j = 0; j < SHELL_CMD_TABLE_LEN; j++ )
        {
            i = shell_search_command( j, scb.argv[0] );
            if( i == -1 )
                continue;
            else
                break;
        }
        if( i == -1 )
        {
            shell_write_str( "Invalid command: " );
            shell_write_line( scb.argv[0] );
            scb.errnum = -1;
        }
        else
        {
            cmd = ((shell_cmd_t*)(scb.cmd_table[j] + i))->cmd;
            scb.errnum = (*cmd)( scb.argc, scb.argv );
        }
    }
    scb.cmdline[0] = 0;
    scb.cmdline_len = 0; 
    scb.cmdline_cursor = 0;
    return 1;
}

 
int (*shell_get_cmd_by_name( const char *name ))(int argc, char *argv[])
{
    int i, j;

    for( i = 0; i < SHELL_CMD_TABLE_LEN; i++ )
    {
        j = shell_search_command( i, (char*)name );
        if( j != -1 )
            return ((shell_cmd_t*)(scb.cmd_table[i] + j))->cmd;
    }
    return 0;
}


char shell_get_short_name( const char *name )
{
    int i, j;

    for( i = 0; i < SHELL_CMD_TABLE_LEN; i++ )
    {
        j = shell_search_command( i, (char*)name );
        if( j != -1 )
            return ((shell_cmd_t*)(scb.cmd_table[i] + j))->sname;
    }
    return 0;
}


int shell_print_help( const char *cmd, int show_hidden )
{
    int i, j;

    for( i = 0; (i < SHELL_CMD_TABLE_LEN) && scb.cmd_table[i]; i++ )
    {
        for( j=0; scb.cmd_table[i][j].name; j++ )
        {
            if( cmd && *cmd )
            {
                if( strlen(cmd) > 1 )
                {
                    if( strcmp(cmd, scb.cmd_table[i][j].name) != 0 )
                        continue;
                }
                else
                {
                    if( (strcmp(cmd, scb.cmd_table[i][j].name) != 0) || \
                        (*cmd != scb.cmd_table[i][j].sname) )
                        continue;
                }
            }
            if( !show_hidden && (scb.cmd_table[i][j].flag == CMD_HIDDEN) )
                continue;
            shell_write_str( scb.cmd_table[i][j].name );
            if( scb.cmd_table[i][j].sname )
            {
                shell_write_char( '/' );
                shell_write_char( scb.cmd_table[i][j].sname );
            }
            shell_write_str( "  " );
            shell_write_line( scb.cmd_table[i][j].description );
            shell_write_str( "  " );
            shell_write_line( scb.cmd_table[i][j].usage );
        }
    }
    return 0;
}


int shell_init( const shell_cmd_t *cmd_table, const char *init_script )
{
    memset( &scb, 0, sizeof(shell_control_block_t) );
    scb.cmd_table[0] = cmd_table;
    scb.script = init_script;
    return shell_driver_init();
}


int shell_set_script( const char *script, int need_free, int error_stop )
{
    scb.script = script;
    if( scb.script_free )
        os_free( (void*)scb.script_free );
    scb.script_free = need_free ? script : 0;
    scb.script_error_stop = error_stop;
    return 1;
}


int shell_is_script_mode( void )
{
    return scb.script ? 1 : 0;
}


int shell_read_line( char *buf, const char *prompt )
{
    char c;
    int r;

    scb.cmdline_len = 0;
    scb.cmdline_cursor = 0;
    if( prompt )
        shell_write_str( prompt );
    else
        shell_write_str( SHELL_INPUT_SUB_PROMPT ); 
    while( 1 )
    {
        if( shell_read_char(&c) == -1 )
            continue;
        r = shell_process_char(c);
        switch( r ) 
        {
        case 1:  /* end of line */
            scb.cmdline[scb.cmdline_len] = 0;
            if( scb.cmdline_len )
                shell_add_cmd_to_history();
            if( buf )
                strcpy( buf, scb.cmdline );
            return scb.cmdline_len;
        case -1:  /* Ctrl-C */
        case -2:  /* Ctrl-Z, end of input */
            shell_write_str("\r\n");
            if( buf )
                *buf = 0;
            return r;
        }
    }
}


char *shell_read_multi_lines( const char *prompt )
{
    char *buf1=0, *buf2=shell_get_buf(), *p;
    int len=0, alloc_size=SHELL_READ_LINES_ALLOC_SIZE_INC;

    buf1 = os_malloc(SHELL_READ_LINES_ALLOC_SIZE_INC);
    if( !buf1 )
        goto alloc_err;
    *buf1 = 0;
    while( 1 )
    {
        switch( shell_read_line(0, prompt) )
        {
        case 0:  /* empty line */
        case -2:  /* Ctrl-Z, end of input */
            if( len )
                return buf1;
            goto abort;
        case -1:  /* Ctrl-C, stop */
            goto abort;
        default:  /* normal line */
            len += strlen(buf2) + 1;
            if( (len+1) > alloc_size )
            {
                while( (len+1) > alloc_size )
                    alloc_size += SHELL_READ_LINES_ALLOC_SIZE_INC;
                p = os_realloc( buf1, alloc_size );
                if( !p )
                    goto alloc_err;
                buf1 = p; 
            }
            strcat( buf1, buf2 );
            strcat( buf1, "\n" );
            break;
        }
    }

abort:
    if( buf1 )
        os_free(buf1);
    return 0;
alloc_err:
    shell_write_line("malloc failed");
    if( buf1 )
        os_free(buf1);
    return 0;
}


int shell_make_16bits_data_buffer( void **pbuf, int *len )
{
    void *buf=0;
    int buf_len;
    uint16_t *p, r;
    char *p2, *p3;

    buf = shell_read_multi_lines(0);
    if( ! buf )
        return 0;
    buf_len = 0;
    p = (uint16_t*)buf;
    p2 = (char*)buf;
    while( *p2 )
    {
        r = (uint16_t)strtol( p2, &p3, 0 );
        if( !p3 || !*p3 )
            break;
        if( p2==p3 )
            goto fail;
        while( *p3 && ((*p3==' ')||(*p3==',')||(*p3=='\t')||(*p3=='\r')||(*p3=='\n')) )
            p3++;
        //if( *p3 && !(*p3>='0' && *p3<='9') )
        //    goto fail;
        p2 = p3;
        *p++ = r;
        buf_len += 1;
    }
    if( !buf_len )
        goto fail;
    p = (uint16_t*)os_realloc( buf, buf_len * 2 );
    if( p )
        buf = p;
    *pbuf = buf;
    *len = buf_len;
    return 1;
fail:
    os_free( buf );
    return 0;
}


int shell_make_float_data_buffer( void **pbuf, int *len )
{
    void *buf=0, *buf2=0;
    int buf_len;
    float *p, r;
    char *p2, *p3;

    buf = shell_read_multi_lines(0);
    if( ! buf )
        return 0;
    buf2 = os_malloc(SHELL_FLOAT_BUF_ALLOC_SIZE_INC);
    if( ! buf2 )
        goto alloc_err;
    buf_len = 0;
    p = (float*)buf2;
    p2 = (char*)buf;
    while( *p2 )
    {
        r = strtof( p2, &p3 );
        if( !p3 || !*p3 )
            break;
        if( p2==p3 )
            goto fail;
        while( *p3 && ((*p3==' ')||(*p3==',')||(*p3=='\t')||(*p3=='\r')||(*p3=='\n')) )
            p3++;
        //if( *p3 && !(*p3>='0' && *p3<='9') )
        //    goto fail;
        p2 = p3;
        *p++ = r;
        buf_len += 1;
    }
    if( !buf_len )
        goto fail;
    os_free(buf);
    p = (float*)os_realloc( buf2, buf_len * 4 );
    if( p )
        buf2 = (float*)p;
    *pbuf = buf2;
    *len = buf_len;
    return 1;
alloc_err:
    shell_write_line("malloc failed");
fail:
    if( buf )
        os_free(buf);
    if( buf2 )
        os_free(buf2);
    return 0;
}


/* load priority sequence:
 * 1. /c/init    (user configured)
 * 2. /r/init    (builtin romfs contents)
 */
static int load_init_script(void)
{
#if MCUSH_FCFS || MCUSH_ROMFS
    const char *addr;
    int size;
#if MCUSH_FCFS
    if( mcush_fcfs_get_raw_address( shell_str_init, &addr, &size ) )
        return shell_set_script( addr, 0, 0 );
    else
#endif
#if MCUSH_ROMFS
    if( mcush_romfs_get_raw_address( shell_str_init, &addr, &size ) )
        return shell_set_script( addr, 0, 0 );
#endif
#endif
    return 0;
}


void shell_run( void *p )
{
    (void)p;

#if ! SHELL_NO_PROMPT_AT_STARTUP
    shell_write_str("\r\n");
    shell_write_str( shell_get_prompt() );
#endif

    load_init_script();
    
    while( 1 )
    {
        switch( shell_read_line(0, "") )
        {
        case 0:  /* empty line */
            break;
        case -2:  /* Ctrl-Z, end of input */
        case -1:  /* Ctrl-C */
            break;
        default:  /* commands */
            shell_process_command();
        }
        shell_write_str( shell_get_prompt() );
        /* error stop in script mode */
        if( (scb.script_error_stop) && (scb.script != NULL)
            && (*scb.script != 0) && (scb.errnum != 0) )
        {
            shell_set_script( 0, 0, 0 );
        }
    }
}


/* call cmd_xxx with arguments
   NOTE: the last argument must be zero */
int shell_call( const char *cmd_name, ... )
{
    va_list ap;
    int ret=0;
    int (*cmd)(int argc, char *argv[]);
    char *argv[SHELL_ARGV_LEN], *s;
    int argc;

    va_start( ap, cmd_name );
    cmd = shell_get_cmd_by_name( cmd_name );
    if( cmd == 0 )
        ret = -1;
    else
    {
        argv[0] = (char*)cmd_name;
        argc = 1;
        while(1)
        {
            s = va_arg( ap, char* );
            if( s )
                argv[argc++] = s;
            else
                break;
        }
        ret = cmd( argc, argv );
    }
    va_end( ap );
    return ret;
}


/* call cmd_xxx with complete command line string,
   NOTE: cmd_line will be split into several sub-strings */
int shell_call_line( char *cmd_line )
{
    int ret=-1;
    int (*cmd)(int argc, char *argv[]);
    char *argv[SHELL_ARGV_LEN];   
    uint8_t argc;
    char cmdline[SHELL_CMDLINE_LEN+1];

    strcpy( cmdline, cmd_line );  /* input line maybe in FLASH area */

    if( shell_split_cmdline_into_argvs( cmdline, &argc, &argv[0] ) )
    {
        if( argc > 0 )
        {
            cmd = shell_get_cmd_by_name( argv[0] );
            if( cmd ) 
                ret = cmd( argc, argv );
        }
    }
    return ret;
}


