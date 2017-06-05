#include "vfs.h"
#include "memfuncs.h"
#include "vga.h"
#include "kmalloc.h"
#include "block.h"
#include "process.h"

#define UNUSED 0xE5
#define EXTENDED_ENTRY 0xF

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

#define DIRECTORY 5
#define FILE 6
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


    //sanity check
    int root = fatPtr->root_cluster_number;
    printk("CHECK root should be 2: %d\n", root);

    //initialize root inode
    Inode *rootInode = malloc(sizeof(Inode));
    //rootInode->length = 
    rootInode->ino_num = fatPtr->root_cluster_number;
    
    sb.root_inode = rootInode;
}

/*
Inode * read_inode(Superblock * sblock, int inode_num) {
    Inode *ino = malloc(sizeof(Inode));
    
    
}*/


//parses one set of long entries (if any) and the classic entry
//returns the start of the next entry, or NULL if that was the last entry

//fills inode pointed to by parameter with cluster (inode num), size, times, etc

//only call on what you know will be a listing, perform ==0 (no more entries) and == 0xE5 (skip this) before starting
ListInode *parse_single_entry(void *start, uint64_t *curNodeSize) {
    ListInode *new = malloc(sizeof(ListInode));

    memset(new, 0, sizeof(ListInode));
    
    DirEntry *dir = (DirEntry *) start;
    LongDirEntry *extended;

    char *filename = malloc(40);

    memset(filename, '\0', 40);

    int offset;
    int i;
    int idx;
    
    

    int noMoreExtended = FALSE;
    int extendedEntriesPresent = FALSE;

    while(!noMoreExtended) {
        if (dir->name[0] != UNUSED) {
            if (dir->attr == EXTENDED_ENTRY) {
                extendedEntriesPresent = TRUE;

                extended = (LongDirEntry *) dir;

                //copy files
                offset = ((extended->order & 0x3F) - 1) * 13;
                idx = offset;
                for (i = 0; i < 5; i++) {

                    if (extended->first[i] != 0xFFFF) {
                        filename[idx] = (char) extended->first[i];   
                        idx++;
                    }

                }
                for (i = 0; i < 6; i++) {
                    if (extended->middle[i] != 0xFFFF) {
                        filename[idx] = (char) extended->middle[i];   
                        idx++;
                    }
                }
                for (i = 0; i < 2; i++) {
                    if (extended->last[i] != 0xFFFF) {
                        filename[idx] = (char) extended->last[i];   
                        idx++;
                    }
                }
                

                if (extended->order & 0x3F == 0x40 ) {
                    //this is the last 
                    noMoreExtended = TRUE;
                }

                dir = (DirEntry *) (extended + 1);
            }
            else {
                //jump out of the loop because its not an extended entry
                //do we need to check anything else?
                noMoreExtended = TRUE;
            }
        }
        else {
            //the entry is unused. increment to next one
            dir++;
        }
    }

    //dir is pointing to a classic entry supposedly

    int cluster = dir->cluster_hi << 16 | dir->cluster_lo & 0xFFFF;
    int size = dir->size;
    new->ino.length = size;
    new->ino.ino_num = cluster;
    
    if (dir->attr & FAT_ATTR_DIRECTORY) {
        new->ino.st_mode = DIRECTORY;
    }
    else {
        new->ino.st_mode = FILE;
    }
    //fillThis->ino.time = 
    
    if (!extendedEntriesPresent) {
        for (i = 0 ; i < 11; i++) {
            filename[i] = dir->name[i];
        }
        filename[11] = '\0';
    }   
    new->filename = filename;


    uint64_t diff = (uint64_t) (dir + 1) - (uint64_t) start;
    
    *curNodeSize = *curNodeSize + diff;
    
    
    return new;

    
}

int get_next_cluster_num(int cur_cluster) {
    return -1;
}

uint16_t * get_cluster(int cluster_num) {

}


