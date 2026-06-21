#include "vmm.h"
#include "pmm.h"
#include "vga.h"
#include "config.h"

#define PAGE_DIR_ADDR   CFG_PAGE_DIR_ADDR
#define PAGE_TBL_0_ADDR CFG_PAGE_TBL_ADDR
#define PAGE_TBL_B_ADDR CFG_PAGE_TBL_B_ADDR

static uint32_t* page_dir   = (uint32_t*)PAGE_DIR_ADDR;
static uint32_t* page_tbl_0 = (uint32_t*)PAGE_TBL_0_ADDR;
static uint32_t* page_tbl_b = (uint32_t*)PAGE_TBL_B_ADDR;

void vmm_map(uint32_t virt, uint32_t phys, uint32_t flags) {
    int dir_idx = virt >> 22;
    int tbl_idx = (virt >> 12) & 0x3FF;

    if (!(page_dir[dir_idx] & PAGE_PRESENT)) {
        /* allocate a new table from PMM post-paging */
        uint32_t t = pmm_alloc();
        if (!t) { vga_print("[VMM] oom\n", 0x0C); return; }
        uint32_t* tbl = (uint32_t*)t;
        for (int i = 0; i < 1024; i++) tbl[i] = 0;
        page_dir[dir_idx] = t | PAGE_PRESENT | PAGE_WRITABLE;
    }

    uint32_t* table = (uint32_t*)(page_dir[dir_idx] & 0xFFFFF000);
    table[tbl_idx] = (phys & 0xFFFFF000) | flags | PAGE_PRESENT;
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

void vmm_unmap(uint32_t virt) {
    int dir_idx = virt >> 22;
    int tbl_idx = (virt >> 12) & 0x3FF;
    if (!(page_dir[dir_idx] & PAGE_PRESENT)) return;
    uint32_t* table = (uint32_t*)(page_dir[dir_idx] & 0xFFFFF000);
    table[tbl_idx] = 0;
    __asm__ volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

int vmm_is_mapped(uint32_t virt) {
    int dir_idx = virt >> 22;
    int tbl_idx = (virt >> 12) & 0x3FF;
    if (!(page_dir[dir_idx] & PAGE_PRESENT)) return 0;
    uint32_t* table = (uint32_t*)(page_dir[dir_idx] & 0xFFFFF000);
    return table[tbl_idx] & PAGE_PRESENT;
}

uint32_t vmm_get_phys(uint32_t virt) {
    int dir_idx = virt >> 22;
    int tbl_idx = (virt >> 12) & 0x3FF;
    if (!(page_dir[dir_idx] & PAGE_PRESENT)) return 0;
    uint32_t* table = (uint32_t*)(page_dir[dir_idx] & 0xFFFFF000);
    return table[tbl_idx] & 0xFFFFF000;
}

void vmm_install(void) {
    /* zero directory and both static tables */
    for (int i = 0; i < 1024; i++) page_dir[i]   = 0;
    for (int i = 0; i < 1024; i++) page_tbl_0[i] = 0;
    for (int i = 0; i < 1024; i++) page_tbl_b[i] = 0;

    /* wire tables into directory */
    page_dir[0]   = PAGE_TBL_0_ADDR | PAGE_PRESENT | PAGE_WRITABLE;
    page_dir[0xB] = PAGE_TBL_B_ADDR | PAGE_PRESENT | PAGE_WRITABLE;

    /* identity map first 4MB via table 0 — no function calls, direct writes */
    for (int i = 0; i < 1024; i++)
        page_tbl_0[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITABLE;

    /* identity map VGA region (0xB8000-0xBFFFF) via table B */
    for (int i = 0; i < 1024; i++)
        page_tbl_b[i] = (0xB0000 + i * 0x1000) | PAGE_PRESENT | PAGE_WRITABLE;

    /* load CR3 and enable paging */
    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_dir));
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}