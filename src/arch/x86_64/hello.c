void sys(int param1, int param2, int sysNum) {
    asm("int $123");

}

void sys_char(int param1, char* param2, int sysNum) {
    asm("int $123");

}


char getc_sys() {
    char c ;
    sys_char(0, &c,5);

    return c;
}

void putc_sys(char c) {
    sys(0,c,6);
}


void _start() {
    char c;

    putc_sys('h');

    putc_sys('h');
    putc_sys('e');
    putc_sys('l');
    putc_sys('l');
    putc_sys('o');
    putc_sys(' ');
    putc_sys('w');    
    putc_sys('o');
    putc_sys('r');
    putc_sys('l');
    putc_sys('d');
    putc_sys('\n');


    while(1) {
    /*
        c = getc_sys();

        if (c) {
            putc_sys(c);
        }
    */
    }
}

