/* deault link scripts for programs running in sram */

USER_STACK_ADDR = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = . ;
        KEEP(*(.isr_vector))
        _eisr_vector = . ;
    } > SRAM

    .text :
    {
        . = ALIGN(4);
        _stext = . ;
        *(.text*)
        /**(.init)*/
        *(.rodata*)
        . = ALIGN(4);
        _etext = . ;
    } > SRAM

    _sidata = . ;

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = . ;
        *(.data*)
        . = ALIGN(4);
        _edata = . ;
    } > SRAM 

    .bss :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > SRAM
    
    .userheap :
    {
        . = ALIGN(4);
        _sheap = . ;
        . = . + USER_HEAP_SIZE ;
        . = ALIGN(4);
        _eheap = . ;
    } > SRAM

    .userstack (USER_STACK_ADDR) :
    {
        . = ALIGN(4);
        _sstack = . ;
        . = . + USER_STACK_SIZE ;
        . = ALIGN(4);
        _estack = . ;
    } > SRAM 

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.ARM.exidx)
        *(.glue*)
        *(.init)
        *(.fini)
    }

}


ASSERT( _sidata + SIZEOF(.data) <= ORIGIN(SRAM) + LENGTH(SRAM), "sram full, fail to append data image" )

