#include "elf_loader.h"
#include "inline_asm.h"


void load_program(File *file, ELF64ProgHeader *program) {


    //allocate user page(s)
    //make buffer point to them

    uint64_t cr3 = read_cr3();


    uint8_t *buffer = alloc_user_vpage(cr3);


    file->lseek(file, program->file_offset, SEEK_SET);
    
    file->read(file, (void *) buffer, program->file_size);

   
}
