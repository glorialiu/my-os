#include "vga.h"

void kmain() {


    int i = 0;

    VGA_clear();
    

    for (i = 0; i < 100; i++) {
        VGA_display_char('m');
        //VGA_display_char('i');
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

    VGA_display_char('a');
    
    VGA_display_char('\n');
    VGA_display_char('w');


    
    VGA_display_char('\n');
    VGA_display_char('i');

    VGA_display_char('\n');
    VGA_display_char('n');



    VGA_display_char('\n');
    VGA_display_char('s');


    VGA_display_char('\n');
    VGA_display_char('p');

    VGA_display_char('\n');
    VGA_display_char('n');



    VGA_display_char('\n');
    VGA_display_char('s');


    VGA_display_char('\n');

	printk("%s", "this should print a string\n");

    printk("this should print an h: %c, this should be an i: %c\n", 'h', 'i');
    
	printk("hi");
    
    printk("hello there!");

    printk("  %u  %u  %d %d ", 500, 204, 205, 210 * -1);

    printk("\n%%");

    printk("hex: %x\n", 17);

    printk("%l", 1000000);
}


