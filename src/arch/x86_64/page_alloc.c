#include "page_alloc.h"
#include "vga.h"

#define ELF_TAG 9 
#define MMAP_TAG 6

void parse_tags(int tagPtr) {
    printk("parse_tags called\n");

    /*
    int loop = 1;
    while(loop) {

    }
*/
    uint64_t tagsStart = tagPtr;

    tag_header *curTag = (tag_header *) tagsStart;

    //char *iter;

    mmap_entry *entry_iter;

     printk("curTag type: %d\n", curTag->type);
    printk("curTag size: %d\n", curTag->size);

       curTag++;
     printk("curTag type: %d\n", curTag->type);
    printk("curTag size: %d\n", curTag->size);

    while (curTag->type != 0 && curTag->size != 8) {
        printk("curTag type!!: %d size: %d\n", curTag->type, curTag->size);
        
        if (curTag->type == MMAP_TAG) {

            //make iter point to start of the entries
            //pointer arithmetic intentional
            entry_iter = (mmap_entry *) (curTag + 2);
           
            while ((char *) entry_iter < ((char *) curTag + curTag->size)) {
                
                printk("entry: %d\n", entry_iter->type);
                entry_iter++;
            }
        
            
        }
        else if (curTag->type == ELF_TAG) {

        }

        curTag = (tag_header *) (((char *) curTag) + curTag->size);
    }

}
