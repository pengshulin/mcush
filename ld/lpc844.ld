/* LPC844, 64k FLASH, 8kB SRAM */

USER_HEAP_SIZE = 4;
USER_STACK_SIZE = 256;
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64K 
    SRAM (rw)  : ORIGIN = 0x10000000, LENGTH = 8K 
}

INCLUDE default.ld
PROVIDE( _vStackTop = _estack );

