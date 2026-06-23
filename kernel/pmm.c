#include "pmm.h"
#include "vga.h"
#include "config.h"

#define BITMAP_ADDR  CFG_BITMAP_ADDR
#define MAX_PAGES    65536          /* supports up to 256MB */

static uint8_t* bitmap = (uint8_t*)BITMAP_ADDR;
static int total_pages_count = 0;

static void bitmap_set(int page) {
    bitmap[page / 8] |= (1 << (page % 8));
}

static void bitmap_clear(int page) {
    bitmap[page / 8] &= ~(1 << (page % 8));
}

static int bitmap_test(int page) {
    return bitmap[page / 8] & (1 << (page % 8));
}

/* mark a physical region as free */
static void pmm_free_region(uint32_t addr, uint32_t len) {
    uint32_t start_page = addr / PAGE_SIZE;
    uint32_t num_pages  = len  / PAGE_SIZE;
    for (uint32_t i = 0; i < num_pages; i++) {
        if (start_page + i < MAX_PAGES)
            bitmap_clear(start_page + i);
    }
}

/* mark a physical region as used */
static void pmm_reserve_region(uint32_t addr, uint32_t len) {
    uint32_t start_page = (addr / PAGE_SIZE);
    uint32_t num_pages  = (len  / PAGE_SIZE) + 1;
    for (uint32_t i = 0; i < num_pages; i++) {
        if (start_page + i < MAX_PAGES)
            bitmap_set(start_page + i);
    }
}

void pmm_install(multiboot_info_t* mb) {
    /* start with everything reserved */
    for (int i = 0; i < MAX_PAGES / 8; i++)
        bitmap[i] = 0xFF;

    if (mb && (mb->flags & MB_FLAG_MMAP)) {
        /* walk the real memory map and free available regions */
        multiboot_mmap_t* entry = (multiboot_mmap_t*)mb->mmap_addr;
        multiboot_mmap_t* end   = (multiboot_mmap_t*)(mb->mmap_addr + mb->mmap_length);

        while (entry < end) {
            if (entry->type == MB_MMAP_AVAILABLE)
                pmm_free_region(entry->addr_low, entry->len_low);
            entry = (multiboot_mmap_t*)((uint32_t)entry + entry->size + 4);
        }

        /* calculate total tracked pages from actual RAM */
        total_pages_count = (mb->mem_upper * 1024) / PAGE_SIZE;
        if (total_pages_count > MAX_PAGES)
            total_pages_count = MAX_PAGES;
    } else {
        /* fallback to config value if no mmap */
        total_pages_count = (CFG_MEMORY_MB * 1024 * 1024) / PAGE_SIZE;
        pmm_free_region(CFG_KERNEL_PAGES * PAGE_SIZE,
                        total_pages_count * PAGE_SIZE);
    }

    /* always reserve the low 2MB — kernel, page tables, bitmap live here */
    pmm_reserve_region(0x0, 0x200000);

    /* reserve our bitmap itself */
    pmm_reserve_region(CFG_BITMAP_ADDR, MAX_PAGES / 8);

    /* reserve page directory and tables */
    pmm_reserve_region(CFG_PAGE_DIR_ADDR, PAGE_SIZE * 3);

    /* reserve task stacks */
    pmm_reserve_region(CFG_STACK_BASE, CFG_MAX_TASKS * CFG_STACK_SIZE);
}

uint32_t pmm_alloc(void) {
    for (int i = 0; i < total_pages_count; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (uint32_t)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void pmm_free(uint32_t addr) {
    int page = addr / PAGE_SIZE;
    if (page < total_pages_count)
        bitmap_clear(page);
}

int pmm_free_pages(void) {
    int count = 0;
    for (int i = 0; i < total_pages_count; i++)
        if (!bitmap_test(i)) count++;
    return count;
}

int pmm_total_pages(void) {
    return total_pages_count;
}