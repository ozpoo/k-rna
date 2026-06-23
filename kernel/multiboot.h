#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/* flags telling us which fields are valid */
#define MB_FLAG_MEM      0x001   /* mem_lower/mem_upper valid */
#define MB_FLAG_MMAP     0x040   /* mmap valid */

/* memory map entry types */
#define MB_MMAP_AVAILABLE 1      /* free RAM */
#define MB_MMAP_RESERVED  2      /* don't touch */

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;     /* KB of low memory (below 1MB) */
    uint32_t mem_upper;     /* KB of high memory (above 1MB) */
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;   /* size of memory map in bytes */
    uint32_t mmap_addr;     /* physical address of memory map */
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader;
    uint32_t apm_table;
} __attribute__((packed)) multiboot_info_t;

typedef struct {
    uint32_t size;          /* size of this entry minus 4 */
    uint32_t addr_low;
    uint32_t addr_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_t;

void multiboot_parse(multiboot_info_t* mb);
uint32_t multiboot_get_ram_kb(void);

#endif