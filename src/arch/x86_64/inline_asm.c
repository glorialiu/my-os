#include "inline_asm.h"

inline void outb(uint16_t port, uint8_t val) {
    asm volatile( "outb %0, %1" : : "a" (val), "Nd"(port) );
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;

    asm volatile ( "inb %1, %0" : "=a" (ret) : "Nd" (port) );

    return ret;

}

inline void io_wait(void) {
    asm volatile ("jmp 1f\n\t"
                  "1:jmp 2f\n\t"
                  "2:");
}

inline void lidt(void *base, uint16_t size) {
    struct {
        uint16_t length;
        void *base;
    } __attribute__((packed)) IDTR = {size, base};

    asm ( "lidt %0" : : "m"(IDTR) );  
}   

inline void lgdt(void *base, uint16_t size) {
    struct {
        uint16_t length;
        void *base;
    } __attribute__((packed)) GDTR = {size, base};

    asm ( "lgdt %0" : : "m"(GDTR) );  
}  

inline int are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

inline void cli() {
    asm volatile ("cli");
}

inline void sti() {
    asm volatile ("sti");
}
