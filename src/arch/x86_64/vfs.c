#include "vfs.h"



Fat32 globalFat;
Fat32 *fatPtr = &globalFat;

Superblock sb;

int cluster_to_sector_offset(int cluster) {
    return 2048 + fatPtr->bpb.reserved_sectors + fatPtr->sectors_per_fat * fatPtr->bpb.num_fats + fatPtr->bpb.sectors_per_cluster * (cluster - 2);
}


void parse_bpb(uint16_t *buffer) {

    globalFat = *(Fat32 *) buffer;
    

    if (fatPtr->signature != 0x29) {
        printk("bad signature\n");
        asm("hlt");    
    }

    int root = fatPtr->root_cluster_number;


    printk("CHECK root should be 2: %d\n", root);

    //initialize root inode with inode cluster # (2) and size
}

/*
Inode * read_inode(Superblock * sblock, int inode_num) {
    Inode *ino = malloc(sizeof(Inode));
    
    
}*/

/*
//parses one set of long entries (if any) and the classic entry
//returns the start of the next entry, or NULL if that was the last entry

//fills inode pointed to by parameter with cluster (inode num), size, times, etc
void *parse_single_entry(void *start, Inode *fillThis) {

    DirEntry *dir = (DirEntry *) start;

    LongDirEntry *extended;

    //if dir entries are
    while (dir->name[0] == 0) {

    }

    if (dir->attr == 0xF) {
        extended = (LongDirEntry *) dir;

        
    }
}

int get_next_cluster_num(int cur_cluster) {

}

uint16_t * get_cluster(int cluster_num) {

}

readdir_cb(char *name, Inode *ino, void *p) {
    if (ino->num == *p) { //assuming p is target inode number?
        
    }
    
    if (ino->type == FILE) {
        //print out file name and info    
        
    }
    else if(ino->type == DIRECTORY) {
        //print out directory name and info
        
        tempList (temporary linked list)
        while(there is still a next cluster) {
            while (there is still another entry) {

                Inode new;
                parse_single_entry()

                //add inode new to tempList

                //decrement size

            }
        }

        for each inode in tempList
            call read_dir on it
        
        free all of tempList
    }
    
}

void read_dir_test() {
    LongDirEntry *extended;
    uint16_t buffer[256];
    ata_read_block(cluster_to_sector_offset(2), buffer, 256);
    DirEntry *dir = (DirEntry *) buffer;

    if (dir->attr == 0xF) {
        extended = (LongDirEntry *) buffer;
        dir = (DirEntry *) (extended + 1);
        int loop = 1;
    while(loop) {
}
    }


}
*/

//TODO:
/*

- write func that takes in a starting address and loops through until all long entries and the classic entry is read for that entry, sends back a return value indicating whether it is the last entry

*/

/*
int read_directory(Inode *ino, readdir_cb cb, void *) {
    uint16_t buffer[256];
    
    PROC_create_thread()
}*/



