#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MB2_MAGIC_EXPECTED  0x36D76289

/* tag types */
#define MB2_TAG_END         0
#define MB2_TAG_MEM         4
#define MB2_TAG_MMAP        6

/* mmap entry types */
#define MB_MMAP_AVAILABLE   1
#define MB_MMAP_RESERVED    2

/* flags for pmm compatibility */
#define MB_FLAG_MEM         0x001
#define MB_FLAG_MMAP        0x040

#define MB1_FLAG_MEM     0x001
#define MB1_FLAG_MMAP    0x040

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot1_info_t;

typedef struct {
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} __attribute__((packed)) multiboot1_mmap_t;

void multiboot1_parse(multiboot1_info_t* mb);

/* Multiboot2 info header — at the pointer GRUB passes in ebx */
typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed)) mb2_header_t;

/* every tag starts with this */
typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) mb2_tag_t;

/* tag type 4 — basic memory */
typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed)) mb2_tag_mem_t;

/* tag type 6 — memory map */
typedef struct {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} __attribute__((packed)) mb2_tag_mmap_t;

/* individual mmap entry */
typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) mb2_mmap_entry_t;

/* compatibility shim so pmm.c doesn't need changes */
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t mmap_addr;
    uint32_t mmap_length;
} multiboot_info_t;

typedef struct {
    uint32_t size;
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} multiboot_mmap_t;

void     multiboot_parse(uint32_t magic, void* mb2_ptr);
uint32_t multiboot_get_ram_kb(void);

/* filled in by multiboot_parse, used by pmm */
extern multiboot_info_t g_mb_info;

#endif