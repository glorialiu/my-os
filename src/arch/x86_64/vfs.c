#include "vfs.h"
#include "memfuncs.h"
#include "vga.h"
#include "kmalloc.h"
#include "block.h"
#include "process.h"
#include "elf_loader.h"

#define UNUSED 0xE5
#define EXTENDED_ENTRY 0xF

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)
#define FAT_ATTR_DEV 0x40
#define FAT_ATTR_RESERVED 0x80

#define DIRECTORY 0x200
#define FILE 6

#define MAX_FD 10

uint64_t prog_start = 0;

Fat32 globalFat;
Fat32 *fatPtr = &globalFat;

Superblock sb;


File *fd_table[MAX_FD];
int nextFd = 0;

File *open(Inode *inode) {
    //TODO:  verify its a file
    int fd = nextFd;
    

    File *new = malloc(sizeof(File));

    fd_table[fd] = new;
    nextFd++;

    new->valid = TRUE;
    new->offset = 0;
    new->first_cluster = inode->ino_num;
    new->size = inode->length;
    
    new->close = close;
    new->read = read;
    new->lseek = lseek;
    

    return new;
}

void bytecpy(uint8_t *dst, uint8_t *src, int numBytes) {
    int i = 0;
    for (i = 0; i < numBytes; i++) {
        dst[i] = src[i];
    }
}

//what if count is an uneven number??? -___-
//assumes buffer is large enough
int read(File *file, void *buffer, int count) {

    //TODO: check if count is past the end of the file. if it is, set count to just go to end of file
    //do i really want to do this though
    
    uint8_t *dstBuffer = (uint8_t *) buffer;
    File *curFd = file; //fd_table[fd];

    int bytesRead = 0;
    uint16_t tempBuffer[256];
    int numValid;

    int curCluster;

    int offsetIntoCluster = file->offset % 512;
    int nth = file->offset / 512;


    curCluster = get_nth_cluster(file->first_cluster, nth);


    //read the first cluster
    ata_read_block(cluster_to_sector_offset(curCluster), tempBuffer, 256);

    if (count < 512) {
        bytecpy(dstBuffer, ((uint8_t *) tempBuffer) + (curFd->offset % 512), count);
        
        file->offset += count;
        return count;
    }

    bytesRead += 512;

     //while the bytesRead doesnt equal count, fill the buffer
    while (bytesRead < count) {
        //fills buffer with 512 bytes of data (full cluster worth)
        curCluster = get_next_cluster_data(curCluster, tempBuffer);

        if (curCluster == -1) {
            //bad call to read
            return NULL;
        }
        numValid = 512;

        //not all the 512 bytes may be valid, find out and update numValid accordingly
        if (bytesRead + 512 > count) {
            numValid = count - bytesRead;
        }
        //copy the valid bytes over from tempBuffer to the buffer
        bytecpy(dstBuffer + bytesRead, (uint8_t *) tempBuffer, numValid);
        
        bytesRead+=numValid;
    }


    file->offset += bytesRead;


}

//SEEK_CUR adds the offset, SEEK_SET sets the offset
//error check for past end of file?
int lseek(File *file, int offset, int mode) {
    
    if (mode == SEEK_CUR) {

        if (file->offset + offset > file->size) {
            goto error;
        }
        file->offset  = file->offset + offset;
        
    }
    else if (mode == SEEK_SET) {
        if (offset > file->size) {
            goto error;
        }
        file->offset = offset;
    }

    return file->offset;

    error:
        printk("ERROR: lseeking past end of file\n");
        return NULL;   
 }


 
//does this even work lol
int close(File **file) {
    free(*file);
}



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
    printk("sanity check- root should be 2: %d\n\n\n", root);

    //initialize root inode
    Inode *rootInode = malloc(sizeof(Inode));
    memset(rootInode, 0, sizeof(Inode));
    //rootInode->length = 
    rootInode->ino_num = fatPtr->root_cluster_number;
    rootInode->st_mode = DIRECTORY;
    rootInode->readdir = readdir_call;
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

