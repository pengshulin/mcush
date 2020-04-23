/* STM32F103xE, 512kB FLASH, 64kB SRAM */
/* use only the upper half part of the flash */

USER_HEAP_SIZE = 40K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08040000, LENGTH = 256K
    SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 64K 
}

INCLUDE default.ld
