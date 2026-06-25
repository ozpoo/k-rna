BITS 32

; ── Multiboot1 header ──────────────────────────────────────────
MB1_MAGIC    equ 0x1BADB002
MB1_FLAGS    equ 0x0
MB1_CHECK    equ -(MB1_MAGIC + MB1_FLAGS)

; ── Multiboot2 header ──────────────────────────────────────────
MB2_MAGIC    equ 0xE85250D6
MB2_ARCH     equ 0
MB2_LENGTH   equ mb2_end - mb2_start
MB2_CHECK    equ -(MB2_MAGIC + MB2_ARCH + MB2_LENGTH)

section .text
global _start
extern kernel_main
extern _bss_start
extern _bss_end

; MB2 header first
align 8
mb2_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd MB2_LENGTH
    dd MB2_CHECK
    dw 0
    dw 0
    dd 8
mb2_end:

; MB1 header immediately after
align 4
    dd MB1_MAGIC
    dd MB1_FLAGS
    dd MB1_CHECK

_start:
    ; save IMMEDIATELY before anything touches registers
    mov [saved_magic], eax
    mov [saved_ptr],   ebx

    mov esp, stack_top

    ; zero BSS
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; push saved values
    push dword [saved_ptr]
    push dword [saved_magic]
    call kernel_main

    cli
hang:
    hlt
    jmp hang

section .data
saved_magic dd 0
saved_ptr   dd 0

section .bss
align 16
stack_bottom:
    resb 65536
stack_top: