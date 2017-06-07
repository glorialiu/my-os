
void sys(int param1, int param2, int sysNum) {
    asm("int $123");
}

char getc1() {
    sys(0,0,5);
}

void putc1(char c) {
    sys(0,c,6);
}

void _start() {

    while(1) {
        putc1('c');
        //putc1(getc1());
    }

}
