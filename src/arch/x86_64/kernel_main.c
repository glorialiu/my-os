#include "vga.h"
#include "ps2.h"
#include "interrupt.h"
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

//implement a tab as 4 spaces?
void kmain() {

    //VGA_clear();


    int loop = 0;
    while(loop) {
    }

  
    idt_init();

    change_text_color(OCEAN_BLUE);

   // printk_tests();

    ps2_initialize();

    sti();

    continuous_polling();



}


