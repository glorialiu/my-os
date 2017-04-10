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
        cursor++;
        //if ( (cursor % width) < (width - 1) ) {
        //    cursor++;
        //}
        
    }
    
}


void kmain() {

    //vgaBuff[0] = (color << 8) | 'c';

    int i = 0;

    for (i = 0; i < 100; i++) {
        print_char('m');
        print_char('i');
    }

    print_char('\r');

    for (i = 0; i < 100; i++) {
        print_char('w');
        print_char('o');
    }

  
}

/*
extern void print_char(char);
extern void print_str(const char *);
extern void print_uchar(unsigned char);
extern void print_short(short);
extern void print_long_hex(long);
*/

