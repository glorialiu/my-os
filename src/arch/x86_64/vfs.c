#include "vfs.h"



Fat32 globalFat;
Fat32 *fatPtr = &globalFat;


int cluster_to_sector_offset(int cluster) {
    return fatPtr->bpb.reserved_sectors + fatPtr->sectors_per_fat * fatPtr->bpb.num_fats + fatPtr->bpb.sectors_per_cluster * (cluster - 2);
}




void parse_bpb(uint16_t *buffer) {

    globalFat = *(Fat32 *) buffer;
    

    if (fatPtr->signature != 0x29) {
        printk("bad signature\n");
        asm("hlt");    
    }

    int root = fatPtr->root_cluster_number;


    printk("root should be 2: %d\n", root);
}
