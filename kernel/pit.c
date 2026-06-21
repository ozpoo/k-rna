#include "pit.h"
#include "idt.h"
#include "pic.h"
#include "task.h"

static void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* these are accessed from asm */
extern uint32_t* get_current_esp_ptr(void);
extern uint32_t  get_next_esp(void);
extern int       need_switch;

int need_switch = 0;
uint32_t next_esp_val = 0;

void pit_handler(void) {
    pic_eoi(0);
    /* just count ticks, no switching */
}

/* pit_isr:
   1. save context onto current task's stack
   2. call pit_handler to pick next task
   3. if need_switch: swap esp and restore next task's context
   4. iret into next task */
__asm__(
    ".global pit_isr\n"
    "pit_isr:\n"
    "    pusha\n"
    "    call pit_handler\n"
    "    cmpl $0, need_switch\n"
    "    je no_switch\n"
    "    movl $0, need_switch\n"
    "    call get_prev_esp_ptr\n"
    "    movl %esp, (%eax)\n"
    "    movl next_esp_val, %esp\n"
    "no_switch:\n"
    "    popa\n"
    "    iret\n"
);

extern void pit_isr(void);

void pit_install(unsigned int hz) {
    unsigned int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    unsigned char mask;
    __asm__ volatile ("inb $0x21, %0" : "=a"(mask));
    mask &= ~0x01;
    __asm__ volatile ("outb %0, $0x21" : : "a"(mask));

    idt_set_gate(32, (uint32_t)pit_isr);
}