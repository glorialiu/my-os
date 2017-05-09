#include <stdint.h>

extern void *alloc_heap_vpage(void);

extern void *free_page(void *address);
extern void *free_pages(void *address, int num);

void walk_ptable(uint64_t);

void ptable_init();

typedef struct CR3 {
    uint64_t reserved1:3;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t reserved2:7;
    uint64_t base_addr: 40;
    uint64_t reserved3:12;
}  __attribute__((packed)) CR3;

typedef struct PML4 {
    uint64_t p:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ignore:3;
    uint64_t available:3;
    uint64_t base_address:40;
    uint64_t avl_bits:11;
    uint64_t nx:1;
} __attribute__((packed)) PML4;

typedef struct PDP {
    uint64_t p:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ignore:1;
    uint64_t zero:1;
    uint64_t ignore2:1;
    uint64_t available:3;
    uint64_t base_address:40;
    uint64_t avl_bits:11;
    uint64_t nx:1;
} __attribute__((packed)) PDP;

typedef struct PD {
    uint64_t p:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t ignore:1;
    uint64_t zero:1;
    uint64_t ignore2:1;
    uint64_t available:3;
    uint64_t base_address:40;
    uint64_t avl_bits:11;
    uint64_t nx:1;
} __attribute__((packed)) PD;

typedef struct PT {
    uint64_t p:1;
    uint64_t rw:1;
    uint64_t us:1;
    uint64_t pwt:1;
    uint64_t pcd:1;
    uint64_t a:1;
    uint64_t d:1;
    uint64_t pat:1;
    uint64_t g:1;
    uint64_t available:2;
    uint64_t demand:1;
    uint64_t base_address:40;
    uint64_t avl_bits:11;
    uint64_t nx:1;
} __attribute__((packed)) PT;


