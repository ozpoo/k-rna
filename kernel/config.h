#ifndef CONFIG_H
#define CONFIG_H

/* ─── display ─────────────────────────────────────────────── */
#define CFG_SCREEN_WIDTH    80
#define CFG_SCREEN_HEIGHT   25
#define CFG_VGA_ADDR        0xB8000

/* ─── memory ──────────────────────────────────────────────── */
// #define CFG_MEMORY_MB       32
#define CFG_MEMORY_MB       128
#define CFG_PAGE_SIZE       4096
// #define CFG_MAX_PAGES       8192        /* 32MB / 4KB */
#define CFG_MAX_PAGES       32768    /* 128MB / 4KB */
#define CFG_KERNEL_PAGES    512         /* reserved for kernel (2MB) */
#define CFG_BITMAP_ADDR     0x200000
#define CFG_PAGE_DIR_ADDR   0x201000
#define CFG_PAGE_TBL_ADDR   0x202000
#define CFG_PAGE_TBL_B_ADDR 0x203000

/* ─── tasks ───────────────────────────────────────────────── */
#define CFG_MAX_TASKS       8
#define CFG_STACK_SIZE      8192
#define CFG_STACK_BASE      0x380000

/* ─── scheduler ───────────────────────────────────────────── */
#define CFG_PIT_HZ          100

/* ─── kernel layout ───────────────────────────────────────── */
#define CFG_KERNEL_LOAD     0x100000    /* 1MB — multiboot standard */

#endif