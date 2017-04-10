#include "vga.h"

#define VGA_BASE 0xb8000
#define LINE(x) (x % 80)


static unsigned short *vgaBuff = (unsigned short *) VGA_BASE;
static int width = 80;
static int height = 20;
static int cursor = 0;


static unsigned char color = 4;

void print_char(char c) {

    if (c == '\n') {
        cursor = cursor + width - LINE(cursor);
        if (cursor >= width*height) {
         //   scroll();
        }
    }
    else if (c == '\r') {
        cursor = cursor - LINE(cursor);
    }
    else {
        vgaBuff[cursor] = (color << 8) | c;
        if ( (cursor % width) < (width - 1) ) {
           cursor++;
        }
        
    }
    
}


void VGA_display_char(char c) {

    if (c != '\n') {
        if (cursor % width == width) {
            print_char('\n');
        }
    }

    print_char(c);
}

void VGA_clear() {
    int i =0;
    
    for (i = 0; i < 80 * 20; i++) {
        vgaBuff[i] = 0;
    }
}


void VGA_display_str(const char * s) {

    const char *temp = s;

    while (*s != '\0') {
        VGA_display_char(*s);
        s++;
    }

}
 
