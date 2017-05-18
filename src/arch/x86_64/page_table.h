#include <stdint.h>

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

typedef PML4 page_table;

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

void ptable_init(page_table *pt);
void setup_page(void *addr, page_table *pt);
void setup_pages(void *addr, int numPages, page_table *pt);

PT *return_pt_entry(void *addr, page_table *pt);
void unresolved_pf();
void page_fault_handler(void *addr, page_table *pt);

PML4 *get_PML4(int, page_table *);
PDP *get_PDP(PML4 *entry, int);
PD *get_PD(PDP *entry,int);
PT *get_PT(PD* entry,int);

//void free_pt(PML4 *pml_entry);

extern void *alloc_heap_vpage(page_table *pt);
extern void *alloc_stack_vpage(page_table *pt);
void *alloc_user_vpage(page_table *pt);

extern void *free_page(void *address);
extern void *free_pages(void *address, int num);

extern void MMU_free_page(void *addr, page_table *pt);
extern void MMU_free_pages(void *addr, int num, page_table *pt);




