/* deault link scripts */

USER_STACK_ADDR = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = .;
        KEEP(*(.isr_vector))
        . = ALIGN(4);
        _eisr_vector = .;
    } >FLASH
 
    .text :
    {
        . = ALIGN(4);
        _text = . ;
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = . ;
    } >FLASH

    /* TODO: as the length of the duplicated .data section is hard to calculate, 
       put my extra flash data before it */
    
    . = ALIGN(4);
    _sidata = .;
    /* read-write .data are duplicated here and copied to sram at startup */

    /* END OF FLASH */

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = . ;
        *(.data*)
        . = ALIGN(4);
        _edata = . ;
    } >SRAM
    
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } >SRAM
    
    .userheap :
    {
        . = ALIGN(4);
        _sheap = . ;
        . = . + USER_HEAP_SIZE ;
        . = ALIGN(4);
        _eheap = . ;
    } >SRAM

    .userstack (USER_STACK_ADDR) :
    {
        . = ALIGN(4);
        _sstack = .;
        . = . + USER_STACK_SIZE ;
        . = ALIGN(4);
        _estack = .;
    } >SRAM 

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

}
