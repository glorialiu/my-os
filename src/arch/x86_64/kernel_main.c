#include "vga.h"
#include "ps2.h"
#include "interrupt.h"
#include "serial.h"
#include "memfuncs.h"
#include "page_alloc.h"
#include "page_table.h"
#include "process.h"
#include "kmalloc.h"
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

void dummy_proc() {
    printk("dummy proc called\n");
}

//implement a tab as 4 spaces?
void kmain(int tagPtr) {

    VGA_clear();


    change_text_color(OCEAN_BLUE);

    ps2_initialize();
    SER_init();


    //printk("tag ptr: %x\n", tagPtr);

    uint64_t tags = tagPtr & 0xFFFFFFFF;

    idt_init();
    sti();

    parse_tags(tags);
   
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

    printk("about to init page table\n");
    
    page_table pt;

    page_table *pt_ptr;
    pt_ptr = (page_table *) ptable_init(&pt);
    /*
    int loop = 1;
    while(loop) {
    }
    */

    int * testv = (int *) alloc_heap_vpage(pt_ptr);
    /*
    int loop = 1;
    while(loop) {
    }
    */


    printk("pointer is %x\n", testv);
    *testv = 1;

    printk("value is %d\n", *testv);

    int*testm = malloc(8);

    int*testm1 = malloc(8);

    *testm1 = 5;

    printk("value should be 5: %d\n", *testm1);

    /*
    int loop = 1;
    while(loop) {
    }*/
   
    // printk_tests();
    //SER_write("hi", 3);

   // char *test = "wow this is a long output string woohoooooo !!!!!\n\n testing new lines yeah\n\nwefjeiaow;ejfwefwf ksdjflasjdf;asBoth have the same basic types of pins. A DB-25 has most of the pins as ground pins, whereas a DE-9 has only a few ground pins. There is a transmitting pin (for sending information away) and a receiving pin (for getting information). Some serial ports can have a duplex mode--that is, they can send and receive simultaneously. There are a few other pins, used for hardware handshaking. In the past, there was no duplex mode, so if a computer wanted to send something it had to tell the other device or computer that it was about to transmit, using one of the hardware handshaking pins. The other thing would then use another handshaking pin to tell it to send whatever it wanted to send. Today there is duplex mode, but the handshaking pins are still used.";
   // SER_write(test, strlen(test));

   
/*

    int loop = 1;
    while(loop) {
    }

    uint64_t fault = 0xFFFFFFFFFFFF;

    int test = * ((int *) fault);
    
    printk("test is %d\n", test);

    */


    //asm volatile("int $8"); //double fault
    //asm volatile("int $14"); //page fault
    //asm volatile("int $13");

  //  asm volatile("int $8"); //double fault
  //  asm volatile("int $14"); //page fault

    
//    kexit();

    
    
    yield();

    PROC_create_kthread(&dummy_proc, NULL);
    
    printk("about to start polling\n");
    continuous_polling();

}


