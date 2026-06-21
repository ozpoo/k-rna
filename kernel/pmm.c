#include "pmm.h"
#include "vga.h"
#include "config.h"

#define REAL_PAGES            CFG_MAX_PAGES
#define BITMAP_ADDR           CFG_BITMAP_ADDR
#define KERNEL_RESERVED_PAGES CFG_KERNEL_PAGES

static uint8_t* bitmap = (uint8_t*)CFG_BITMAP_ADDR;
static int total_pages  = CFG_MAX_PAGES;

static void bitmap_set(int page) {
    bitmap[page / 8] |= (1 << (page % 8));
}

static void bitmap_clear(int page) {
    bitmap[page / 8] &= ~(1 << (page % 8));
}

static int bitmap_test(int page) {
    return bitmap[page / 8] & (1 << (page % 8));
}

void pmm_install(void) {
    for (int i = 0; i < REAL_PAGES / 8; i++)
        bitmap[i] = 0xFF;
    for (int i = KERNEL_RESERVED_PAGES; i < REAL_PAGES; i++)
        bitmap_clear(i);
}

uint32_t pmm_alloc(void) {
    for (int i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return (uint32_t)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void pmm_free(uint32_t addr) {
    int page = addr / PAGE_SIZE;
    if (page < total_pages)
        bitmap_clear(page);
}

int pmm_free_pages(void) {
    int count = 0;
    for (int i = 0; i < total_pages; i++)
        if (!bitmap_test(i)) count++;
    return count;
}