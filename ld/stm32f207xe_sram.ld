/* stm32f207xe, 5124kB flash, 128kB sram */

USER_HEAP_SIZE = 40K;
USER_STACK_SIZE = 1K;
MEMORY
{
    SRAM (rw)    : ORIGIN = 0x20000000, LENGTH = 128K 
}


INCLUDE default_sram.ld
