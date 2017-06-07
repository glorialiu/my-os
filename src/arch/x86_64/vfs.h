#include <stdint.h>
#include "types.h"

extern void parse_bpb(uint16_t *buffer);

typedef struct FatBPB {
    uint8_t jmp[3];
    char oem_id[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t num_dirents;
    uint16_t tot_sectors;
    uint8_t mdt;
    uint16_t num_sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t num_hidden_sectors;
    uint32_t large_sector_count;
} __attribute__((packed)) FatBPB;


typedef struct Fat32 {
    struct FatBPB bpb;
    uint32_t sectors_per_fat;
    uint16_t flags;
    uint8_t major_vers;
    uint8_t minor_vers;
    uint32_t root_cluster_number;
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint8_t zero[12];
    uint8_t drive_num;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t serial_num;
    char label[11];
    char sys_id[8];
    uint8_t boot_code[420];
    uint8_t boot_sig[2];
} __attribute__((packed)) Fat32;

struct timeval {
    uint16_t mod_time;
    uint16_t access_date;
    uint16_t create_time;
    uint16_t create_date;
};


typedef struct File {
    /* "META DATA" */
    int first_cluster;
    int offset;
    int valid;
    int size;
    
    int (*close) (struct File **file);
    int (*read) (struct File *file, char *dst, int len);
    int (*lseek) (struct File *file, uint64_t offset, int mode);
} File;

typedef struct Inode Inode;

typedef int (*readdir_cb)(const char *, Inode *, void *);



typedef struct Inode {
    size_t length;
    struct timeval time;
    mode_t st_mode;
    uid_t uid;
    gid_t gid;
    uint64_t ino_num;

    //file operations
    File * (*open) (Inode *);
    
    //directory operations
    
    int (*readdir)(Inode *inode, readdir_cb cb, void *p);
    int (*unlink) (Inode *, const char *name); //delete a file from directory

    void (*free) (Inode **);
    
} Inode;

typedef struct Superblock {

    Inode *root_inode;
    //Inode *(*read_inode) (Superblock *, int inode_num);
    char *name;
    char *type;

} Superblock;

typedef struct DirEntry {
    uint8_t name[11];
    uint8_t attr;
    uint8_t nt;
    uint8_t ct_tenths;
    uint16_t ct;
    uint16_t cd;
    uint16_t ad;
    uint16_t cluster_hi;
    uint16_t mt;
    uint16_t md;
    uint16_t cluster_lo;
    uint32_t size;
} __attribute__((packed)) DirEntry;


typedef struct LongDirEntry {
    uint8_t order;
    uint16_t first[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t middle[6];
    uint16_t zero;
    uint16_t last[2];
} __attribute__((packed)) LongDirEntry;

extern void read_dir_test();


typedef struct ListInode {

    struct Inode ino;
    /*
    size_t length;
    struct timeval time;
    mode_t st_mode;
    uid_t uid;
    gid_t gid;
    uint64_t ino_num;
    //and other funcs
    */
    struct ListInode *next;
    char *filename;
} ListInode;



void print_mode(mode_t mode);
int recursive_readdir(char *name, Inode *ino, void *p);
int get_next_cluster_data(int curCluster, uint16_t *buffer);
ListInode *parse_single_entry(void *start, uint64_t *curNodeSize);


int readdir_call(Inode *ino, readdir_cb cb, void *p);

File *open(Inode *inode);
void bytecpy(uint8_t *dst, uint8_t *src, int numBytes);
int read(File *file, void *buffer, int count);
int lseek(File *file, int offset, int mode);
int close(File **file);


int get_nth_cluster(int start_cluster, int n);
int get_next_cluster_num(int curCluster);

Inode * path_readdir(char *name, Inode *ino, void *p);

