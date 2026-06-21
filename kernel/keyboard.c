#include "keyboard.h"
#include "idt.h"
#include "pic.h"
#include "vga.h"

static unsigned char inb(unsigned short port) {
    unsigned char val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static const char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        if (scancode < sizeof(scancode_map)) {
            char c = scancode_map[scancode];
            if (c == '\b') {
                vga_print("<bksp>", 0x0C);
            } else if (c) {
                vga_putchar(c, 0x0A);
            }
        }
    }
    pic_eoi(1);
}

__asm__ (
    ".global keyboard_isr\n"
    "keyboard_isr:\n"
    "    pusha\n"
    "    call keyboard_handler\n"
    "    popa\n"
    "    iret\n"
);

extern void keyboard_isr(void);

void keyboard_install(void) {
    idt_set_gate(33, (uint32_t)keyboard_isr);
}
