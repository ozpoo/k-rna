#include "multiboot.h"
#include "vga.h"
#include "config.h"

static uint32_t total_ram_kb = 0;

void multiboot_parse(multiboot_info_t* mb) {
    if (!mb) {
        vga_print("[BOOT] no multiboot info!\n", 0x0C);
        return;
    }

    /* basic memory info */
    if (mb->flags & MB_FLAG_MEM) {
        total_ram_kb = mb->mem_upper + 1024;  /* upper is above 1MB */
        vga_print("[BOOT] mem_lower: ", 0x0B);
        vga_print_int(mb->mem_lower, 0x0E);
        vga_print("KB\n", 0x0B);
        vga_print("[BOOT] mem_upper: ", 0x0B);
        vga_print_int(mb->mem_upper, 0x0E);
        vga_print("KB\n", 0x0B);
        vga_print("[BOOT] total RAM: ", 0x0B);
        vga_print_int(total_ram_kb / 1024, 0x0E);
        vga_print("MB\n", 0x0B);
    }

    /* detailed memory map */
    if (mb->flags & MB_FLAG_MMAP) {
        vga_print("[BOOT] memory map:\n", 0x0B);

        multiboot_mmap_t* entry = (multiboot_mmap_t*)mb->mmap_addr;
        multiboot_mmap_t* end   = (multiboot_mmap_t*)(mb->mmap_addr + mb->mmap_length);

        while (entry < end) {
            vga_print("  ", 0x0F);
            vga_print_int(entry->addr_low, 0x0E);
            vga_print(" len:", 0x0F);
            vga_print_int(entry->len_low, 0x0E);
            vga_print(entry->type == MB_MMAP_AVAILABLE ? " [free]\n" : " [reserved]\n",
                      entry->type == MB_MMAP_AVAILABLE ? 0x0A : 0x0C);

            /* advance to next entry */
            entry = (multiboot_mmap_t*)((uint32_t)entry + entry->size + 4);
        }
    }
}

uint32_t multiboot_get_ram_kb(void) {
    return total_ram_kb;
}