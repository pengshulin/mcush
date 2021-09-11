/* code runs in sram */

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = . ;
        KEEP(*(.isr_vector))
        . = ALIGN(4);
        _eisr_vector = . ;
        *(.after_vectors)
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
        LONG(0x43444E45);  /* ASCII: ENDC */
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
        . = . + USER_STACK_SIZE ; /* request stack area just for usage statistics */
    } > SRAM

    _sstack = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;
    _estack = ORIGIN(SRAM) + LENGTH(SRAM);


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
ASSERT( _eheap <= _sstack, "heap/stack conflict" )

