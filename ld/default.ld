/* deault link scripts */

USER_STACK_ADDR = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = .;
        KEEP(*(.isr_vector))            /* Startup code */
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
    
    /* read-write data are appended/copied to the end of flash */
    . = ALIGN(4);
    _sidata = .;  /* copy from here at startup */

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
