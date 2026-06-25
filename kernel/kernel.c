#include "config.h"
#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "pmm.h"
#include "vmm.h"
#include "pit.h"
#include "task.h"
#include "multiboot.h"

void task_a(void) {
    int count = 0;
    while (1) {
        unsigned short* vga = (unsigned short*)0xB8000;
        const char* label = "task A: ";
        for (int i = 0; label[i]; i++)
            vga[22 * 80 + i] = (unsigned short)label[i] | (0x0A << 8);
        int n = count++;
        int col = 8;
        char buf[16];
        int len = 0;
        if (n == 0) {
            vga[22 * 80 + col] = '0' | (0x0E << 8);
        } else {
            while (n > 0) { buf[len++] = '0' + (n % 10); n /= 10; }
            for (int i = len - 1; i >= 0; i--)
                vga[22 * 80 + col++] = (unsigned short)buf[i] | (0x0E << 8);
        }
        schedule();
    }
}

void task_b(void) {
    int count = 0;
    while (1) {
        unsigned short* vga = (unsigned short*)0xB8000;
        const char* label = "task B: ";
        for (int i = 0; label[i]; i++)
            vga[23 * 80 + i] = (unsigned short)label[i] | (0x0D << 8);
        int n = count++;
        int col = 8;
        char buf[16];
        int len = 0;
        if (n == 0) {
            vga[23 * 80 + col] = '0' | (0x0E << 8);
        } else {
            while (n > 0) { buf[len++] = '0' + (n % 10); n /= 10; }
            for (int i = len - 1; i >= 0; i--)
                vga[23 * 80 + col++] = (unsigned short)buf[i] | (0x0E << 8);
        }
        schedule();
    }
}

void kernel_main(uint32_t magic, void* mb_ptr) {
    vga_hide_cursor();
    vga_clear();

    vga_print("Karna v0.7\n", 0x0F);
    vga_print("==========\n\n", 0x0F);

    if (magic == 0x36D76289) {
        vga_print("[BOOT] multiboot2\n", 0x0A);
        multiboot_parse(magic, mb_ptr);
    } else if (magic == 0x2BADB002) {
        vga_print("[BOOT] multiboot1\n", 0x0A);
        multiboot1_parse((multiboot1_info_t*)mb_ptr);
    } else {
        vga_print("[BOOT] unknown magic: ", 0x0C);
        vga_print_hex(magic, 0x0C);
        vga_print("\n", 0x0C);
    }
    vga_print("\n", 0x0F);

    idt_install();
    vga_print("[IDT]      ", 0x0B);
    vga_print("installed\n", 0x0A);

    pic_install();
    vga_print("[PIC]      ", 0x0B);
    vga_print("remapped\n", 0x0A);

    keyboard_install();
    vga_print("[KEYBOARD] ", 0x0B);
    vga_print("ready\n", 0x0A);

    pmm_install(&g_mb_info);
    vga_print("[PMM]      ", 0x0B);
    vga_print_int(pmm_free_pages(), 0x0E);
    vga_print(" pages free / ", 0x0A);
    vga_print_int(pmm_total_pages(), 0x0E);
    vga_print(" total\n", 0x0A);

    vmm_install();
    vga_print("[VMM]      ", 0x0B);
    vga_print("paging enabled\n", 0x0A);

    task_install();
    task_create(task_a);
    task_create(task_b);
    vga_print("[TASKS]    ", 0x0B);
    vga_print("2 tasks created\n", 0x0A);

    pit_install(CFG_PIT_HZ);
    vga_print("[PIT]      ", 0x0B);
    vga_print("timer ready\n\n", 0x0A);

    vga_print("scheduler running two tasks below:\n", 0x0F);
    vga_print("type something:  ", 0x0E);

    __asm__ volatile ("sti");
    task_start();

    while (1) { __asm__ volatile ("hlt"); }
}