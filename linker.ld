ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
        . += 0x10000;
        _bss_end = .;
    }
}