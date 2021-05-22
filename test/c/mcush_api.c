/* MCUSH device C-API demo
 * control one/multi devices in C
 * this demo version uses syscall in blocking mode
 * MCUSH designed by Peng Shulin, all rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef WIN32
    #include <windows.h>
#else
    #include <termios.h>
#endif
#include "mcush_api.h"


int mcush_open( mcush_dev_t *device, const char *ttyname, int baudrate )
{
    if( (device==NULL) || (ttyname==NULL) )
        return MCUSH_ERR_API;
    memset( (void*)device, 0, sizeof(mcush_dev_t) );

#ifdef WIN32
    HANDLE handle;
    COMMTIMEOUTS timeout;
    DCB dcb;

    handle = CreateFile( ttyname, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                      OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL, NULL);
    if( handle == INVALID_HANDLE_VALUE )
        return MCUSH_ERR_IO;

    device->handle = handle;

    SetupComm( handle, 1024, 1024);
    timeout.ReadIntervalTimeout = 100;
    timeout.ReadTotalTimeoutMultiplier = 5000;
    timeout.ReadTotalTimeoutConstant = 5000;
    timeout.WriteTotalTimeoutMultiplier = 500;
    timeout.WriteTotalTimeoutConstant = 2000;
    SetCommTimeouts(handle, &timeout);

    GetCommState(handle, &dcb);
    dcb.BaudRate = baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(handle, &dcb);
    return 1;
#else
    int ret;
    struct termios newtio;
    speed_t speed;

    device->handle = open( ttyname, O_RDWR);
    if( device->handle < 0 )
        return MCUSH_ERR_IO;

    device->ttyname = strdup(ttyname);
    switch( baudrate )
    {
    case 150: speed = B150; break;
    case 200: speed = B200; break;
    case 300: speed = B300; break;
    case 600: speed = B600; break;
    case 1200: speed = B1200; break;
    case 1800: speed = B1800; break;
    case 2400: speed = B2400; break;
    case 4800: speed = B4800; break;
    case 0:
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    case 460800: speed = B460800; break;
    default: return MCUSH_ERR_API;
    }
    cfsetispeed( &newtio, speed );
    cfsetospeed( &newtio, speed );

    memset( &newtio, 0, sizeof(struct termios) );
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_lflag = 0;
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 0;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 50;    /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */
    ret = tcflush( device->handle, TCIFLUSH );
    if( ret < 0 )
        return MCUSH_ERR_IO;
    ret = tcsetattr( device->handle, TCSANOW, &newtio );
    if( ret < 0 )
        return MCUSH_ERR_IO;

    return 1;
#endif
}


int mcush_close( mcush_dev_t *device )
{
    if( device->handle )
    {
#ifdef WIN32
        CloseHandle(device->handle);
#else
        close( device->handle );
#endif
        device->handle = 0;
    }
    return 1;
}


int mcush_putc( mcush_dev_t *device, char c )
{
#ifdef WIN32
    DWORD wCount;

    if( ! WriteFile( device->handle, (PCVOID)&c, 1, &wCount, NULL) )
        return 0;
    return wCount ? 1 : 0;
#else
    int ret = write( device->handle, &c, 1 );
    if( ret < 0 )
        return ret;
    ret = tcdrain( device->handle );
    if( ret < 0 )
        return ret;
    return 1;
#endif
}


int mcush_puts( mcush_dev_t *device, const char *str )
{
    int count=0;
    char *p=(char*)str;

    while(*p)
    {
        if( mcush_putc( device, *p++ ) == 0 )
            break;
        count++;
    }
    return count;
}


int mcush_getc( mcush_dev_t *device, char *c )
{
#ifdef WIN32
    DWORD wCount;

    if( ! ReadFile( device->handle, (PVOID)c, 1, &wCount, NULL) )
        return 0;
    return wCount ? 1 : 0;
#else
    return read( device->handle, c, 1 );
#endif
}


int mcush_read_line( mcush_dev_t *device, char *s )
{
    return 0;
}


