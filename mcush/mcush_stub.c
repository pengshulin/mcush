/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

#if defined(SUPPORT_NEWLIB)
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/fcntl.h>
#include <unistd.h>


#ifndef HALT_ON_SBRK_FAIL
#define HALT_ON_SBRK_FAIL  0
#endif

/* use the waste area (if exists) between heap and stack */
#ifndef EXPAND_HEAP_TO_STACK
#define EXPAND_HEAP_TO_STACK  1
#endif
    
extern char _sheap, _eheap, _sstack, _estack;

caddr_t _sbrk(int incr)
{
    static char *heap_end = NULL;
    char *prev_heap_end, *new_heap_end;
    
    if( heap_end == NULL ) 
        heap_end = &_sheap;
    prev_heap_end = heap_end;
    new_heap_end = heap_end + incr;
#if EXPAND_HEAP_TO_STACK
    if( new_heap_end > &_sstack )
#else
    if( new_heap_end > &_eheap )
#endif
    {
#if HALT_ON_SBRK_FAIL
        halt("sbrk");
#else
        return (caddr_t)(-1);
#endif
    }
    heap_end = new_heap_end;
    return (caddr_t)prev_heap_end;
}
 

char *parse_file_flag( int flag, char *buf )
{
    buf[0] = 0;

    switch( flag & 0x03 )
    {
    case O_RDONLY: strcpy(buf, "r");  break;
    case O_WRONLY: strcpy(buf, "w");  break;
    case O_RDWR:
    default:       strcpy(buf, "rw");  break;
    }
    if( flag & O_CREAT )
        strcat(buf, "+");
    if( flag & O_APPEND )
        strcat(buf, "a");
    return buf;
}

#if MCUSH_VFS
int _open( const char *name, int flag, int m )
{
    char buf[8];
    int fd;

    (void)m;
    fd = mcush_open( name, (const char *)parse_file_flag(flag, buf) );
    return fd>0 ? fd : -1;
}


size_t _read( int fd, void *buf, size_t len )
{
    return mcush_read( fd, buf, len );
}


size_t _write( int fd, const void *buf, size_t len )
{
    return mcush_write( fd, (void*)buf, len );
}


int _close( int fd )
{
    return mcush_close( fd );
}


off_t _lseek( int fd, off_t offset, int w )
{
    return mcush_seek( fd, offset, w );
}


//int _fstat( int fd, struct stat *s )
int _fstat( int fd, void *s )
{
    (void)fd;
    (void)s;
    return 0;
}


int _isatty( int fd )
{
    (void)fd;
    return 0;
}
#endif


void _exit(int status)
{
    (void)status;
    halt("exit");
    while(1);
}


void _kill(int pid)
{
    (void)pid;
}


int _getpid(void)
{
    return 0;
}


int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    unsigned int s, tick;

    if( get_rtc_tick( (uint32_t*)&tick ) )
    {
        tv->tv_sec = tick + 8*60*60;
        tv->tv_usec = 0;
    }
    else
    {
        tick = os_tick();
        s = tick / OS_TICK_RATE;
        tick = tick - s * OS_TICK_RATE;
        tick = tick * 1000000 / OS_TICK_RATE; 
        tv->tv_sec = s;
        tv->tv_usec = tick;
    }

    tz->tz_minuteswest = 0;
    tz->tz_dsttime = 0;
    return 0;
}


int _link(const char *oldpath, const char *newpath)
{
    (void)oldpath;
    (void)newpath;
    return -1;
}


int _unlink(const char *pathname)
{
    (void)pathname;
    return -1;
}


clock_t _times(struct tms *buf)
{
    buf->tms_utime = 0;
    buf->tms_stime = 0;
    buf->tms_cutime = 0;
    buf->tms_cstime = 0;
    return 0;
}



#endif
