/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include "hal.h"
#include <sys/types.h>

#ifndef HALT_ON_SBRK_FAIL
#define HALT_ON_SBRK_FAIL  0
#endif

    
extern char _sheap, _eheap;
char *heap_end;
caddr_t _sbrk(int incr) {
    char *prev_heap_end;
    
    if( !heap_end ) 
        heap_end = &_sheap;
    prev_heap_end = heap_end;
    heap_end += incr;
    if( heap_end > &_eheap )
#if HALT_ON_SBRK_FAIL
        halt("sbrk");
#else
        heap_end = &_eheap;
#endif
    return (caddr_t)prev_heap_end;
}

 
void _halt(void)
{
    int i, led_num=hal_led_get_num();

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
    return -1;
}

size_t _read( int fd, void *buf, size_t len )
{
    return 0;
}


size_t _write( int fd, const void *buf, size_t len )
{
    return shell_driver_write( buf, len );
}

int _close( int fd )
{
    return 0;
}

off_t _lseek( int fd, off_t offset, int w )
{
    return 0;
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