int mcush_wait_until_prompts( mcush_dev_t *device )
{
    char *receive=NULL;
    char newline[512];
    char *p, c;
    int newline_len, receive_len, receive_buf_len;
    int prompt_type=-1;

    newline_len = receive_len = receive_buf_len = 0;
    while( 1 )
    {
        if( mcush_getc( device, &c ) <= 0 )
            return MCUSH_ERR_TIMEOUT;  /* timeout */
        newline[newline_len++] = c;
        /* compare with prompt */
        if( (newline_len==2) && (newline[1]=='>') )
        {
            switch( newline[0] )
            {
            case '=':
                prompt_type=MCUSH_PROMPT_TYPE_NORMAL;
                break;
            case '?':
                prompt_type=MCUSH_PROMPT_TYPE_SYNTAX_ERR;
                break;
            case '!':
                prompt_type=MCUSH_PROMPT_TYPE_EXEC_ERR;
                break;
            }
            if( prompt_type >= 0 )
            {
                device->prompt_type = prompt_type;
                device->prompt[0] = newline[0];
                device->prompt[1] = newline[1];
                device->prompt[2] = 0;
                device->response = receive;
                return prompt_type;
            }
        }
        if( c == MCUSH_TERMINATOR_READ )
        {
            if( receive == NULL )
            {
                /* allocate new buf */
                receive = malloc( MCUSH_ALLOC_BYTES );
                if( receive == NULL )
                    return MCUSH_ERR_MEMORY;
                receive_buf_len = MCUSH_ALLOC_BYTES;
                memcpy( receive, newline, newline_len );
                receive_len = newline_len;
            }
            else if( receive_len + newline_len >= receive_buf_len )
            {
                /* expand buf size */
                p = realloc( receive, receive_buf_len + MCUSH_ALLOC_BYTES );
                if( p == NULL )
                {
                    free( receive );
                    return MCUSH_ERR_MEMORY;
                }
                receive = p;
                receive_buf_len += MCUSH_ALLOC_BYTES;
                memcpy( &receive[receive_len], newline, newline_len );
                receive_len += newline_len;
            }
            else
            {
                /* append buf */
                memcpy( &receive[receive_len], newline, newline_len );
                receive_len += newline_len;
            }
            newline_len = 0;
        }
    }
    return prompt_type;
}


int mcush_connect( mcush_dev_t *device )
{
    /* send reset to the device and get one prompt */
    if( mcush_putc( device, MCUSH_TERMINATOR_RESET ) == 0 )
        return 0;
    if( mcush_wait_until_prompts( device ) == 0 )
        return 1;
    return 0;
}


int mcush_write_command( mcush_dev_t *device, const char *cmd )
{
    int ret, cmd_len=strlen(cmd);

    if( mcush_puts( device, cmd ) != cmd_len )
        return MCUSH_ERR_IO;
    if( mcush_putc( device, MCUSH_TERMINATOR_WRITE ) < 0 )
        return MCUSH_ERR_IO;
    ret = mcush_wait_until_prompts( device );
    if( ret < 0 )
        return ret;
    /* check cmd response from the top line */
    if( (memcmp( cmd, device->response, cmd_len ) == 0) &&
            (device->response[cmd_len] == MCUSH_TERMINATOR_WRITE) )
    {
        device->result = &device->response[cmd_len+1];
        return strlen(device->result);
    }
    return MCUSH_ERR_RET_CMD_NOT_MATCH;
}


int mcush_scpi_idn( mcush_dev_t *device, char *output )
{
    int ret;

    if( output == NULL )
        return MCUSH_ERR_API;
    ret = mcush_write_command( device, "*idn?" );
    if( ret <= 0 )
        return ret;
    strcpy( output, device->result );
    return 1;
}


int mcush_scpi_rst( mcush_dev_t *device )
{
    int ret;

    ret = mcush_write_command( device, "*rst" );
    if( ret <= 0 )
        return 0;
    return 1;
}


