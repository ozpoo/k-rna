#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include "multiboot.h"

#define PAGE_SIZE 4096

void     pmm_install(multiboot_info_t* mb);
uint32_t pmm_alloc(void);
void     pmm_free(uint32_t addr);
int      pmm_free_pages(void);
int      pmm_total_pages(void);

#endif