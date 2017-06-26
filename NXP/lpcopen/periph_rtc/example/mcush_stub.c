/* MCUSH designed by Peng Shulin, all rights reserved. */
#include <sys/types.h>

    
extern char _sheap, _eheap;
char *heap_end;
caddr_t _sbrk(int incr) {
    char *prev_heap_end;
    
    if( !heap_end ) 
        heap_end = &_sheap;
    prev_heap_end = heap_end;
    heap_end += incr;
    if( heap_end > &_eheap )
        heap_end = &_eheap;
    return (caddr_t)prev_heap_end;
}

 
int _open( const char *name, int flag, int m )
{
    return -1;
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
    //halt("EXIT");
    while(1);
}

void _kill(int pid)
{

}

int _getpid(void)
{
    return 0;
}