/*
    
    if (cluster == 538972192) {
        int loop = 1;
        while(loop) {

        }
    }*/
    
    if (dir->attr & FAT_ATTR_DIRECTORY) {
        new->ino.st_mode = DIRECTORY;
    }
    else {
        new->ino.st_mode = FILE;
    }

    if (dir->attr & FAT_ATTR_HIDDEN) {
        new->ino.st_mode = new->ino.st_mode | FAT_ATTR_HIDDEN;
    }
    if (dir->attr & FAT_ATTR_SYSTEM) {
        new->ino.st_mode = new->ino.st_mode | FAT_ATTR_SYSTEM;
    }

    if (dir->attr & FAT_ATTR_VOLUME_ID) {
        new->ino.st_mode = new->ino.st_mode | FAT_ATTR_VOLUME_ID;
    }

    new->ino.time.mod_time = dir->mt;
    new->ino.time.access_date = dir->ad;
    new->ino.time.create_time = dir->ct;
    new->ino.time.create_date = dir->cd;
    
    if (!extendedEntriesPresent) {
        for (i = 0 ; i < 11; i++) {
            filename[i] = dir->name[i];
        }
        filename[11] = '\0';
    }   
    new->filename = filename;


    uint64_t diff = (uint64_t) (dir + 1) - (uint64_t) start;
    
    *curNodeSize = *curNodeSize + diff;


    if (dir->attr == 0) {
        return NULL;
    }
    
    
    return new;

    
}


int get_next_cluster_num(int curCluster) {
    
    int fat_sector = 2048 + fatPtr->bpb.reserved_sectors + ((curCluster * 4) / 512);
    int fat_offset = (curCluster * 4) % 512;

    uint16_t fat_buffer[256]; 
    ata_read_block(fat_sector, fat_buffer, 256);

    uint8_t *clusterChain = (uint8_t *) fat_buffer;

    uint32_t chain = *((uint32_t *) &clusterChain[fat_offset]) &  0x0FFFFFFF;

    if ((chain == 0) || ((chain & 0x0FFFFFFF) >= 0x0FFFFFF8)) { //TODO: more checks. "bad" sector?

        return -1;
    }

    return chain;
}

int get_nth_cluster(int start_cluster, int n) {
    int i;
    int nth = start_cluster;
    for (i = 0; i < n; i++) {
        nth = get_next_cluster_num(nth);
    }
    return nth;
}

//given the current cluster number, the function looks up the next cluster number in the fat table
//and fills the buffer with the next cluster's data
// and returns the next cluster number;
int get_next_cluster_data(int curCluster, uint16_t *buffer) {
    int fat_sector = 2048 + fatPtr->bpb.reserved_sectors + ((curCluster * 4) / 512);
    int fat_offset = (curCluster * 4) % 512;

    uint16_t fat_buffer[256]; 
    ata_read_block(fat_sector, fat_buffer, 256);

    uint8_t *clusterChain = (uint8_t *) fat_buffer;

    uint32_t chain = *((uint32_t *) &clusterChain[fat_offset]) &  0x0FFFFFFF;

    if ((chain == 0) || ((chain & 0x0FFFFFFF) >= 0x0FFFFFF8)) { //TODO: more checks. "bad" sector?

        return -1;
    }


    ata_read_block(cluster_to_sector_offset(chain), buffer, 256);

    return chain;
}


void print_inode_info(ListInode *inode, int spaces) {

    if (! (inode->ino.st_mode & FAT_ATTR_VOLUME_ID)) {
    int i = 0;
    for (i = 0; i < spaces; i ++ ) {
        printk("    ");
    }
    printk("%s   ", inode->filename);

    print_mode(inode->ino.st_mode);
    //print_time_info(inode->ino.time);

    printk("inode:%d, %d bytes",inode->ino.ino_num, inode->ino.length);

    printk("\n");

    }

}

void print_mode(mode_t mode) {
    if (mode & DIRECTORY) {
        printk(" d--------- ");
    }
    else {
        printk(" ---------- ");
    }

    /*
    if (mode & FAT_ATTR_SYSTEM) {
        printk("system");
    }*/
}

void print_time_info(struct timeval t) {

    int hour =  t.mod_time>>11;
    int minute = t.mod_time>> 5 & 0x3F;
    int seconds = t.mod_time & 0x1F;
    printk("mod time: %d:%d:%d", hour, minute, seconds);

    printk("        t.mod_time: %d", t.mod_time);
}

int readdir_call(Inode *ino, readdir_cb cb, void *p) {
    cb("hi", ino, p);
}

