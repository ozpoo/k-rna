#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PAGE_SIZE     4096
#define PMM_MAX_PAGES 8192   /* tracks 32MB */

void     pmm_install(void);
uint32_t pmm_alloc(void);
void     pmm_free(uint32_t addr);
int      pmm_free_pages(void);

#endif