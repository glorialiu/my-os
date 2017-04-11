#include "vga.h"
#include "memfuncs.h"

#define VGA_BASE 0xb8000
#define LINE(x) (x % 80)


static unsigned short *vgaBuff = (unsigned short *) VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;


static unsigned char color = 4;

void scroll() {
    int i = 0;

    int srcOffset = width;
    int destOffset = 0;

    while (srcOffset < width * height) {
        
        memcpy(vgaBuff + destOffset, vgaBuff + srcOffset, width * sizeof(unsigned short)); 

        srcOffset += width;
        destOffset += width;
    }

    cursor -= width;
}

void print_char(char c) {

    if (c == '\n') {
        cursor = cursor + width - LINE(cursor);
        if (cursor >= width*height) {
            scroll();
        }
    }
    else if (c == '\r') {
        cursor = cursor - LINE(cursor);
    }
    else {
        vgaBuff[cursor] = (color << 8) | c;
        //if ( (cursor % width) < (width - 1) ) {
           cursor++;
        //}
        
    }
    
}
int printk(const char *fmt, ...) {

    va_list params;


    va_start(params, fmt);


    //%% %d %u %x %c %p %h[dux] (value to be displayed as a short/unsigned short) %l[dux] %q[dux] (long long) %s
    
    while (*fmt) {
        if (*fmt == '%') {
            switch(*++fmt) {
                case 'c':
                    print_char(va_arg(params, int));
                    break;
                case 'u':
                    print_long((long) va_arg(params, unsigned int));
                    break;
                case 's':
                    print_str(va_arg(params, char *));
                    break;
                case 'x':
                    print_long_hex(va_arg(params, long));
                    break;
                case 'd':
                    print_long((long)va_arg(params, int));
                case 'h':
                    if (fmt + 1 && *(fmt + 1) == 'd'){
                        
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'u') {

                    }
                    else if (fmt + 1 && *(fmt + 1) == 'x') {

                    }
                    else {

                    }
    
                    break;
                case 'l':

                    break;
                case 'q':
            
                    break;
                case '%':
                    print_char('%');
                    break;
                default:
                    print_char('%');
                    print_char(*fmt);
            }
        }
        else {
            print_char(*fmt);
        }
        fmt++;
    }


    va_end(params);

}

void print_str(const char *s) {

}

void print_uchar(unsigned char u) {

}

void print_short(short sh) {

}

void print_long_hex(long l) {

}

void print_long(long l) {

}


void VGA_display_char(char c) {
/*
    if (c != '\n') {
        if (cursor % width == width - 1) {
            print_char('\n');
        }
    }
*/
    print_char(c);
}

void VGA_clear() {
    int i =0;
    
    for (i = 0; i < width * height; i++) {
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
 
