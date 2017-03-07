/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <stdio.h>
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
    char buf[128];

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


void shell_set_errno( int errno )
{
    cb.errno = errno;
}


int shell_get_errno( void )
{
    return cb.errno;
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
    if( cb.errno < 0 )
        return "?>";
    else if( cb.errno > 0 )
        return "!>";
    else
        return "=>";
}


void shell_set_prompt_hook( const char *(*hook)(void) )
{
    cb.prompt_hook = hook;
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
        if( strcmp( name, ct->name ) == 0 )
            return i;
        if( ct->sname && (strcmp( name, ct->sname ) == 0) )
            return i;
    }
    return -1;
}


static int shell_process_char( char c )
{
    int (*cmd)(int argc, char *argv[]);
    int i, j;

    if( c == '\n' )
    {
        cb.errno = 0;
        shell_driver_write_char( '\n' );
        if( cb.cmdline_len )
        {
            strcpy( cb.cmdline_history, cb.cmdline );
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
                    cb.errno = -1;
                }
                else
                {
                    cmd = ((shell_cmd_t*)(cb.cmd_table[j] + i))->cmd;
                    cb.errno = (*cmd)( cb.argc, cb.argv );
                }
            }
        }
        shell_write_str( shell_get_prompt() );
        cb.cmdline[0] = 0;
        cb.cmdline_len = 0; 
        cb.cmdline_cursor = 0;
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
        cb.errno = 0;
        shell_write_str( "\r\n" );
        shell_write_str( shell_get_prompt() );
    }
    else if( c == 0x10 )  /* Ctrl-P, previous history */
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
        strcpy( cb.cmdline, cb.cmdline_history );
        cb.cmdline_cursor = cb.cmdline_len = strlen(cb.cmdline);
        shell_write_str( cb.cmdline );
    }
    else if( c == 0x0E )  /* Ctrl-N, next history */
    {
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
    else if( c == 0 )  /* NULL, ignore */
    {
    }
    else if( cb.cmdline_len < SHELL_CMDLINE_LEN )  /* new char */
    {
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


int shell_print_help( void )
{
    int i, j;

    for( i = 0; (i < SHELL_CMD_TABLE_LEN) && cb.cmd_table[i]; i++ )
    {
        for( j=0; cb.cmd_table[i][j].name; j++ )
        {
            shell_write_str( cb.cmd_table[i][j].name );
            shell_write_str( "  " );
            shell_write_line( cb.cmd_table[i][j].help );
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
    char c;
    
    shell_write_str("\r\n");
    shell_write_str( shell_get_prompt() );
    
    while( 1 )
    {
        if( shell_driver_read_char(&c) == -1 )
            continue;
        shell_process_char( c );
    }
}


