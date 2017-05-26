#include "vga.h"
#include "memfuncs.h"
#include "inline_asm.h"
#include "serial.h"

#define VGA_BASE 0xb8000
#define LINE(x) (x % 80)

#define SHORT_MAX_LEN 5
#define INT_MAX_LEN 10
#define LONG_MAX_LEN 20
#define LLONG_MAX_LEN 20  
// ya idk about these....


static unsigned short *vgaBuff = (unsigned short *) VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;


static unsigned char COLOR = RED;





int VGA_row_count(void) {
    return height;
}

int VGA_col_count(void) {
    return width;
}


void VGA_display_attr_char(int x, int y, char c, int fg, int bg) {
    vgaBuff[y * width + x] = (fg << 8) | (bg << 12) | c;
}




void change_text_color(int new) {
    COLOR = new;
}

void new_text_color() {
    COLOR = COLOR + 1;
}

char * toHex(long input) {

    //uint64_t test = input;
    //char *output = "00000000";

    /*
    int loop = 1;
    while (loop) {

    }*/
    char *output = "000000000000";

    static char HEX_ARRAY[] = "0123456789ABCDEF";

    int idx = 11;

    while (input > 0) {
        output[idx--] = HEX_ARRAY[(input & 0xF)];

        input >>= 4;
    }

    int i = 0;

    
    for (i = 0; i < 12; i++) {

        if (*output != '0') {
            break;
        }
        else {
            output++;
        }
    }


    return output;
}


void scroll() {

    int srcOffset = width;
    int destOffset = 0;

    while (srcOffset < width * height) {
        
        memcpy(vgaBuff + destOffset, vgaBuff + srcOffset, width * sizeof(unsigned short)); 

        srcOffset += width;
        destOffset += width;
    }

    cursor -= width;

    memset(vgaBuff + cursor, '\0', width * sizeof(unsigned short));
}


void VGA_clear() {
    int i =0;
    
    for (i = 0; i < width * height; i++) {
        vgaBuff[i] = 0;
    }
    cursor = 0;
}


void VGA_display_str(const char * s) {

    char *temp = (char *) s;

    while (*temp != '\0') {
        VGA_display_char(*temp);
        temp++;
    }

}

