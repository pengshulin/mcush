/* deault link scripts: code in FLASH and .bss in SRAM */

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = . ;
        KEEP(*(.isr_vector))
        . = ALIGN(4);
        _eisr_vector = . ;
        *(.after_vectors*)
    } > FLASH
 
    .signature :
    {
        KEEP(*(.signature))
    } > FLASH

    .global_section_table :
    {
        __section_table_start = .;
        __data_section_table = .;
        /* copy table: SRC, DEST, LEN */
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        /* zeroing table: START, LEN */
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
    } >FLASH
 
 
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
    } > FLASH

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
    
    .dma :
    {
        . = ALIGN(4);
        *(.dma)
        . = ALIGN(4);
    } > SRAM
        
    .userheap :
    {
        . = ALIGN(4);
        _sheap = . ;
        *(.userheap)
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


ASSERT( _sidata + SIZEOF(.data) <= ORIGIN(FLASH) + LENGTH(FLASH), "flash full, fail to append data image" )
ASSERT( _eheap <= _sstack, "heap/stack conflict" )

