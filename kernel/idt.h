#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* one IDT entry — exactly 8 bytes, layout is mandated by x86 */
typedef struct {
    uint16_t offset_low;   /* lower 16 bits of handler address */
    uint16_t selector;     /* code segment selector */
    uint8_t  zero;         /* always 0 */
    uint8_t  type_attr;    /* type and attributes */
    uint16_t offset_high;  /* upper 16 bits of handler address */
} __attribute__((packed)) idt_entry_t;

/* what we hand to lidt */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

void idt_set_gate(int n, uint32_t handler);
void idt_install(void);

#endif