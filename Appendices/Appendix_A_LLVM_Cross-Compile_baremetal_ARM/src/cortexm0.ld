ENTRY(main)
MEMORY {
    ROM(rx)   : ORIGIN = 0x0,       LENGTH = 0x11FFFF       /* 1.125 MB */
    ROM2(rx)  : ORIGIN = 0x120000,  LENGTH = 0x20000        /* 128 KB */
    RAM(rwx)  : ORIGIN = 0x800000,  LENGTH = 0x400000       /* 4 MB */
}

SECTIONS {
    /*  The code should be loaded at address 0x0 */
    /* the dot (.) symbol is location counter */
    . = ORIGIN(ROM);
    .text : {
        /* When link-time garbage collection is in use (‘--gc-sections’), it is often useful
            to mark sections that should not be eliminated. This is accomplished by 
            surrounding an input section’s wildcard entry with KEEP() */
        KEEP(*(.vector_table));
        . = ALIGN(4);
        __vec_end__ = .;
        /* Input sections: .text section in all files. */
        * (.text)
        . = ALIGN(4);
        __end_text__ = .;
    } > ROM /* assign .text section to a previously defined region of memory 'ROM' */

    . = ORIGIN(ROM2);
    .rodata : {
         __dataro_start__ = .;
        *(.rodata)
        . = ALIGN(4);
         __dataro_end__ = .;
    } > ROM2

    . = ORIGIN(RAM);
    .data : {
        __data_start__ = .;
        * (.data);
        . = ALIGN(4);
        __heap_low = .;     /* for _sbrk */
        . = . + 0x10000;    /* 64kB of heap memory */
        __heap_top = .;     /* for _sbrk */
        __data_end__ = .;
    } > RAM

