
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

                    if (new) {

                        if (nextSlashIndex == -1 && strncmp(new->filename, name + 1, strlen(name) - 1) == 0) {
                            return new;
                        }
                        else if (strncmp(new->filename, name + 1, nextSlashIndex - 1) == 0) {
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
