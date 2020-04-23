/* STM32F103xC, 256kB FLASH, 64kB SRAM */
/* upper half part of the flash */

USER_HEAP_SIZE = 1K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08020000, LENGTH = 128K 
    SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 64K 
}

INCLUDE default.ld
