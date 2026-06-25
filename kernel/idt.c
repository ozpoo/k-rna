#include "idt.h"

#define IDT_SIZE 256

static idt_entry_t idt[IDT_SIZE];
static idt_ptr_t   idt_ptr;

void idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x10;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_install(void) {
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;
    idt_ptr.base  = (uint32_t)&idt;
    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}
