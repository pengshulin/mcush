/* STM32F429xG, 1024kB FLASH, 256+4kB SRAM, with external upto 256MB SDRAM */

USER_HEAP_SIZE = 100K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM (rw)    : ORIGIN = 0x20000000, LENGTH = 192K 
    CCMSRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K 
    BKPSRAM (rw) : ORIGIN = 0x40024000, LENGTH = 4K 
    SDRAM (rw)   : ORIGIN = 0xD0000000, LENGTH = 256M
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

    .sdram (NOLOAD):
    {
        . = ALIGN(4);
        *(.sdram)
        . = ALIGN(4);
    } >SDRAM
 }

