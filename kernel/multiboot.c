#include "multiboot.h"
#include "vga.h"

static uint32_t total_ram_kb = 0;
multiboot_info_t g_mb_info = {0};

void multiboot_parse(uint32_t magic, void* mb2_ptr) {
    if (magic != MB2_MAGIC_EXPECTED) {
        vga_print("[BOOT] bad magic: 0x", 0x0C);
        vga_print_int(magic, 0x0C);
        vga_print("\n", 0x0C);
        return;
    }

    if (!mb2_ptr) {
        vga_print("[BOOT] null mb2 pointer!\n", 0x0C);
        return;
    }

    vga_print("[BOOT] mb2 magic ok\n", 0x0A);

    /* walk tags — they start 8 bytes in, after the header */
    uint32_t addr = (uint32_t)mb2_ptr + 8;
    mb2_header_t* hdr = (mb2_header_t*)mb2_ptr;
    uint32_t end = (uint32_t)mb2_ptr + hdr->total_size;

    while (addr < end) {
        mb2_tag_t* tag = (mb2_tag_t*)addr;

        if (tag->type == MB2_TAG_END) break;

        if (tag->type == MB2_TAG_MEM) {
            mb2_tag_mem_t* mem = (mb2_tag_mem_t*)tag;
            g_mb_info.flags    |= MB_FLAG_MEM;
            g_mb_info.mem_lower = mem->mem_lower;
            g_mb_info.mem_upper = mem->mem_upper;
            total_ram_kb = mem->mem_upper + 1024;

            vga_print("[BOOT] mem_lower: ", 0x0B);
            vga_print_int(mem->mem_lower, 0x0E);
            vga_print("KB\n", 0x0B);
            vga_print("[BOOT] mem_upper: ", 0x0B);
            vga_print_int(mem->mem_upper, 0x0E);
            vga_print("KB\n", 0x0B);
            vga_print("[BOOT] total RAM: ", 0x0B);
            vga_print_int(total_ram_kb / 1024, 0x0E);
            vga_print("MB\n", 0x0B);
        }

        if (tag->type == MB2_TAG_MMAP) {
            mb2_tag_mmap_t* mmap_tag = (mb2_tag_mmap_t*)tag;
            g_mb_info.flags      |= MB_FLAG_MMAP;
            g_mb_info.mmap_addr   = (uint32_t)mmap_tag + sizeof(mb2_tag_mmap_t);
            g_mb_info.mmap_length = tag->size - sizeof(mb2_tag_mmap_t);

            vga_print("[BOOT] memory map:\n", 0x0B);

            mb2_mmap_entry_t* entry = (mb2_mmap_entry_t*)g_mb_info.mmap_addr;
            mb2_mmap_entry_t* mend  = (mb2_mmap_entry_t*)(g_mb_info.mmap_addr + g_mb_info.mmap_length);

            while (entry < mend) {
                vga_print("  0x", 0x0F);
                vga_print_int((uint32_t)entry->addr, 0x0E);
                vga_print(" len:", 0x0F);
                vga_print_int((uint32_t)entry->len, 0x0E);
                vga_print(entry->type == MB_MMAP_AVAILABLE ? " [free]\n" : " [reserved]\n",
                          entry->type == MB_MMAP_AVAILABLE ? 0x0A : 0x0C);
                entry++;
            }
        }

        /* tags are 8-byte aligned */
        addr += (tag->size + 7) & ~7;
    }
}

uint32_t multiboot_get_ram_kb(void) {
    return total_ram_kb;
}

void multiboot1_parse(multiboot1_info_t* mb) {
    if (!mb) {
        vga_print("[BOOT] null ptr\n", 0x0C);
        return;
    }

    /* read fields as raw uint32 to avoid struct layout issues */
    uint32_t* raw = (uint32_t*)mb;

    vga_print("[BOOT] raw[0] flags:      0x", 0x0B);
    vga_print_int(raw[0], 0x0E);
    vga_print("\n", 0x0B);
    vga_print("[BOOT] raw[1] mem_lower:  ", 0x0B);
    vga_print_int(raw[1], 0x0E);
    vga_print("KB\n", 0x0B);
    vga_print("[BOOT] raw[2] mem_upper:  ", 0x0B);
    vga_print_int(raw[2], 0x0E);
    vga_print("KB\n", 0x0B);
    vga_print("[BOOT] raw[11] mmap_len:  ", 0x0B);
    vga_print_int(raw[11], 0x0E);
    vga_print("\n", 0x0B);
    vga_print("[BOOT] raw[12] mmap_addr: 0x", 0x0B);
    vga_print_int(raw[12], 0x0E);
    vga_print("\n", 0x0B);

    /* fill compat struct directly from raw */
    g_mb_info.flags      = raw[0];
    g_mb_info.mem_lower  = raw[1];
    g_mb_info.mem_upper  = raw[2];
    g_mb_info.mmap_length = raw[11];
    g_mb_info.mmap_addr  = raw[12];

    total_ram_kb = raw[2] + 1024;
}

// void multiboot1_parse(multiboot1_info_t* mb) {
//     if (!mb) {
//         vga_print("[BOOT] null mb1 ptr!\n", 0x0C);
//         return;
//     }

//     if (mb->flags & MB1_FLAG_MEM) {
//         total_ram_kb = mb->mem_upper + 1024;
//         vga_print("[BOOT] mem_lower: ", 0x0B);
//         vga_print_int(mb->mem_lower, 0x0E);
//         vga_print("KB\n", 0x0B);
//         vga_print("[BOOT] mem_upper: ", 0x0B);
//         vga_print_int(mb->mem_upper, 0x0E);
//         vga_print("KB\n", 0x0B);
//         vga_print("[BOOT] total RAM: ", 0x0B);
//         vga_print_int(total_ram_kb / 1024, 0x0E);
//         vga_print("MB\n", 0x0B);

//         /* fill compat struct for pmm */
//         g_mb_info.flags    |= MB_FLAG_MEM;
//         g_mb_info.mem_lower = mb->mem_lower;
//         g_mb_info.mem_upper = mb->mem_upper;
//     }

//     if (mb->flags & MB1_FLAG_MMAP) {
//         g_mb_info.flags      |= MB_FLAG_MMAP;
//         g_mb_info.mmap_addr   = mb->mmap_addr;
//         g_mb_info.mmap_length = mb->mmap_length;

//         vga_print("[BOOT] memory map:\n", 0x0B);

//         multiboot1_mmap_t* entry = (multiboot1_mmap_t*)mb->mmap_addr;
//         multiboot1_mmap_t* end   = (multiboot1_mmap_t*)(mb->mmap_addr + mb->mmap_length);

//         while (entry < end) {
//             vga_print("  0x", 0x0F);
//             vga_print_int(entry->addr_low, 0x0E);
//             vga_print(" len:", 0x0F);
//             vga_print_int(entry->len_low, 0x0E);
//             vga_print(entry->type == 1 ? " [free]\n" : " [reserved]\n",
//                       entry->type == 1 ? 0x0A : 0x0C);
//             entry = (multiboot1_mmap_t*)((uint32_t)entry + entry->size + 4);
//         }
//     }
// }