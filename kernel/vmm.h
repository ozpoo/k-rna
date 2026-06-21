#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define PAGE_PRESENT    0x1
#define PAGE_WRITABLE   0x2
#define PAGE_USER       0x4

void     vmm_install(void);
void     vmm_map(uint32_t virt, uint32_t phys, uint32_t flags);
void     vmm_unmap(uint32_t virt);
int      vmm_is_mapped(uint32_t virt);
uint32_t vmm_get_phys(uint32_t virt);

#endif