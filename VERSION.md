mykernel v0.4
══════════════════════════════════

boot/boot.asm          ← assembly, BSS zeroing, stack setup
kernel/config.h        ← single source of truth
kernel/vga.c/h         ← direct framebuffer driver
kernel/idt.c/h         ← interrupt descriptor table
kernel/pic.c/h         ← 8259A programmable interrupt controller  
kernel/keyboard.c/h    ← IRQ1 scancode handler
kernel/pmm.c/h         ← bitmap physical memory allocator
kernel/vmm.c/h         ← two-level x86 page tables
kernel/pit.c/h         ← programmable interval timer
kernel/task.c/h        ← task structures, cooperative scheduler
kernel/switch.asm      ← pure assembly context switch
kernel/kernel.c        ← ties it all together
linker.ld              ← memory layout
Makefile               ← build system

A bootable, multitasking, memory-managing, interrupt-driven kernel. From scratch. On a Saturday.