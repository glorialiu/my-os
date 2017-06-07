#include "elf_loader.h"



void load_program(File *file, ELF64ProgHeader *program) {


    //allocate user page(s)
    //make buffer point to them


    uint8_t *buffer = alloc_user_vpage(0);


    file->lseek(file, program->file_offset, SEEK_SET);
    
    file->read(file, (void *) buffer, program->file_size);

    
}
