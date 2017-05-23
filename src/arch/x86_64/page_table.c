#include "page_table.h"
#include "page_alloc.h"
#include "memfuncs.h"
#include "inline_asm.h"
#include "vga.h"

#define NUM_PML4_ENTRIES 1
#define NUM_PDP_ENTRIES NUM_PML4_ENTRIES * 512
#define NUM_PD_ENTRIES NUM_PDP_ENTRIES * 512
#define NUM_PT_ENTRIES NUM_PD_ENTRIES * 512

#define NUM_PAGES_IDENTITY_MAPPED 5120

#define BOTTOM_9BIT_MASK 511
#define BOTTOM_12BIT_MASK 0xFFF
#define BOTTOM_40BIT_MASK 0x3FFFFFFFFFF

#define ENTRY_SIZE 8

#define PAGE_SIZE 4096
#define NUM_STACK_PAGES 512


static void *physicalPFStart = (void *)  0x0;

static void *kStackEnd = (void *) 0x8200000;

static void *kHeapEnd = (void *)  0x400000000;

//0x8000000;
//0x400000000;

static void *userSpaceEnd = (void *) 0x800000000;

page_table *pt_temp;

// my implementation of kSbrk
// returns the current end of kHeap if numBytes param == 0
// otherwise, returns the new end of kHeap
uint64_t ksbrk(int numBytes) {
    int numPages;  

    if (numBytes != 0) {
        // add one just in case of decimal
        numPages = numBytes / PAGE_SIZE + 1; 
        page_table *cr3 = (page_table *) read_cr3();
        alloc_heap_vpages(cr3, numPages);
    }

    return (uint64_t) kHeapEnd;
}

void *ptable_init(page_table *pt_start) {
    /*
    int loop = 1;
    while (loop) {

    }*/


    pt_start = (PML4 *) MMU_pf_alloc();
    PDP *id_pdp = (PDP *) MMU_pf_alloc();
    PD *id_pd = (PD *) MMU_pf_alloc();

    pt_temp = pt_start;

    memset(pt_start, 0, ENTRY_SIZE * 512);
    memset(id_pdp, 0, ENTRY_SIZE * 512);
    memset(id_pd, 0 , ENTRY_SIZE * 512);
 
    // setting first PML4 block
    pt_start->p = 1;
    pt_start->rw = 1;
    pt_start->us = 1;
    pt_start->base_address = (uint64_t) id_pdp >> 12;

    // setting first PDP block
    id_pdp->p = 1;
    id_pdp->rw = 1;
    id_pdp->us = 1;
    id_pdp->base_address = (uint64_t) id_pd >> 12;

    // set entire page of PD blocks
    int i;
    int j;
    uint64_t *temp_pt;
    int id_map = 0;

    for (i = 0; i < 63; i++) {
        id_pd->p = 1;
        id_pd->rw = 1;
        id_pd->us = 1;

        //make each block "point" to a new page
        id_pd->base_address = (uint64_t) MMU_pf_alloc() >> 12;

        //iterate through page of pt entries
        temp_pt =(uint64_t *) (id_pd->base_address << 12);

        for (j = 0; j < 512; j++) {
            
            *temp_pt = id_map | 7;
            id_map += PAGE_SIZE;
            temp_pt++;
        }
        id_pd++;
    }
    



    uint64_t ptr = (uint64_t) pt_start;

    printk("about to load cr3\n");
    load_cr3(ptr);
    printk("loaded cr3 successfully\n");
    return pt_start;
}



void setup_pages(void *addr, int numPages, page_table *pt) {
    int i = 0;
    uint64_t address = (uint64_t) addr;
    
    for (i = 0; i < numPages; i++) {
        setup_page((void*) address, pt);
        address += PAGE_SIZE;
    }
}

void setup_page(void *addr, page_table *pt) {
    uint64_t address = (uint64_t) addr;

    int pmlIdx = (address >> 39 & BOTTOM_9BIT_MASK);
    int pdpIdx = (address >> 30 & BOTTOM_9BIT_MASK);
    int pdIdx = (address >> 21 & BOTTOM_9BIT_MASK);
    int ptIdx = (address >> 12 & BOTTOM_9BIT_MASK);

    PML4 *pml_entry;
    PDP *pdp_entry;
    PD *pd_entry;
    PT *pt_entry;
    
    /*
    int loop = 1;
    while (loop) {
    }*/

    void *temp;

    pml_entry = get_PML4(pmlIdx, pt);
    if (pml_entry->p != 1) {
        pml_entry->p = 1;
        pml_entry->rw = 1;
        pml_entry->us = 1;
            
        temp =  MMU_pf_alloc();
        memset(temp, 0, ENTRY_SIZE * 512);
        pml_entry->base_address = (uint64_t) temp >> 12;
    }

    pdp_entry = get_PDP(pml_entry, pdpIdx);
    if (pdp_entry-> p != 1) {
        pdp_entry->p = 1;
        pdp_entry->rw = 1;
        pdp_entry->us = 1;
        pdp_entry->zero = 0; //this shouldnt be necessary

        temp =  MMU_pf_alloc();
        memset(temp, 0, ENTRY_SIZE * 512);
        pdp_entry->base_address = (uint64_t) temp >> 12;        
    }

    pd_entry = get_PD(pdp_entry, pdIdx);
    if (pd_entry-> p != 1) {
        pd_entry->p = 1;
        pd_entry->rw = 1;
        pd_entry->us = 1;
        pd_entry->zero = 0;

        temp =  MMU_pf_alloc();
        memset(temp, 0, ENTRY_SIZE * 512);

        pd_entry->base_address = (uint64_t) temp >> 12;        
    }

    pt_entry = get_PT(pd_entry, ptIdx);
    //set up the page table entry
    pt_entry->rw = 1;
    pt_entry->us = 1;

    //temporarily not doing demand paging for debugging purposes;
    pt_entry->demand = 1;
    //pt_entry->p = 1;
    //pt_entry->base_address = (uint64_t) MMU_pf_alloc() >> 12; 


}