int recursive_readdir(char *name, Inode *ino, void *p) {
  //  if (ino->ino_num == *p) { //assuming p is target inode number?
        
  //  }

    if (ino->st_mode & FILE) {
        //printk("found file");
    }
    else if(ino->st_mode & DIRECTORY) {
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

        


        while(!noMoreClusters) { //iterates through clusters (sectors)
            while (!noMoreEntries) { //iterates within the cluster (sector)

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
                    new = parse_single_entry((void *) idxInCurBlock, &idxInCurBlock);

                    if (new) {
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
                

            }
           //returns the next cluster number and fills buffer with data;
            curCluster = get_next_cluster_data(curCluster, buffer);
            if (curCluster == -1) {
                //if get_next_cluster_data returns null, that means there is no more data
                noMoreClusters = TRUE;
            }
            else {
                //start iterating through new block. reset vars.
                idxInCurBlock = (uint64_t) buffer;
                noMoreEntries = FALSE;
                //printk("another cluster needed\n");
            }
        }

        //printk("****\n");
        while (tempListHead) {
            print_inode_info(tempListHead, p);

            if(tempListHead->filename[0] != '.' && tempListHead->ino.ino_num != 0){
                recursive_readdir(name, (Inode *) tempListHead, p + 1);
            }
            tempListHead = tempListHead->next;
        }



    /* TODO: free all of tempList*/
    }
    
    return 0;
}

void read_dir_test() {
    
    //LongDirEntry *extended;
    //uint16_t buffer[256];
    //ata_read_block(cluster_to_sector_offset(3), buffer, 256);
    //DirEntry *dir = (DirEntry *) buffer;

    //uint64_t index = (uint64_t) dir;
    //uint8_t check;
    //ListInode *temp;
    //int idx = 8;

    char *hi = "hi";

    //recursive_readdir("hi", sb.root_inode, 0);


    //printing out contents
   // sb.root_inode->readdir(sb.root_inode, recursive_readdir, 0);

    //testing read

   

    /*
    Inode fakeNode;
    fakeNode.length = 92;
    fakeNode.ino_num = 9195;
    */
    
    uint16_t test[200];
    Inode *nodey = path_readdir("/boot/a.out", sb.root_inode, NULL);

    File *executable = open(nodey);
    
    executable->lseek(executable, 0, SEEK_SET);
    executable->read(executable, (void*) test, 400);
    ELFCommonHeader *elf = (ELFCommonHeader *) test;
    ELF64Header *header = (ELF64Header *) test;

    executable->lseek(executable, header->prog_table_pos, SEEK_SET);

    uint16_t test2[50];

    executable->read(executable, (void*)test2, header->prog_ent_size);
    ELF64ProgHeader *ent = (ELF64ProgHeader *) test2;

    load_program(executable, ent);

    prog_start = header->prog_entry_pos;

    printk("****a.out loaded****\n\n");
   //PROC_create_kthread((void *) prog_start, NULL);
    PROC_create_uthread((void *) prog_start, NULL);


    /*
    char *str = (char*) test;
    printk("%s\n", str);

*/


/*

    Inode ino;
    ino.ino_num = 2;
    ino.st_mode = DIRECTORY;
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





//takes in a path and returns the index of the next slash (not including the first one)
int next_slash(char *path) {
    int length = strlen(path);
    
    int idx = 1;
    int found = FALSE;

    while (idx < length) {
        if (path[idx] == '/') {
            return idx;
        }
        idx++;
    }

    return -1;
}

Inode * path_readdir(char *name, Inode *ino, void *p) {

    int nextSlashIndex = next_slash(name);


    //if nextSlashIndex == -1, this is the file, return the inode
    //next name will be name + nextSlashIndex

  //  if (ino->ino_num == *p) { //assuming p is target inode number?
        
  //  }

    if (ino->st_mode & FILE) {
        
    }
    else if(ino->st_mode & DIRECTORY) {
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

        while(!noMoreClusters) { //iterates through clusters (sectors)
            while (!noMoreEntries) { //iterates within the cluster (sector)

                byteCheck = *(uint8_t *) idxInCurBlock;

                if (byteCheck == 0) {
                    noMoreEntries = TRUE;
                    noMoreClusters = TRUE;
                }
                else if (byteCheck == 0xE5) {
                    idxInCurBlock = (uint64_t) (((DirEntry *) idxInCurBlock) + 1);
                }
                else {
                    new = parse_single_entry((void *) idxInCurBlock, &idxInCurBlock);



    

                    if (new && new->filename[0] != '.') {

                        if (nextSlashIndex == -1 && strncmp(new->filename, name + 1, strlen(name) - 1) == 0) {
                            return (Inode *)new;
                        }
                        else if (nextSlashIndex != -1 && strncmp(new->filename, name + 1, nextSlashIndex - 1) == 0) {
                            return path_readdir(name + nextSlashIndex, (Inode *) new, NULL);
                        }
                    }
                }
            }
           //returns the next cluster number and fills buffer with data;
            curCluster = get_next_cluster_data(curCluster, buffer);
            if (curCluster == -1) {
                //if get_next_cluster_data returns null, that means there is no more data
                noMoreClusters = TRUE;
            }
            else {
                //start iterating through new block. reset vars.
                idxInCurBlock = (uint64_t) buffer;
                noMoreEntries = FALSE;
                //printk("another cluster needed\n");
            }
        }

    }
    
    return 0;
}



