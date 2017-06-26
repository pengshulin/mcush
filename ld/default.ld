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
        . = ALIGN(4);
    } >FLASH


    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = . ;
        *(.data*)
        . = ALIGN(4);
        _edata = . ;
    } >SRAM 

    .data_copy_in_flash :
    {
        _sidata = .;
        . = _sidata + SIZEOF(.data);
        _eidata = .;
    } > FLASH

    .flash_magic_code : AT(_eidata)
    {
        BYTE(109);  /* 'm' */
        BYTE( 99);  /* 'c' */
        BYTE(117);  /* 'u' */
        BYTE(115);  /* 's' */
        BYTE(104);  /* 'h' */
    } > FLASH

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
