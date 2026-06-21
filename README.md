# mykernel

A minimal, open kernel built from scratch targeting x86, with ARM planned.
No OS. No libc. No runtime. Just code and hardware.

Built on a 2017 Intel MacBook Pro, developed in Helix, run in QEMU.

---

## Philosophy

- Minimal — every line has a reason
- Open — anyone can read it, run it, hack it
- Educational — the codebase is meant to be understood, not just used
- Portable — architecture layer designed for clean porting to ARM

---

## Current State — v0.6

Boots in QEMU. Runs two concurrent tasks. Handles keyboard input.
Paging enabled. Physical memory managed via bitmap allocator.

### What works

| Component       | Description                                      |
|----------------|--------------------------------------------------|
| Boot stub       | Assembly, Multiboot compliant, zeroes BSS        |
| VGA driver      | Direct framebuffer, colors, scroll, cursor hide  |
| IDT             | 256 gate descriptor table                        |
| PIC             | 8259A remapped, IRQ0/IRQ1 unmasked               |
| Keyboard        | IRQ1 handler, US scancode map                    |
| PMM             | Bitmap allocator, 4KB pages                      |
| VMM             | Two-level x86 page tables, paging enabled        |
| PIT             | Programmable interval timer at 100Hz             |
| Scheduler       | Cooperative, pure asm context switch             |
| Config          | Single config.h for all tunable values           |

### Memory layout

```
0x100000    kernel load address (1MB)
0x200000    PMM bitmap
0x201000    page directory
0x202000    page table 0 (identity maps 0-4MB)
0x203000    page table B (identity maps VGA region)
0x380000    task stacks (8KB each, up to 8 tasks)
0xB8000     VGA text buffer
```

### Toolchain

```bash
brew install x86_64-elf-gcc x86_64-elf-binutils nasm qemu helix
```

### Build and run

```bash
make          # build
make run      # run in QEMU
make clean    # clean build artifacts
```

---

## Project structure

```
mykernel/
├── boot/
│   └── boot.asm          # multiboot entry, BSS zero, stack setup
├── kernel/
│   ├── config.h          # all tunable constants — start here
│   ├── kernel.c          # kernel_main — init sequence
│   ├── vga.c/h           # VGA text mode driver
│   ├── idt.c/h           # interrupt descriptor table
│   ├── pic.c/h           # 8259A PIC driver
│   ├── keyboard.c/h      # keyboard IRQ handler
│   ├── pmm.c/h           # physical memory manager
│   ├── vmm.c/h           # virtual memory manager
│   ├── pit.c/h           # timer driver
│   ├── task.c/h          # task structures + scheduler
│   └── switch.asm        # assembly context switch
├── linker.ld             # memory layout script
└── Makefile
```

---

## Roadmap

### Next — v0.7: Multiboot info parsing
- Read actual RAM map from bootloader
- Replace hardcoded CFG_MEMORY_MB with real detected memory
- Print memory map on boot

### v0.8: VBE framebuffer
- Query BIOS for available video modes before protected mode
- Switch to high resolution pixel framebuffer
- Replace VGA text driver with pixel renderer
- Configurable resolution via config.h

### v0.9: Preemptive scheduler
- PIT forces task switches — tasks don't need to yield
- Per-task kernel stack for safe interrupt handling
- Priority levels

### v1.0: Syscall interface
- int 0x80 dispatch table
- Tasks request kernel services safely
- Foundation for userspace

### v1.1: ELF loader
- Load ELF binaries into task contexts
- Separate kernel and user address spaces

### v1.2: Ramdisk + VFS
- Simple in-memory filesystem
- Virtual filesystem interface

### v1.3: ARM port
- Target: Samsung Galaxy Tab
- New arch/ directory structure
- arch/x86/ and arch/arm/ layers
- Shared kernel core

---

## Key concepts by file

**boot.asm** — the CPU starts here in 32-bit protected mode thanks to GRUB/Multiboot.
Sets up a stack, zeroes BSS, calls kernel_main.

**config.h** — change screen size, memory, task count, timer frequency here.
One file, one place.

**pmm.c** — tracks 4KB pages with a bitmap. 0 = free, 1 = used.
pmm_alloc() returns a physical address. pmm_free() returns it to the pool.

**vmm.c** — builds x86 two-level page tables. Page directory at CFG_PAGE_DIR_ADDR.
Identity mapped for now (virtual == physical). Foundation for process isolation.

**task.c + switch.asm** — each task has a saved esp. Context switch saves
callee-saved registers, swaps esp, restores. Pure asm, no compiler interference.

---

## Notes

- Currently cooperative scheduling — tasks must call schedule() to yield
- Preemptive switching (PIT-driven) is the next major milestone
- ARM port planned after syscall interface is solid
- RISC-V is a longer term target (fully open ISA, no boot blobs)

---

## Running on real hardware

Not yet — QEMU only for now. Real hardware boot via USB planned after
VBE framebuffer is working. The Intel MacBook Pro is the first real target.
Samsung Galaxy Tab (ARM) is second.

EOF
```

```bash
make clean && make run
```

Actually no — no need to rebuild, just:

```bash
cat README.md
```

Make sure it looks right, then:

```bash
git init
git add .
git commit -m "mykernel v0.6 — multitasking, paging, keyboard, config"
```