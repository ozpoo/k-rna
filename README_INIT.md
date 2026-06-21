**Step 1: Install the toolchain**

You're on a Mac, so we'll use Homebrew. You need a cross-compiler that targets bare metal x86 (not macOS, not Linux — *nothing*).

```bash
brew install nasm qemu xorriso
```

Then the cross-compiler. The easiest path on Mac is:

```bash
brew install x86_64-elf-gcc x86_64-elf-binutils
brew install i386-elf-binutils i386-elf-gcc
```

Verify:
```bash
i386-elf-gcc --version
nasm --version
qemu-system-i386 --version
```

All three should respond. If the tap gives you trouble, let me know — there's a manual build path as fallback.

---

**Step 2: Project structure**

Create this layout:

```
mykernel/
├── boot/
│   └── boot.asm        ← assembly boot stub
├── kernel/
│   └── kernel.c        ← your first C kernel code
├── linker.ld           ← tells the linker how to arrange memory
└── Makefile
```

---

**Step 3: The boot stub**

This is the assembly that runs *first* — before your C code. It satisfies the Multiboot spec (what GRUB expects), sets up a stack, then jumps to your C `kernel_main`.

`boot/boot.asm`:
```nasm
; Multiboot header constants
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
    resb 16384          ; 16KB stack
stack_top:

section .text
global _start
_start:
    mov esp, stack_top  ; set up the stack
    call kernel_main    ; jump to C
    cli                 ; if kernel_main returns, halt
.hang:
    hlt
    jmp .hang
```

---

**Step 4: The kernel**

`kernel/kernel.c`:
```c
// VGA text buffer lives at this address — always, on x86
static unsigned short* const VGA = (unsigned short*)0xB8000;

// Each character is 2 bytes: [color | ascii]
// 0x0F = white text on black background
static void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        VGA[i] = (unsigned short)str[i] | (0x0F << 8);
    }
}

void kernel_main(void) {
    print("hello from mykernel");
    // just hang forever
    while (1) {}
}
```

---

**Step 5: The linker script**

This tells the linker where in memory to place everything. Multiboot kernels load at 1MB (`0x100000`).

`linker.ld`:
```ld
ENTRY(_start)

SECTIONS {
    . = 0x100000;

    .text ALIGN(4K) : {
        *(.multiboot)
        *(.text)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
}
```

---

**Step 6: Makefile**

`Makefile`:
```makefile
CC      = i386-elf-gcc
AS      = nasm
LD      = i386-elf-gcc

CFLAGS  = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib
ASFLAGS = -f elf32
LDFLAGS = -m32 -ffreestanding -nostdlib -T linker.ld

OBJS = boot/boot.o kernel/kernel.o

all: mykernel.bin

boot/boot.o: boot/boot.asm
	$(AS) $(ASFLAGS) -o $@ $

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c -o $@ $

mykernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -lgcc

run: mykernel.bin
	qemu-system-i386 -kernel mykernel.bin

clean:
	rm -f boot/boot.o kernel/kernel.o mykernel.bin
```

---

**Step 7: Build and run**

```bash
make
make run
```

QEMU will open and you should see:

```
hello from mykernel
```

white text on black, top-left corner of the screen.

---

**What just happened, precisely:**

1. QEMU acted as GRUB — it can load Multiboot kernels directly with `-kernel`
2. Your assembly ran first, set up a 16KB stack, called `kernel_main`
3. `kernel_main` wrote directly to `0xB8000` — the VGA text buffer hardwired into x86
4. No OS, no libc, no runtime. Just your code and the hardware.

---

```bash
brew install helix
```

Just type `hx` to open it.

