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

extern uint64_t read_cr2(void);

extern void load_cr3(uint64_t newReg);

extern uint64_t read_cr3(void);

extern void insw (uint16_t port, void *addr, int cnt);

extern uint16_t inw(uint16_t port);
