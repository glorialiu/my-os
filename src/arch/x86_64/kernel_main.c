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


    printk("%c", 'h');
    
}