void print_inode_info(ListInode *inode) {
    printk("name: %s    inode #: %d\n", inode->filename, inode->ino.ino_num);
}

int readdir_cb1(char *name, Inode *ino, void *p) {
  //  if (ino->ino_num == *p) { //assuming p is target inode number?
        
  //  }

    if (ino->st_mode == FILE) {
        //print out file name and info
    }
    else if(ino->st_mode == DIRECTORY) {
        //print out directory name and info
        //read in actually directory listings

        uint16_t buffer[256];
        ata_read_block(cluster_to_sector_offset(ino->ino_num), buffer, 256);

        uint64_t idxInCurBlock = (uint64_t) buffer;
        uint8_t byteCheck;

        int noMoreEntries = FALSE;
        int noMoreClusters = FALSE;
        int nextCluster, curCluster;

        ListInode *new;
        ListInode *tempListHead = NULL; // (temporary linked list)
        ListInode *iter;


        while(!noMoreClusters) {
            noMoreEntries = FALSE;
            while (!noMoreEntries) {

                byteCheck = *(uint8_t *) idxInCurBlock;

                if (byteCheck == 0) {
                    //printk("no more entries");
                    noMoreEntries = TRUE;
                    noMoreClusters = TRUE;
                }
                else if (byteCheck == 0xE5) {
                    printk("skip this entry");
                    idxInCurBlock = (uint64_t) (((DirEntry *) idxInCurBlock) + 1);
                }
                else {
                    new = parse_single_entry(idxInCurBlock, &idxInCurBlock);
                    new->next = NULL;
                    //add new inode new to tempList
                    
                    if (!tempListHead) {
                        tempListHead = new;
                    }
                    else {
                        iter = tempListHead;
                        while(iter->next) {
                            iter = iter->next;
                        }
                        iter->next = new;
                    }

                }
                

            }
            nextCluster = get_next_cluster_num(curCluster);
            if (nextCluster == -1) {
                noMoreClusters = TRUE;
            }
        }

        printk("****\n");
        while (tempListHead) {
            print_inode_info(tempListHead);
            

            if(tempListHead->filename[0] != '.' && tempListHead->ino.ino_num != 0){
                readdir_cb1(name, (Inode *) tempListHead, NULL);
            }
            
            
            tempListHead = tempListHead->next;
        }



    /*
        for each inode in tempList
            call read_dir on it        
        free all of tempList*/
    }
    
}

void read_dir_test() {
    LongDirEntry *extended;
    uint16_t buffer[256];
    ata_read_block(cluster_to_sector_offset(3), buffer, 256);
    DirEntry *dir = (DirEntry *) buffer;

    uint64_t index = (uint64_t) dir;
    uint8_t check;
    ListInode *temp;
    int idx = 8;


    Inode ino;
    ino.ino_num = 2;
    ino.st_mode = DIRECTORY;

    char *hi = "hi";



    readdir_cb1(hi, &ino, NULL);
    

/*

    while(idx) {

        //only call on what you know will be a listing, perform ==0 (no more entries) and == 0xE5 (skip this) before starting


        check = *(uint8_t *) index;

        if (check == 0) {
            printk("no more entries");
        }
        else if (check == 0xE5) {
            printk("skip this entry");
            index = (uint64_t) (((DirEntry *) index) + 1);
        }
        else {
            temp = parse_single_entry(index, &index);
            

            
        }
        
        
        
        
        printk("name: %s\n", temp->filename);
        idx--;
    }
*/


    /*
    if (dir->attr == 0xF) {
        extended = (LongDirEntry *) buffer;
        dir = (DirEntry *) (extended + 1);

        //dir = dir + 1;

    }*/

    kexit();
}


//TODO:
/*

- write func that takes in a starting address and loops through until all long entries and the classic entry is read for that entry, sends back a return value indicating whether it is the last entry

*/

/*
int read_directory(Inode *ino, readdir_cb cb, void *) {
    uint16_t buffer[256];
    
    PROC_create_thread()
}*/



