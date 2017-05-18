/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "shell.h"


static shell_control_block_t cb;


int shell_read_char( char *c )
{
    return shell_driver_read_char( c );
}


void shell_write_char( char c )
{
    shell_driver_write_char( c );
}

 
void shell_write_str( const char *str )
{
    if( str )
        shell_driver_write( (const char*)str, strlen(str) ); 
}


void shell_write_int( int i )
{
    char buf[32];
    sprintf( buf, "%d", i );
    shell_write_str( (const char*)buf ); 
}


void shell_write_float( float f )
{
    char buf[32];
    sprintf( buf, "%f", f );
    shell_write_str( (const char*)buf ); 
}


void shell_write_hex( int x )
{
    char buf[32];
    sprintf( buf, "0x%08x", x );
    shell_write_str( (const char*)buf ); 
}


int shell_printf( char *fmt, ... )
{
    va_list ap;
    int n;
    char buf[256];

    va_start( ap, fmt );
    n = vsprintf( buf, fmt, ap );
    shell_driver_write( buf, n );
    va_end( ap );
    return n;
}


void shell_write_line( const char *str )
{
    if( str )
        shell_driver_write( (const char*)str, strlen(str) );
    shell_driver_write( "\r\n", 2 );
}


int shell_eval_int( const char *str, int *i )
{
    return sscanf(str, "%i", i) == 1 ? 1 : 0;
}


void shell_set_errnum( int errnum )
{
    cb.errnum = errnum;
}


int shell_get_errnum( void )
{
    return cb.errnum;
}


int shell_add_cmd_table( const shell_cmd_t *cmd_table )
{
    int i;

    for( i=0; i<SHELL_CMD_TABLE_LEN; i++ )
    {
        if( ! cb.cmd_table[i] )
        {
            cb.cmd_table[i] = cmd_table;
            return 1;
        }
    }
    return 0;
}


static const char *shell_get_prompt( void )
{
    if( cb.prompt_hook )
        return (*cb.prompt_hook)();
    if( cb.errnum < 0 )
        return "?>";
    else if( cb.errnum > 0 )
        return "!>";
    else
        return "=>";
}


void shell_set_prompt_hook( const char *(*hook)(void) )
{
    cb.prompt_hook = hook;
}


char *shell_get_buf( void )
{
    return cb.cmdline;
}


static int shell_split_cmdline_into_argvs( char *cmd_line )
{
    char *p = cmd_line;

    cb.argc = 0;
    while( *p && ((*p == ' ') || (*p == '\t')) )
        p++;
    if( !*p )
        return cb.argc;
    while( (cb.argc < SHELL_ARGV_LEN) && *p )
    {
        cb.argv[cb.argc++] = p;
        while( *p && ((*p != ' ') && (*p != '\t')) )
            p++;
        if( !*p )
            break;
        *p++ = 0;
        while( *p && ((*p == ' ') || (*p == '\t')) )
            p++;
        if( !*p )
            break;
    }
    return cb.argc; 
}


static int shell_search_command( int index, char *name )
{
    int i=0;
    const shell_cmd_t *ct = cb.cmd_table[index];
    if( !ct )
        return -1;
    for( i=0; ct->name; i++, ct++ )
    {
        if( strlen(name) > 1 )
        {
            if( strcmp( name, ct->name ) == 0 )
                return i;
        }
        else
        {
            if( *name == ct->sname )
                return i;
        }
    }
    return -1;
}


static void shell_add_cmd_to_history(void)
{
    char *p1, *p2;
    if( cb.cmdline_len == 0 )
        return;
    if( cb.cmdline_len > SHELL_CMDLINE_HISTORY_LEN )
        return;
    p1 = &cb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN];
    p2 = p1 - cb.cmdline_len - 1;
    while( p1 >= cb.cmdline_history )
        *p1-- = *p2--;
    strcpy( cb.cmdline_history, cb.cmdline );
    p1 = &cb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN];
    while( *p1 )
        *p1-- = 0;
    cb.history_index = 0;
}