PML4 *get_PML4(int offset, page_table *pt_start) {
    return pt_start + offset;
}

PDP *get_PDP(PML4 *pml4_entry, int offset) {
    PDP *temp = (PDP *) (pml4_entry->base_address << 12);
    
    return temp + offset;
}

PD *get_PD(PDP *pdp_entry, int offset) {
    PD *temp = (PD *) (pdp_entry->base_address << 12);
    
    return temp + offset;
}

PT *get_PT(PD *pd_entry, int offset) {
    PT *temp = (PT *) (pd_entry->base_address << 12);
    
    return temp + offset;
}

//walk the page table to return the correct page table entry for the address

//also check if valid
PT *return_pt_entry(void *addr, page_table *pt) {
    uint64_t address = (uint64_t) addr;

    int pmlIdx = (address >> 39 & BOTTOM_9BIT_MASK);
    int pdpIdx = (address >> 30 & BOTTOM_9BIT_MASK);
    int pdIdx = (address >> 21 & BOTTOM_9BIT_MASK);
    int ptIdx = (address >> 12 & BOTTOM_9BIT_MASK);
    int pageOffset = (address & BOTTOM_12BIT_MASK);


    PML4 *pml_entry;
    PDP *pdp_entry;
    PD *pd_entry;
    PT *pt_entry;

    pml_entry = get_PML4(pmlIdx, pt);
    if (pml_entry->p != 1) {
        unresolved_pf();
    }

    pdp_entry = get_PDP(pml_entry, pdpIdx);
    if (pdp_entry->p != 1) {
        unresolved_pf();       
    }

    pd_entry = get_PD(pdp_entry, pdIdx);
    if (pd_entry->p != 1) {
        unresolved_pf();    
    }

    pt_entry = get_PT(pd_entry, ptIdx);

    //do we need to check r/w?
    //eventually do something with user mode?
    return pt_entry;   

}

void unresolved_pf() {
    uint64_t faultCause = read_cr2();
    /*
    int loop = 1;
    while(loop) {
    }
    */
    printk("PAGE FAULT (UNRESOLVED): %x\n", faultCause);
    asm volatile("hlt");
}

void page_fault_handler(int num, int error, void *arg) {
    uint64_t addr = read_cr2();
  
    /*
    int loop = 1;
    while(loop) {
    }
    */



    printk("error code %d\n", error);
    printk("%x caused page fault\n", addr);
    //walk page table to return pt entry
    //return_pt_entry will check the pt entry for errors in page table levels internally
    uint64_t cr3 = read_cr3();

    PT *entry = return_pt_entry((void *) addr, cr3); 
    /*
    int loop = 1;
    while(loop) {
    }*/

    if (entry->demand == 1) {
        printk("allocating on demand\n");
        entry->demand = 0;
        entry->p = 1;
        entry->base_address = (uint64_t) MMU_pf_alloc() >> 12; 
    }
    else {
        unresolved_pf();
    }
}


void *alloc_heap_vpage(page_table *pt) {
    void *addr = kHeapEnd;
    kHeapEnd += PAGE_SIZE;
    setup_page(addr, pt);

    return addr;
}

void *alloc_heap_vpages(page_table *pt, int numPages) {
    void *addr = kHeapEnd;
    kHeapEnd += PAGE_SIZE * numPages;
    setup_pages(addr, numPages, pt);

    return addr;
}


void *alloc_stack_vpage(page_table *pt) {
    //each stack is >=2MB

    void *addr = kStackEnd;
    kStackEnd += NUM_STACK_PAGES * PAGE_SIZE;
    setup_pages(addr, NUM_STACK_PAGES, pt);

    return addr;
}

void *alloc_user_vpage(page_table *pt) {
    void *addr = userSpaceEnd;
    userSpaceEnd += PAGE_SIZE;
    setup_page(addr, pt);

    return addr;   
}


void MMU_free_page(void *addr, page_table *pt) {
    PT *pt_entry = return_pt_entry(addr, pt);

    pt_entry->p = 0;
    pt_entry->demand = 0;
}

void MMU_free_pages(void *addr, int num, page_table *pt) {
    uint64_t address = (uint64_t) addr;
    int i = 0;

    for (i=0 ;i < num; i++) {
        MMU_free_page((void *)address, pt);
        address += PAGE_SIZE;
    }
    
}

/*
//TODO: free the page table!
void free_pt(PML4 *pml_entry) {
    PDP *pdp_entry;
    PD *pd_entry;
    PT *pt_entry;

    //TODO: bottom 12 bits something??

    pdp_entry = (PDP *) (uint64_t) pml_entry->base_address << 12;
    pd_entry = (PD *) (uint64_t) pdp_entry->base_address << 12;
    pt_entry = (PT *) (uint64_t) pt_entry->base_address << 12;
    
    //free yet to be implemented
    
    free(pt_entry);
    free(pd_entry);
    free(pdp_entry);
    free(pt_entry);
    
}
*/
