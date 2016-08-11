/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"
#include <sys/types.h>

caddr_t _sbrk(int incr) {
    extern char _sheap, _eheap;
    static char *heap_end;
    char *prev_heap_end;
    
    if (heap_end == NULL) {
        heap_end = &_sheap;
    }
    prev_heap_end = heap_end;
    if (heap_end + incr > &_eheap) {
        for(;;);
    }
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

 
#if DEBUG_HALT_MESSAGE
volatile char *halt_message = 0;
void halt(const char *message)
{
    halt_message = (char*)message;
    while(1);
}
#endif



