#include <stdint.h>
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

typedef struct timeval {
    uint64_t mod_time;
    uint64_t access_time;
    uint64_t create_time;
};
/*
typedef int (*readdir_cb)(const char *, Inode *, void *);

typedef struct Inode {
    size_t length;
    struct timeval time;
    mode_t st_mode;
    uid_t uid;
    gid_t gid;
    uint64_t ino_num;

    //file operations
    //File * (*open) (Inode *);
    
    //directory operations
    int (*readdir)(Inode *inode, readdir_cb cb, void *p);
    int (*unlink) (Inode *, const char *name); //delete a file from directory

    void (*free) (Inode **);
    
} Inode;

typedef struct Superblock {

};*/
extern void parse_bpb(uint16_t *buffer);