static char *shell_get_history(int index)
{
    char *p = cb.cmdline_history;
    while( index > 1 )
    {
        while(*p++);  /* move to next item */
        if( !*p || p >= &cb.cmdline_history[SHELL_CMDLINE_HISTORY_LEN] )
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
        cb.errnum = 0;
        shell_driver_write_char( '\n' );
        return 1;
    }
    else if( c == '\r' )  /* ignored */
    {
    }
    else if( (cb.cmdline_len == 0) && ((c == ' ') || (c == '\t')) )  /* ignore */
    {
    }
    else if( (c == '\b') || (c == 0x7F) )  /* backspace, DEL */
    {
        if( cb.cmdline_len && cb.cmdline_cursor )
        {
            if( cb.cmdline_len == cb.cmdline_cursor )
            {
                cb.cmdline[--cb.cmdline_len] = 0;
                shell_write_str( "\b \b" );
                cb.cmdline_cursor--;
            }
            else
            {
                for( i=cb.cmdline_cursor; i<cb.cmdline_len; i++ )
                    cb.cmdline[i-1] = cb.cmdline[i];
                cb.cmdline[--cb.cmdline_len] = 0;
                cb.cmdline_cursor--;
                shell_driver_write_char( '\b' );
                shell_write_str( &cb.cmdline[cb.cmdline_cursor] );
                shell_driver_write_char( ' ' );
                for( i=cb.cmdline_len+1; i>cb.cmdline_cursor; i-- )
                    shell_driver_write_char( '\b' );
            }
        }
    }
    else if( c == 0x03 )  /* Ctrl-C, reset line */
    {
        shell_driver_reset();
        cb.cmdline_len = 0;
        cb.cmdline_cursor = 0;
        cb.errnum = 0;
        cb.history_index = 0;
        return -1;
    }
    else if( c == 0x1A )  /* Ctrl-Z, end of input */
    {
        cb.cmdline[cb.cmdline_cursor] = 0;
        return -2;
    }
    else if( (c == 0x10) || (c == 0x0E) )  /* Ctrl-P/N, previous/next history */
    {
        p = 0;
        if( c == 0x10 )  /* Ctrl-P */
        {
            p = shell_get_history( cb.history_index + 1 );
            if( p )
                cb.history_index += 1;
        }
        else if( c == 0x0E )  /* Ctrl-N */
        {
            if( cb.history_index > 1 )
            {
                p = shell_get_history( cb.history_index - 1 );
                if( p )
                    cb.history_index -= 1;
            }
        }
        if( p )
        {
            while( cb.cmdline_cursor < cb.cmdline_len )
            {
                shell_write_str( " " );
                cb.cmdline_cursor++;
            }
            while( cb.cmdline_len )
            {
                shell_write_str( "\b \b" );
                cb.cmdline_len--;
            }
            strcpy( cb.cmdline, p );
            cb.cmdline_cursor = cb.cmdline_len = strlen(cb.cmdline);
            shell_write_str( cb.cmdline );
        }
    }
    else if( c == 0x01 )  /* Ctrl-A, begin */
    {
        while( cb.cmdline_cursor )
        {
            shell_driver_write_char( '\b' );
            cb.cmdline_cursor--;
        }
    }
    else if( c == 0x05 )  /* Ctrl-E, end */
    {
        while( cb.cmdline_cursor < cb.cmdline_len )
            shell_driver_write_char( cb.cmdline[cb.cmdline_cursor++] );
    }
    else if( c == 0x06 )  /* Ctrl-F, right */
    {
        if( cb.cmdline_cursor < cb.cmdline_len )
            shell_driver_write_char( cb.cmdline[cb.cmdline_cursor++] );
    }
    else if( c == 0x02 )  /* Ctrl-B, left */
    {
        if( cb.cmdline_cursor )
        {
            shell_driver_write_char( '\b' );
            cb.cmdline_cursor--;
        }
    }
    else if( c == 0x04 )  /* Ctrl-D, delete */
    {
        if( cb.cmdline_cursor < cb.cmdline_len )
        {
            for( i=cb.cmdline_cursor; i<cb.cmdline_len; i++ )
                cb.cmdline[i] = cb.cmdline[i+1];
            cb.cmdline[--cb.cmdline_len] = 0;
            shell_write_str( &cb.cmdline[cb.cmdline_cursor] );
            shell_driver_write_char( ' ' );
            for( i=cb.cmdline_len+1; i>cb.cmdline_cursor; i-- )
                shell_driver_write_char( '\b' );
        }
    }
    else if( c == 0x0B )  /* Ctrl-K, kill remaining */
    {
        if( cb.cmdline_cursor < cb.cmdline_len )
        {
            cb.cmdline[cb.cmdline_cursor] = 0;
            for( i=cb.cmdline_cursor; i<cb.cmdline_len; i++ )
                shell_driver_write_char( ' ' );
            for( i=cb.cmdline_cursor; i<cb.cmdline_len; i++ )
                shell_driver_write_char( '\b' );
            cb.cmdline_len = cb.cmdline_cursor;
        }
    }
    else if( c == 0x0C )  /* Ctrl-L, update, ignore */
    {
    }
    else if( c == 0 )  /* NULL, ignore */
    {
    }
    else if( cb.cmdline_len < SHELL_CMDLINE_LEN )  /* new char */
    {
        if( c == '\t')  /* convert TAB to space */
            c = ' ';
        if( cb.cmdline_cursor == cb.cmdline_len )  /* append */
        {
            cb.cmdline[cb.cmdline_len++] = c;
            cb.cmdline[cb.cmdline_len] = 0;
            cb.cmdline_cursor++;
            shell_driver_write_char( c );
        }
        else  /* insert */
        {
            cb.cmdline[cb.cmdline_len+1] = 0;
            for( i=cb.cmdline_len; i>cb.cmdline_cursor; i-- )
                cb.cmdline[i] = cb.cmdline[i-1];
            cb.cmdline_len++;
            cb.cmdline[cb.cmdline_cursor++] = c;
            shell_write_str( &cb.cmdline[cb.cmdline_cursor-1] );
            for( i=cb.cmdline_len; i>cb.cmdline_cursor; i-- )
                shell_driver_write_char( '\b' );
        }
    }
    return 0;
}

                
int shell_read_line( char *buf, const char *prompt )
{
    char c;
    int s;
    cb.cmdline_len = 0;
    cb.cmdline_cursor = 0;
    if( buf )
        *buf = 0;
    if( prompt )
        shell_write_str( prompt );
    else
        shell_write_str( SHELL_INPUT_SUB_PROMPT ); 
    while( 1 )
    {
        if( shell_driver_read_char(&c) == -1 )
            continue;
        s = shell_process_char(c);
        switch( s ) 
        {
        case 1:  /* end of line */
            cb.cmdline[cb.cmdline_len] = 0;
            if( cb.cmdline_len )
                shell_add_cmd_to_history();
            if( buf )
                strcpy( buf, cb.cmdline );
            return cb.cmdline_len;
        case -1:  /* Ctrl-C */
        case -2:  /* Ctrl-Z, end of input */
            shell_write_str("\r\n");
            return s;
        }
    }
}


