#include "types.h"
#include <stdint.h>

typedef struct HDProcessQueue {
    struct Process *head;
    struct DriveCmd *commandHead;
} HDProcessQueue;

typedef struct ATACmd {
    void *dst;
    uint64_t lba;
    uint64_t len;
    int complete;
    struct ProcessQueue *queue;
    struct ATACmd *next;
} ATACmd;

typedef struct Bus {
    uint16_t base;
    uint16_t control;
} Bus;


enum BlockDevType { MASS_STORAGE, PARTITION };

typedef struct BlockDev {
    uint64_t totalLen; // this is the total length of block device, found during initialization phase
                        //uint16_t 100-103
    int (*read_block)(struct BlockDev *dev, uint64_t blk_num, void *dst);
    uint32_t blkSize; // will be 512 (confirm this?)
    enum BlockDevType type; //partition or entire disk
    char *name; //give the block device a name
    uint8_t fsType; 
    struct BlockDev *next;
} BlockDev;


typedef struct ATABlockDev {
    struct BlockDev dev; //inheritance
    uint16_t ataBase; // base port for ata
    uint16_t ataMaster; 
    uint8_t slave; //indicates if block device is using master or slave
    uint8_t irq; //irq number that the block device is using
    struct ATARequest *requestHead;
    struct ATARequest *requestTail;
} ATABlockDev;

//worry about this later but:
struct PartitionBlockDev {
    struct ATABlockDev dev;
    uint64_t offset;
    uint64_t length;
};

struct ATABlockDev *ata_probe(uint16_t base, uint16_t master, uint8_t slave, char *name, uint8_t irq);

int BLK_register(BlockDev *dev);
int read_block(BlockDev *dev, uint64_t blk_num, void *dest);
void ata_init();
void ata_identify(ATABlockDev *device);
void ata_soft_reset(Bus *dev);
void ata_io_wait(uint16_t);
int detect_devtype (int slavebit, Bus *dev);

extern void ata_isr();

extern void init_ata_read_queue();
void ata_read_block(uint64_t lba, void *dst, uint64_t len);
void dequeue_command();
void enqueue_command(ATACmd *cmd);

extern void block_test_thread(int);

void printType(int type);

extern void read_mbr_block();
extern void read_bpb_block();

extern void poll_read_block(int block_num, uint16_t buffer);
