#include <stdint.h>

extern void outb(uint16_t port, uint8_t val);

extern uint8_t inb(uint16_t port);

extern void io_wait(void);

extern void lidt(void *base, uint16_t size);

extern void lgdt(void *base, uint16_t size);

extern int are_interrupts_enabled();

extern void cli(void);

extern void sti(void);

extern void ltr(uint16_t offset);

extern void interrupt(uint16_t num);