int shell_process_command(void)
{
    int (*cmd)(int argc, char *argv[]);
    int i, j;
    if( shell_split_cmdline_into_argvs( cb.cmdline ) )
    {
        for( j = 0; j < SHELL_CMD_TABLE_LEN; j++ )
        {
            i = shell_search_command( j, cb.argv[0] );
            if( i == -1 )
                continue;
            else;
                break;
        }
        if( i == -1 )
        {
            shell_write_str( "Invalid command: " );
            shell_write_line( cb.argv[0] );
            cb.errnum = -1;
        }
        else
        {
            cmd = ((shell_cmd_t*)(cb.cmd_table[j] + i))->cmd;
            cb.errnum = (*cmd)( cb.argc, cb.argv );
        }
    }
    cb.cmdline[0] = 0;
    cb.cmdline_len = 0; 
    cb.cmdline_cursor = 0;
    return 1;
}

 
int shell_print_help( const char *cmd, int show_hidden )
{
    int i, j;

    for( i = 0; (i < SHELL_CMD_TABLE_LEN) && cb.cmd_table[i]; i++ )
    {
        for( j=0; cb.cmd_table[i][j].name; j++ )
        {
            if( cmd && *cmd )
            {
                if( strlen(cmd) > 1 )
                {
                    if( strcmp(cmd, cb.cmd_table[i][j].name) != 0 )
                        continue;
                }
                else
                {
                    if( (strcmp(cmd, cb.cmd_table[i][j].name) != 0) || \
                        (*cmd != cb.cmd_table[i][j].sname) )
                        continue;
                }
            }
            if( !show_hidden && (cb.cmd_table[i][j].flag == CMD_HIDDEN) )
                continue;
            shell_write_str( cb.cmd_table[i][j].name );
            if( cb.cmd_table[i][j].sname )
            {
                shell_write_char( '/' );
                shell_write_char( cb.cmd_table[i][j].sname );
            }
            shell_write_str( "  " );
            shell_write_line( cb.cmd_table[i][j].description );
            shell_write_str( "  " );
            shell_write_line( cb.cmd_table[i][j].usage );
        }
    }
    return 0;
}


int shell_init( const shell_cmd_t *cmd_table )
{
    memset( &cb, 0, sizeof(shell_control_block_t) );
    cb.cmd_table[0] = cmd_table;
    return shell_driver_init();
}


void shell_run( void )
{
    shell_write_str("\r\n");
    shell_write_str( shell_get_prompt() );
    
    while( 1 )
    {
        switch( shell_read_line(0, "") )
        {
        case 0:  /* empty line */
            break;
        case -2:  /* Ctrl-Z, end of input */
        case -1:  /* Ctrl-C */
            //shell_write_str("\r\n");
            break;
        default:  /* commands */
            shell_process_command();
        }
        shell_write_str( shell_get_prompt() );
    }
}


char *shell_read_multi_lines( const char *prompt )
{
    char *buf1=0, *buf2=shell_get_buf(), *p;
    int len=0, alloc_size=SHELL_READ_LINES_ALLOC_SIZE_INC;

    buf1 = malloc(SHELL_READ_LINES_ALLOC_SIZE_INC);
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
            {
                //shell_printf("buf1 @ %08X, len=%d\n", buf1, len );
                //shell_write_line(buf1);
                return buf1;
            }
            goto abort;
        case -1:  /* Ctrl-C, stop */
            goto abort;
        default:  /* normal line */
            len += strlen(buf2);
            if( len >= alloc_size )
            {
                while( alloc_size < len+1 )
                    alloc_size += SHELL_READ_LINES_ALLOC_SIZE_INC;
                p = realloc( buf1, len );
                if( !p )
                    goto alloc_err;
                buf1 = p; 
            }
            strcat( buf1, buf2 );
            //shell_printf("buf1 @ %08X, len=%d\n", buf1, len );
            //shell_write_line(buf1);
            break;
        }
    }

abort:
    if( buf1 )
        free(buf1);
    return 0;
alloc_err:
    shell_write_line("failed to allocate memory");
    if( buf1 )
        free(buf1);
    return 0;
}


