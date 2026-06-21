MAGIC    equ 0x1BADB002
FLAGS    equ 0x0
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 65536
stack_top:

; separate interrupt stack — never swapped
int_stack_bottom:
    resb 16384
int_stack_top:

section .text
global _start
global int_stack_top
extern kernel_main
extern _bss_start
extern _bss_end

_start:
    mov esp, stack_top

    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    call kernel_main
    cli
hang:
    hlt
    jmp hang