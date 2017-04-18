#include "vga.h"
#include "ps2.h"

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

//implement a tab as 4 spaces?
void kmain() {

    int i = 0;

    VGA_clear();
    
    /*
    for (i = 0; i < 100; i++) {
        VGA_display_char('m');
    }
    VGA_display_char('\n');

    for (i = 0; i < 30; i++) {
        VGA_display_char('w');
        VGA_display_char('o');
    }
    VGA_display_char('\n');
    for (i = 0; i < 20 ; i++) {
        VGA_display_char('h');
        VGA_display_char('i');
        VGA_display_char('a');
    }
    for (i = 0; i < 17; i++) {
        VGA_display_str("hello world\n");
    }

    VGA_display_char('\n');

	printk("%s", "this should print a string\n");

    printk("this should print an h: %c, this should be an i: %c\n", 'h', 'i');
    
	printk("hi");
    
    printk("hello there!");

    printk("  %u  %u  %d %d ", 500, 204, 205, 210 * -1);

    printk("\n%%");

    printk("hex: %x\n", 17);
    */
    
    change_text_color(OCEAN_BLUE);

    printk_tests();

    ps2_initialize();


    int c;
    while (1) {
        c = get_char();
    
        if (c != INVALID_CHAR) {
            printk("%c", c);
        }
    }
}


