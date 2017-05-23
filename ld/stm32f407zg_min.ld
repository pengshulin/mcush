/* STM32F407ZG, 1024kB FLASH, 192kB SRAM */

USER_HEAP_SIZE = 100K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM (rw)    : ORIGIN = 0x20000000, LENGTH = 128K 
    CCMSRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K 
}

INCLUDE default.ld

SECTIONS
{
    .ccmsram (NOLOAD):
    {
        . = ALIGN(4);
        *(.ccmsram)
        . = ALIGN(4);
    } >CCMSRAM
}

