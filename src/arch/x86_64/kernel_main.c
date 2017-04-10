#include "vga.h"

void kmain() {


    int i = 0;

    VGA_clear();

    for (i = 0; i < 40; i++) {
        VGA_display_char('m');
        VGA_display_char('i');
    }

    VGA_display_char('\n');

    for (i = 0; i < 30; i++) {
        VGA_display_char('w');
        VGA_display_char('o');
    }

    VGA_display_char('\n');

    for (i = 0; i < 20; i++) {
        VGA_display_char('n');
        VGA_display_char('o');
    }

    VGA_display_str("\n\nhello world");
  
}


