#include <stdint.h>

void parse_mbr(uint16_t *);

typedef struct PartitionEntry {
    uint64_t status:8;
    uint64_t first_head:8;
    uint64_t first_cyl_hi:8;
    uint64_t first_cyl_lo: 8;
    uint64_t type:8;
    uint64_t last_head:8;
    uint64_t last_cyl_hi:8;
    uint64_t last_cyl_lo:8;
    uint64_t first_lba:32;
    uint64_t num_sectors:32;
    
} __attribute__((packed)) PartitionEntry;

