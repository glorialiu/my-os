#include "vga.h"
#include "ps2.h"
#include "interrupt.h"
#include "serial.h"
#include "memfuncs.h"
#include "page_alloc.h"
#include "page_table.h"
#include "process.h"
#include "kmalloc.h"
#include "block.h"
/*
void printk_tests() {
    printk("******TEST SCRIPT FOR PRINTK******\n");

    //printk("\nCHARACTER TEST: %%c\n");

    printk("\nUNSIGNED: %%u\n");
    
    printk("expected: 5 - actual: %u\t\texpected: 12345678 - actual: %u\n", 5, 12345678);
    printk("expected: 0 - actual: %u\t\texpected: 4292967295 - actual: %u\n", 0, 4292967295);


    printk("\nSTRING TEST: %%s\n");
    printk("this is a string!\n");
    printk("it works!\n");

    printk("\nHEXADECIMAL TEST: %%x\n");
    printk("expected: 9D - actual: %x\t\texpected: 56ED2 - actual: %x\n", 157, 0x56ED2);

    printk("\nINTEGER TEST: %%d\n");
    printk("expected: 2017 - actual: %d\t\texpected: 2147483647 - actual: %d\n", 2017, 2147483647);

    printk("\nPOINTER TEST: %%p\n");
    printk("expected: FFFFFFFF - actual: %p\n", 0xFFFFFFFF);


    printk("\n%%h prefix test\n");

    printk("%hd ", 3200);
    printk("%hu ", 3200);
    printk("%hx ", 3200);

    printk("\n%%l prefix test\n");

    printk("%ld ", 655266);
    printk("%lu ", 655266);
    printk("%lx ", 655266);

    printk("\n%%q prefix test\n");

    printk("%qd ", 655266);
    printk("%qu ", 655266);
    printk("%qx ", 655266);
}
*/

void continuous_polling() {
    int c;
    while (1) {
        c = get_char();
    
        if (c != INVALID_CHAR) {
            printk("%c", c);
        }
    }
}

void write_to_page(void *ptr) {
    
    uint64_t address = (uint64_t) ptr;

    uint64_t *writePtr = (uint64_t *) ptr;

    int i = 0;
    
    for (i = 0; i < 512; i++) {
        *writePtr = address;
        writePtr++;
    }
}

int check_page(void *ptr) {
    uint64_t address = (uint64_t) ptr;

    uint64_t *readPtr = (uint64_t *) ptr;

    int i = 0;
    
    for (i = 0; i < 512; i++) {
        if (*readPtr != address) {
            return FALSE;
        }
        readPtr++;
    }
    return TRUE;
}

void dummy_proc1() {
    printk("dummy proc 1 called\n");
    yield();

    
    printk("dummy proc 1 called 2nd time\n");
    yield();
    printk("dummy proc 1 trying to exit\n");
    //kexit();
}

void dummy_proc2() {
    printk("dummy proc 2 called\n");
    yield();
    printk("dummy proc 2 called 2nd time\n");
    yield();
    printk("dummy proc 2 trying to exit\n");

}

void dummy_proc3() {
    printk("dummy proc 3 called\n");
    yield();
    printk("dummy proc 3 called 2nd time\n");
    yield();

    printk("dummy proc 3 trying to exit\n");

}

void phys_page_alloc_tests() {
  /*
    void *temp = MMU_pf_alloc();
    void *temp2 = MMU_pf_alloc();
    void *temp3 = MMU_pf_alloc();

    write_to_page(temp);
    printk("result: %d\n", check_page(temp));

    write_to_page(temp2);
    printk("result: %d\n", check_page(temp2));

    write_to_page(temp3);
    printk("result: %d\n", check_page(temp3));

    void *ptr;


    int i = 0;
    for (i = 0; i < 32736 + 10; i++) {
        //printk("%d pages allocated\n", i + 1);
        ptr = MMU_pf_alloc();

        if (ptr) {
            write_to_page(ptr);
            printk("result: %d\n", check_page(ptr));
        }
    }
    MMU_pf_free(temp);
    MMU_pf_free(temp2);
    MMU_pf_free(temp3);

    MMU_pf_alloc();
    MMU_pf_alloc();
    MMU_pf_alloc();

*/

}

void coop_multitasking_tests() {
    /*
    PROC_create_kthread(&dummy_proc1, NULL);
    PROC_create_kthread(&dummy_proc2, NULL);
    PROC_create_kthread(&dummy_proc3, NULL);

    PROC_run();*/
}

void keyboard_io(void *ptr)
{

/*
    while (1) { 
        printk("%c", getc());
    }*/

    char c;
    while (1) { 
        c = getc();

        if (c) {
            printk("%c", c);
        }
    }   
}

void kBreak() {
    int loop = 1;   
    while (loop) {

    }
}

//implement a tab as 4 spaces?
void kmain(int tagPtr) {

    VGA_clear();
    change_text_color(OCEAN_BLUE);
    ps2_initialize();
    SER_init();
    uint64_t tags = tagPtr & 0xFFFFFFFF;
    idt_init();
    sti();
    parse_tags(tags);

    printk("about to init page table\n");
    page_table pt;
    page_table *pt_ptr;
    pt_ptr = (page_table *) ptable_init(&pt);

    int*testm = malloc(8);
    int*testm1 = malloc(8);
    *testm1 = 5;
    printk("TESTING MALLOC: value should be 5: %d\n", *testm1);
   

    //setup_snakes(4);
    //PROC_run();
    
    //printk("about to start polling\n");
    //continuous_polling();

    cmd_queue_init();

    //PROC_create_kthread(&cmd_queue_init, NULL);
    
    PROC_create_kthread(&keyboard_io, NULL);
    //kBreak();

    //init_block_devices();
    
    while(1) {
        //printk("PROC_run()\n");
        PROC_run();
        asm("hlt");
    }
}


