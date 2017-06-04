#include "mbr.h"
#include "vga.h"

#define PARTITION1 446
#define PARTITION2 462


void parse_mbr(uint16_t *buffer) {
    
    PartitionEntry *ptr = (PartitionEntry *) (((uint8_t *) buffer) + PARTITION1);

    printk("\n***Partition Entry 1***\nstatus: %d\nfirst_head: %d\n", ptr->status, ptr->first_head);

    printk("1st sector cylinder: %d\n", ptr->first_cyl_hi << 8 | ptr->first_cyl_lo);
    printk("type: %d\nlast sector head: %d\n", ptr->type, ptr->last_head);
    printk("last sector cylinder: %d\n", ptr->last_cyl_hi << 8 | ptr->last_cyl_lo);
    printk("first lba: %d\nnumber of sectors: %d\n", ptr->first_lba, ptr->num_sectors);

    printk("\n\n");
   
}
