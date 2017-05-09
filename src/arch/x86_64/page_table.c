#include "page_table.h"
#include "page_alloc.h"
#include "mem_funcs.h"


#define NUM_PML4_ENTRIES 1
#define NUM_PDP_ENTRIES NUM_PML4_ENTRIES * 512
#define NUM_PD_ENTRIES NUM_PDP_ENTRIES * 512
#define NUM_PT_ENTRIES NUM_PD_ENTRIES * 512

#define BOTTOM_9BIT_MASK 511
#define BOTTOM_12BIT_MASK 0xFFF
#define BOTTOM_40BIT_MASK 0x3FFFFFFFFFF

#define ENTRY_SIZE 8

#define PAGE_SIZE 4096
static void *physicalPFStart = (void *)  0x0;

static void *kStackStart = (void *) 0x10000000000;

static void *kHeapStart = (void *) 0xF0000000000;
static void *kHeapEnd = (void *) 0xF0000000000;

static void *userSpaceStart = (void *) 0x100000000000;


PML4 pml_table[NUM_PML4_ENTRIES];
PDP pdp_table[NUM_PDP_ENTRIES];
PD pd_table[NUM_PD_ENTRIES];
PT pt_table[NUM_PT_ENTRIES];

void ptable_init() {
    //memset pml_table, pdp_table, etc to zeroes
    memset(pml_table, 0, NUM_PML4_ENTRIES * ENTRY_SIZE);
    memset(pdp_table, 0, NUM_PDP_ENTRIES * ENTRY_SIZE);
    memset(pd_table, 0, NUM_PD_ENTRIES * ENTRY_SIZE);
    memset(pt_table, 0, NUM_PT_ENTRIES * ENTRY_SIZE);


    // TODO: identity paging. done, but how exactly does this work? we didnt MMU_alloc pages, where were these pages "reserved"? in boot.asm?
    setup_idpaging();

    // TODO: load page table into CR3, do other set up necessary to load a page table?

    
}

void setup_idpaging() {
    pml_table[0] = 7; //shortcut to setup bottom 3 bits
    pdp_table[0] = 7;
    pd_table[0] = 7;

    int i = 0;
    int page_addr = 0;

    for (i = 0; i < NUM_PAGES_IDENTITY_MAPPED; i++) {
        pt_table[i] = page_addr | 7;
        page_addr+=PAGE_SIZE; //PAGE_SIZE is 4096 
    }
   
    
}

void setup_page(void *addr) {
    uint64_t address = (uint64_t) addr;
    

    int pmlIdx = (address >> 39 & BOTTOM_9BIT_MASK);
    int pdpIdx = (address >> 30 & BOTTOM_9BIT_MASK);
    int pdIdx = (address >> 21 & BOTTOM_9BIT_MASK);
    int ptIdx = (address >> 12 & BOTTOM_9BIT_MASK);
    int pageOffset = (address & BOTTOM_12BIT_MASK)

    int pmlRealIdx = pmlIdx;
    int pdpRealIdx = pmlRealIdx * 512;
    int pdRealIdx = pdpRealIdx * 512;
    int ptRealIdx = pdRealIdx * 512;

    uint64_t pmlAddress = (uint64_t) pml_table;
    uint64_t pdpAddress = (uint64_t) pdp_table + pdpRealIdx;
    uint64_t pdAddress = (uint64_t) pd_table + pdRealIdx;
    uint64_t ptAddress = (uint64_t) pt_table + ptReadIdx;


    //important bits -> present, address, user/supervisor, r/w bit
    PML4 *newPml = pml_table[pmlRealIdx];
    newPml->p = 1; // it is present
    newPml->rw = 1; //has read AND write access
    newPml->us = 1; // user mode code can access it
    newPml->base_address = pdpAddress & BOTTOM_40BIT_MASK; //putting address of next entry ( a PDP entry) it points to

    PDP *newPdp = pdp_table[pdpRealIdx];
    newPdp->p = 1; 
    newPdp->rw = 1; 
    newPdp->us = 1; 
    newPdp->base_address = pdAddress & BOTTOM_40BIT_MASK;
    
    PD *newPd = pd_table[pdReadIdx];
    newPd->p = 1; 
    newPd->rw = 1; 
    newPd->us = 1; 
    newPd->base_address = ptAddress & BOTTOM_40BIT_MASK;

    PT *newPt = pt_table[ptRealIdx];
    newPt->p = 1; 
    newPt->rw = 1; 
    newPt->us = 1; 
    //demand paging
    newPt->demand = 1;



    //allocate a physical page frame 
    //physPage = MMU_pf_alloc();
    //newPt->base_address = (uint64_t) physPage & BOTTOM_40BIT_MASK;

}

