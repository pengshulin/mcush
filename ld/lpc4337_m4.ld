/* LPC4337, 1024kB FLASH, 136kB SRAM */

USER_HEAP_SIZE = 16K;
USER_STACK_SIZE = 1K;
MEMORY
{
    FLASH (rx)   : ORIGIN = 0x1A000000, LENGTH = 512K
    FLASH2 (rx)  : ORIGIN = 0x1B000000, LENGTH = 512K
    SRAM (rw)    : ORIGIN = 0x10000000, LENGTH = 32K 
    SRAM2 (rw)   : ORIGIN = 0x10080000, LENGTH = 32K 
    SHARE (rw)   : ORIGIN = 0x10088000, LENGTH = 8K 
    AHBSRAM (rw) : ORIGIN = 0x20000000, LENGTH = 64K 
    SDRAM (rw)   : ORIGIN = 0x28000000, LENGTH = 64M 
}

USER_STACK_ADDR = ORIGIN(SRAM) + LENGTH(SRAM) - USER_STACK_SIZE;


SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        _sisr_vector = .;
        KEEP(*(.isr_vector))
        *(.after_vectors)
        . = ALIGN(4);
        _eisr_vector = .;
    } >FLASH
 
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
        _text = . ;
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = . ;
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
    PROVIDE( _vStackTop = _estack );

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

}

SECTIONS
{
    .flash2 (NOLOAD):
    {
        . = ALIGN(4);
        *(.flash2)
        . = ALIGN(4);
    } >FLASH2

    .ahbsram (NOLOAD):
    {
        . = ALIGN(4);
        *(.ahbsram)
        . = ALIGN(4);
    } >AHBSRAM

    .share (NOLOAD):
    {
        . = ALIGN(4);
        *(.share)
        . = ALIGN(4);
    } >SHARE

    .sdram (NOLOAD):
    {
        . = ALIGN(4);
        *(.sdram)
        . = ALIGN(4);
    } >SDRAM
}
