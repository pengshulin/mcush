/* STM32F407xE, 512kB FLASH, 192+4kB SRAM */

USER_HEAP_SIZE = 1K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rw)    : ORIGIN = 0x20000000, LENGTH = 128K 
    CCMSRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K 
    BKPSRAM (rw) : ORIGIN = 0x40024000, LENGTH = 4K 
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

    .bkpsram (NOLOAD):
    {
        . = ALIGN(4);
        *(.bkpsram)
        . = ALIGN(4);
    } >BKPSRAM

}