//walk the page table to return the correct page table entry for the address

//also check if valid
PT *return_pt_entry(void *addr) {
    uint64_t address = (uint64_t) addr;
    

    int pmlIdx = (address >> 39 & BOTTOM_9BIT_MASK);
    int pdpIdx = (address >> 30 & BOTTOM_9BIT_MASK);
    int pdIdx = (address >> 21 & BOTTOM_9BIT_MASK);
    int ptIdx = (address >> 12 & BOTTOM_9BIT_MASK);
    int pageOffset = (address & BOTTOM_12BIT_MASK)

    int pmlRealIdx = pmlIdx;
    int pdpRealIdx = pmlRealIdx * 512;
    int pdRealIdx = pdpRealIdx * 512;
    int ptRealIdx = pdRealIdx * 512;

    //should i use address or index to get the page table entry?
    /*
    uint64_t pmlAddress = (uint64_t) pml_table;
    uint64_t pdpAddress = (uint64_t) pdp_table + pdpRealIdx;
    uint64_t pdAddress = (uint64_t) pd_table + pdRealIdx;
    uint64_t ptAddress = (uint64_t) pt_table + ptReadIdx;
    */

    PML4 *pml_entry = pml_table[pmlRealIdx];
    PDP *pdp_entry = pdp_table[pdpRealIdx];
    PD *pd_entry = pd_table[pdReadIdx];
    PT *pt_entry = pt_table[ptRealIdx];

    //check if they're all present
    if(!(pml_entry->p && pdp_entry->p && pd_entry->p && pt_entry->p)) {
        unresolved_pf();
    }

    //do we need to check r/w?
    //eventually do something with user mode?
    return pt_entry;   

}

void unresolved_pf() {
    //TODO: get what its in CR2        
    faultCause = %CR2;
    printk("PAGE FAULT (UNRESOLVED): %d\n", (int) faultCause);
    asm volatile("hlt");
}

void page_fault_handler(void *addr) {
    uint64_t faultAddr = (uint64_t) addr;

    void *physPage;

    uint64_t faultCause;

    //walk page table to find pt entry, set to entry
    //return_pt_entry will check the pt entry for errors internally
    PT *entry = return_pt_entry(addr); 


    if (entry->demand == 1) {
        physPage = MMU_pf_alloc();
        entry->demand = 0;
        entry->base_address = (uint64_t) physPage & BOTTOM_40BIT_MASK;
    }
    else {
        unresolved_pf()
    }
}


void *alloc_heap_vpage(void) {
    void *addr = kHeapEnd;
    kHeapEnd += 4096;
    setup_page(addr);

    return addr;
}

//TODO: write other funcs 

void *alloc_stack_vpage(void) {
    //each stack is >=2MB
}

void *alloc_user_vpage(void) {
    
}

//TODO: free pages

//TODO: multi page allocation at once- how: just call the normal page alloc multiple times in a loop

//TODO: write helper funcs to get specific entry type from an address:

//PDP *get_PDP(void *addr)
//PD *get_PD(void *addr)
//etc


//TODO: free the page table!

void free_pt(PML4 *pml_entry) {
    PDP *pdp_entry;
    PD *pd_entry;
    PT *pt_entry;

    pdp_entry = (PDP *) pml_entry->base_addr;
    pd_entry = (PD *) pdp_entry->base_addr;
    pt_entry = (PT *) pt_entry->base_addr;
    
    //free yet to be implemented
    /*
    free(pt_entry);
    free(pd_entry);
    free(pdp_entry);
    free(pt_entry);
    */
}
