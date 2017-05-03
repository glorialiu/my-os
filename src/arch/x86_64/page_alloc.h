#include <stdint.h>
extern void parse_tags(int tagPtr);

typedef struct tag_header {
    uint64_t type:32;
    uint64_t size:32;

}__attribute__((packed)) tag_header;

typedef struct mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint64_t type:32;
    uint64_t reserved:32;

}__attribute__((packed)) mmap_entry;