void print_char(char c) {
    
    int enable_interrupts = 0;
    if (are_interrupts_enabled()) {
        enable_interrupts = 1;
        cli();
    }
    

    if (c == '\n') {
        cursor = cursor + width - LINE(cursor);
        if (cursor >= width*height) {
            scroll();
        }
    }
    else if (c == '\r') {
        cursor = cursor - LINE(cursor);
    }
    else if (c == '\t') {
        print_char(' ');
        print_char(' ');
        print_char(' ');
        print_char(' ');
        
    }
    else {
        vgaBuff[cursor] = (COLOR << 8) | c;
        cursor++;
        if ( (cursor % width) >= (width - 1) ) {
            print_char('\n');
        }
        
    }

    if (enable_interrupts) {
        sti();
    }
    SER_write(&c, 1);

    
}
int printk(const char *fmt, ...) {


    int enable_interrupts = 0;
    if (are_interrupts_enabled()) {
        enable_interrupts = 1;
        cli();
    }

    va_list params;


    va_start(params, fmt);

    

    //%x %p %h[dux] (value to be displayed as a short/unsigned short) %l[dux] %q[dux] (long long)
    //to do: 
    // -negative numbers?
    // -how to print in x

    int loop = 1;

    while (*fmt) {
        if (*fmt == '%') {
            switch(*++fmt) {
                case 'c':

                    print_char(va_arg(params, int));
                    break;
                case 'u':
                    print_uint(va_arg(params, unsigned int));
                    break;
                case 's':
                    print_str(va_arg(params, char *));
                    break;
                case 'x':
                    /*
                    while (loop) {

                    }*/
                    print_long_hex(va_arg(params, long));
                    break;
                case 'd':
                    print_int(va_arg(params, int));
                    break;
                case 'p':
                    print_long_hex(va_arg(params, long));
                    break;
                case 'h':
                    if (fmt + 1 && *(fmt + 1) == 'd'){
                        fmt++;
                        print_int(va_arg(params, int));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'u') {
                        fmt++;
                        print_int((int)va_arg(params, unsigned));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'x') {
                        fmt++;
                        print_int((int)va_arg(params, unsigned));
                    }
                    else {
                        print_char('%');
                        print_char('h');
                    }
    
                    break;
                case 'l':
                    if (fmt + 1 && *(fmt + 1) == 'd'){
                        fmt++;
                        print_long((long)va_arg(params, int));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'u') {
                        fmt++;
                        print_long((long)va_arg(params, unsigned));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'x') {
                        fmt++;
                        print_long((long)va_arg(params, unsigned));
                    }
                    else {
                        print_char('%');
                        print_char('l');
                    }
    
                    break;
                case 'q':
                   if (fmt + 1 && *(fmt + 1) == 'd'){
                        fmt++;
                        print_quad((long long) va_arg(params, int));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'u') {
                        fmt++;
                        print_quad((long long) va_arg(params, unsigned));
                    }
                    else if (fmt + 1 && *(fmt + 1) == 'x') {
                        fmt++;
                        print_quad((long long) va_arg(params, unsigned));
                    }
                    else {
                        print_char('%');
                        print_char('q');
                    }
    
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

    if (enable_interrupts) {
        sti();
    }
    

    return 1;

}

void print_str(const char *s) {
    VGA_display_str(s);
}

void print_uint(unsigned int u) {
    char values[INT_MAX_LEN];
    unsigned int ndx, value;
    
    value = u;
    ndx = INT_MAX_LEN;
    do {
        values[ndx--] = '0' + value % 10;
        value /= 10;

    } while (value > 0);


    while (ndx < INT_MAX_LEN) {
        print_char(values[++ndx]);
    }
}

void print_int(int n) {
    char values[INT_MAX_LEN + 1];
    unsigned int ndx, value;

    value = n;
    ndx = INT_MAX_LEN;
/*
    if (value < 0) {
        print_char('-');
        value *= -1;
    }
*/
    do {
        values[ndx--] = '0' + value % 10;
        value /= 10;
    } while (value > 0);

    while (ndx < INT_MAX_LEN) {
        print_char(values[++ndx]);
    }
}

/*
void print_uchar(unsigned char u) {
    
}
*/
void print_short(short sh) {

    char values[SHORT_MAX_LEN];
    unsigned int ndx, value;

    value = sh;
    ndx = SHORT_MAX_LEN - 1;
/*
    if (value < 0) {
        print_char('-');
        value *= -1;
    }
*/



    do {
        values[ndx--] = '0' + value % 10;
        value /= 10;

    } while (value > 0);



    while (ndx < SHORT_MAX_LEN - 1) {
        print_char(values[++ndx]);
    }

}

void print_long_hex(long l) {
    char *temp = toHex(l);
    print_str(temp);
}

void print_long(long l) {

    char values[LONG_MAX_LEN];
    unsigned int ndx, value;

    value = l;
    ndx = LONG_MAX_LEN - 1;

/*
    if (value < 0) {
        print_char('-');
        value *= -1;
    }
*/

    do {
        values[ndx--] = '0' + value % 10;
        value /= 10;

    } while (value > 0);


    while (ndx < LONG_MAX_LEN - 1) {
        print_char(values[++ndx]);
    }
    
}

void print_quad(long long l) {

    char values[LLONG_MAX_LEN + 1];
    unsigned int ndx, value;

    value = l;
    ndx = LLONG_MAX_LEN;

/*
    if (value < 0) {
        print_char('-');
        value *= -1;
    }
*/

    do {
        values[ndx--] = '0' + value % 10;
        value /= 10;

    } while (value > 0);


    while (ndx < LLONG_MAX_LEN) {
        print_char(values[++ndx]);
    }
    
}



void VGA_display_char(char c) {

    if (c != '\n') {
        if (cursor % width >= width - 1) {
            print_char('\n');
        }
    }

    print_char(c);
}

void backspace() {
    cursor--;
    vgaBuff[cursor] = (COLOR << 8) | '\0';
    //cursor--;
}

 
