/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>



#ifndef HALT_ON_SBRK_FAIL
#define HALT_ON_SBRK_FAIL  0
#endif

/* use the waste area (if exists) between heap and stack */
#ifndef EXPAND_HEAP_TO_STACK
#define EXPAND_HEAP_TO_STACK  1
#endif
    
extern char _sheap, _eheap, _sstack, _estack;
char *heap_end;
caddr_t _sbrk(int incr) {
    char *prev_heap_end;
    
    if( !heap_end ) 
        heap_end = &_sheap;
    prev_heap_end = heap_end;
    heap_end += incr;
#if EXPAND_HEAP_TO_STACK
    if( heap_end > &_sstack )
#else
    if( heap_end > &_eheap )
#endif
    {
#if HALT_ON_SBRK_FAIL
        halt("sbrk");
#else
        return (caddr_t)(-1);
#endif
    }
    return (caddr_t)prev_heap_end;
}

 
void _halt(void)
{
    int i, led_num=hal_led_get_num();

    portENTER_CRITICAL();
    for( i=0; i<led_num; i++ )
        hal_led_clr(i);
    while(1)
    {
        for( i=0; i<led_num; i++ )
            hal_led_toggle(i);
        hal_delay_ms( 500 );
    }
}

volatile char *halt_message;
void _halt_with_message(const char *message)
{
    halt_message = (char*)message;
    _halt();
}


int _open( const char *name, int flag, int m )
{
    char buf[8];
    int fd=mcush_open( name, (const char *)parse_file_flag(flag, buf) );
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
    return 0;
}


int _isatty( int fd )
{
    return 0;
}


void _exit(int status)
{
    halt("exit");
    while(1);
}


void _kill(int pid)
{

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
        tick = xTaskGetTickCount();
        s = tick / configTICK_RATE_HZ;
        tick = tick - s * configTICK_RATE_HZ;
        tick = tick * 1000000 / configTICK_RATE_HZ; 
        tv->tv_sec = s;
        tv->tv_usec = tick;
    }

    tz->tz_minuteswest = 0;
    tz->tz_dsttime = 0;
    return 0;
}


int _link(const char *oldpath, const char *newpath)
{
    return -1;
}


int _unlink(const char *pathname)
{
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




