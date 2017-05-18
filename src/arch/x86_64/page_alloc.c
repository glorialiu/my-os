#include "page_alloc.h"
#include "vga.h"

#define ELF_TAG 9 
#define MMAP_TAG 6

#define NUM_ZONES 6

#define END_TYPE 0
#define END_SIZE 8

static int nextFreePage = 0;

uint64_t head = 0;

static int highMemStart = 0;
static int highMemEnd = 20;
static int lowMemStart = 0;
static int lowMemEnd = 0;

int alignBy8(int num) {

    if (num % 8 == 0) {
        return num;
    }
    
    return num + (8 - (num % 8));

}

int alignBy4096(int num) {

    if (num % 4096 == 0) {
        return num;
    }
    
    return num + (4096 - (num % 4096));

}


void parse_tags(int tagPtr) {

    zone zones[NUM_ZONES];

    int kMinAddress = 0;
    int kMaxAddress = 0;

    /*
    int loop = 1;
    while(loop) {

    }
*/
    uint64_t tagsStart = tagPtr;
    tag_header *curTag = (tag_header *) tagsStart;


    mmap_entry *entry_iter;
    elf_parse *elf_parser;
    elf_section_header *section_iter;

    int totalSize = curTag->type;
    printk("tag size: %d\n", totalSize);

    curTag++;

    int zIdx = 0;

    while (curTag->type != END_TYPE && curTag->size != END_SIZE) {
        //printk("curTag type!!: %d size: %d\n", curTag->type, curTag->size);
        
        if (curTag->type == MMAP_TAG) {


            //make iter point to start of the entries
            //pointer arithmetic intentional
            entry_iter = (mmap_entry *) (curTag + 2);
           
            while ((char *) entry_iter < ((char *) curTag + curTag->size)) {
                
               printk("type: %d, base_addr: %d, length: %d\n", entry_iter->type, (int)entry_iter->base_addr, (int) entry_iter->length);

                if (entry_iter->type == 1) {
                    zones[zIdx].base = (int) entry_iter->base_addr;
                    zones[zIdx++].length = (int) entry_iter->length;
                }
                entry_iter++;
            }
        }
        else if (curTag->type == ELF_TAG) {
            

            elf_parser = (elf_parse *) curTag + 1; // pointer arithmetic intentional

            tag_header *temp = curTag + 1;

            //skip over four bytes for the string table index section
            section_iter = (elf_section_header *) (((char *) temp) + 4);

            int idx = 0;
            int numEntries = temp->type; //this isn't actually type.
                                        //using tag_header as a byte overlay. fix this at some point.

            for (idx = 0; idx < numEntries; idx++) {

                printk("section header type: %x, load address: %d, size: %d\n", section_iter->type, section_iter->address, section_iter->size);


                if (kMinAddress > section_iter->address) {
                    kMinAddress = section_iter->address;
                }
                if (kMaxAddress < section_iter->address + section_iter->size) {
                    kMaxAddress = section_iter->address + section_iter->size;
                }
                section_iter++;
            }

            printk("min: %d, max: %d\n", kMinAddress, kMaxAddress);
            
        }




        curTag = (tag_header *) (((char *) curTag) + alignBy8(curTag->size));
    }

    int i = 0;
    for (i = 0; i < zIdx; i++) {
        printk("base: %d, length: %d\n", zones[i].base, zones[i].length);
    }
    
    lowMemStart = zones[0].base;
    lowMemEnd = zones[0].base + zones[0].length;
    highMemStart = zones[1].base + kMaxAddress;
    highMemEnd = zones[1].base + zones[1].length;

    printk("lowMemStart: %d, lowMemEnd: %d, highMemStart: %d. highMemEnd: %d\n", lowMemStart, lowMemEnd, highMemStart, highMemEnd);
   
    nextFreePage = 0;//alignBy4096(kMaxAddress);
    
    printk("nextFreePage: %d\n", nextFreePage);
    printk("parsing tags finished.\n");


}



void *MMU_pf_alloc(void) {
    //page size is 4096
    //printk("mmu pf alloc called\n");

    //printk("high mem end: %d\n", highMemEnd);

    //printk("nextFreePage in pf alloc: %d\n", nextFreePage);

    uint64_t allocatedPage;
    uint64_t temp;

    allocatedPage = nextFreePage;
    nextFreePage = nextFreePage + 4096;

    if (nextFreePage > lowMemEnd && nextFreePage < highMemStart) {
        nextFreePage = alignBy4096(highMemStart);
        //printk("skipping devices section\n");
    }

    if (allocatedPage < highMemEnd) {
        //printk("high mem end: %d\n", highMemEnd);
        printk("allocated page is %d\n", (int) allocatedPage);
        return (void *) allocatedPage;
    }    
    else {
        // get a page from the free list
        if (head == 0) {
            // you're all out of pages.. yikes
            printk("you're all out of pages.. yikes\n");
            return 0;
        }
        else {


            temp = head;
            head = *(uint64_t *) head;
            //printk("allocated a page..\n");
            return (void *) temp;  

        }
    }
    
  
}

void MMU_pf_free(void * page) {
    printk("free called\n");
    uint64_t newAddress = (uint64_t) page;
    
    uint64_t *ptr = (uint64_t *) head;

    if (head == 0) { //if head is null essentially
        head = (uint64_t) page;
        *(uint64_t *) head = 0;
        printk("head is %d\n", head);
    }
    else {
        printk("head is not null\n");
        while (*ptr != 0) {
            ptr = (uint64_t *) *ptr;
        }
        *ptr = newAddress;
        printk("end of current free list: %d\n", (int) ptr);
        *(uint64_t *) newAddress = 0;

    }


}

