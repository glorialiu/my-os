#include <stdint.h>

extern void parse_tags(int tagPtr);

extern void *MMU_pf_alloc(void);
extern void MMU_pf_free(void *);


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

typedef struct zone {
    int base;
    int length;
} zone;

typedef struct elf_parse {
    uint64_t num_entries:32;
    uint64_t entry_size:32;
    uint64_t section_idx:32;
} __attribute__((packed)) elf_parse;

typedef struct elf_section_header {
    uint64_t name_idx:32;
    uint64_t type: 32;
    uint64_t flags;
    uint64_t address;
    uint64_t offset;
    uint64_t size;
    uint64_t table_idx:32;
    uint64_t extra:32;
    uint64_t align;
    uint64_t fixed_entry;
} __attribute__((packed)) elf_section_header;



