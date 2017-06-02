#include "interrupt.h"
#include "vga.h"
#include "serial.h"
#include "page_table.h"
#include "process.h"
#include "ps2.h"
#include "block.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2
#define PIC1_DATA (PIC1+1)
#define PIC2_DATA (PIC2+1)
#define PIC_EOI 0x20
#define IRQS_PER_PIC 8
#define PIC_INIT 0x11
#define PIC1_START 0x20
#define PIC2_START 0x28
#define IRQ2 4
#define ICW4_8086 0x01



#define IDT_ENTRY_SIZE 16
#define IOPB_SIZE 16
#define TSS_SEGMENT_SIZE 104
#define TSS_DESC_SIZE 16
#define GDT_ENTRY_SIZE 8


#define INTERRUPT_TYPE 0xE
#define TRAP_TYPE 0xF

#define HARDWARE 0
#define SOFTWARE 3

#define STACK_SIZE 4096

// this will probably change
#define NUM_GDT_ENTRIES 8

#define KERNEL_SEGMENT 0x08
#define TSS_SEGMENT 0x10


/* for interrupts */
struct IDT_entry IDT_table[NUM_INTERRUPTS];
struct IRQT irq_table[NUM_INTERRUPTS];

/* special stacks for certain faults */

char DF_stack[STACK_SIZE];
char GPF_stack[STACK_SIZE];
char PF_stack[STACK_SIZE];
char SYS_stack[STACK_SIZE];


struct TSS ts_segment;
struct TSS_descriptor ts_descriptor;
struct IOPB io_bitmap;

// GLOBAL DESCRIPTOR TABLE
struct GDT_entry GDT[NUM_GDT_ENTRIES];


extern int tss;
extern int gdt64;

void idt_init() {

    // initialize PIC in IRQ_init
    IRQ_init();

    // set masks for interrupt lines so no interrupts occur
    int i = 0;
    for (i = 0; i < 16; i++) {
        IRQ_set_mask(i);
    }

    // initialize IDT
    memset(&IDT_table, 0, NUM_INTERRUPTS * IDT_ENTRY_SIZE);

    // load IDT into special register
    lidt(&IDT_table, NUM_INTERRUPTS * IDT_ENTRY_SIZE);

    // see all IDT entries in IDT_table to jump to irq_handler0
    // also fill irq_table (c-side) with dummy handlers
    for (i = 0; i < 256; i++) {
        set_handler_in_IDT(&IDT_table[i], (uint64_t) &irq_handler0, KERNEL_SEGMENT, 0);
        IRQ_set_handler(i, dummy_handler, NULL);
    }
   
    // set handlers that get their own stack

    set_handler_in_IDT(&IDT_table[13], (uint64_t) &irq_gpf_handler, KERNEL_SEGMENT, 1);
    IRQ_set_handler(13, gpf_handler, NULL);

    set_handler_in_IDT(&IDT_table[8], (uint64_t) &irq_handler8, KERNEL_SEGMENT, 2);
    IRQ_set_handler(8, df_handler, NULL);

    set_handler_in_IDT(&IDT_table[14], (uint64_t) &irq_pf_handler, KERNEL_SEGMENT, 3);
    IRQ_set_handler(14, pf_handler, NULL);

    // set handlers that dont get their own stack
    // set segment not present handler
     set_handler_in_IDT(&IDT_table[11], (uint64_t) &irq_handler11, KERNEL_SEGMENT, 0);
    IRQ_set_handler(11, segment_np_handler, NULL);   
    // set keyboard handler
    set_handler_in_IDT(&IDT_table[33], (uint64_t) &irq_handler33, KERNEL_SEGMENT, 0);
    IRQ_set_handler(33, keyboard_handler, NULL);
    // set invalid tss handler
    set_handler_in_IDT(&IDT_table[10], (uint64_t) &irq_handler10, KERNEL_SEGMENT,0);
    IRQ_set_handler(10, invalid_tss_handler, NULL);
    // set handler for serial port (IRQ #4), which would be interrupt # 0x24
    set_handler_in_IDT(&IDT_table[0x24], (uint64_t) &irq_handler36, KERNEL_SEGMENT, 0);
    IRQ_set_handler(0x24, serial_irq_handler, NULL);
    

    // set handler for system calls
    set_handler_in_IDT(&IDT_table[123], (uint64_t) &irq_handler123, KERNEL_SEGMENT, 4);
    IRQ_set_handler(123, syscall_handler, NULL);
    init_syscall_handler_table();

    // set handler for ATA
    set_handler_in_IDT(&IDT_table[0x2E], (uint64_t) &irq_handler46, KERNEL_SEGMENT, 0);
    IRQ_set_handler(0x2E, ata_handler, NULL);

    TSS_init();
    ltr(TSS_SEGMENT);

    IRQ_clear_mask(4);
    IRQ_clear_mask(1);

    

    printk("*****INTERRUPTS INITIALIZED*****\n");
}

void segment_np_handler(int num, int error, void *arg) {
    printk("EXCEPTION: SEGMENT NOT PRESENT. Error code %d.\n", error);   

    error_code_print(error);

    asm volatile("hlt");
}

void df_handler(int num, int error, void *arg) {
    printk("EXCEPTION: DOUBLE FAULT OCCURRED.\n");

    asm volatile("hlt");
}

void pf_handler(int num, int error, void *arg) {
    page_fault_handler(num, error, arg);
}

void dummy_handler(int num, int error, void *arg) {
    printk("DUMMY HANDLER: %d\n", num);

    asm volatile("hlt");
}

void keyboard_handler(int num, int error, void *arg) {
    kbd_isr();
}

void invalid_tss_handler(int num, int error, void *arg) {
    printk("EXCEPTION: INVALID TSS.\n");
    asm volatile("hlt");
}

void gpf_handler(int num, int error, void *arg) {
    printk("EXCEPTION: GENERAL PROTECTION FAULT. Error code %d.\n", error);
    error_code_print(error);

    asm volatile("hlt");
}

void ata_handler(int num, int error, void *arg) {
    //printk("ata handler\n");
    ata_isr();
    //asm volatile("hlt");
}

//address: address of asm interrupt handler
//segment: kernel (0x08)
//tss_st_num: stack that interrupt should be pushed on in TSS, 0 if not on TSS
void set_handler_in_IDT(IDT_entry *entry, uint64_t address, uint16_t segment, int tss_st_num) {

    uint64_t mask = (1 << 16) - 1;

    entry->offset_15_0 = address & mask;
    entry->offset_31_16 = (address >> 16) & mask;
    entry->offset_63_32 = (address >> 32) & (0x100000000 - 1);
    
    entry->type = INTERRUPT_TYPE;
    entry->DPL = HARDWARE;
    entry->present = 1;
    entry->selector = KERNEL_SEGMENT;

    entry->IST = tss_st_num;
}

//entry point for all interrupts
void irq_c_handler(int num, int error, int paramForSysCall) {

    IRQT *target = &irq_table[num];

    if (num == 123) {
        target->handler(num, error, &paramForSysCall);
    }
    else {
        target->handler(num, error, NULL); 
    }

    IRQ_end_of_interrupt(num);

}
void TSS_init() {

    // intialize tss segment to all zeroes
    memset((void *) &ts_segment, 0, TSS_SEGMENT_SIZE);

    // fill in fields of tss
    ts_segment.interrupt_st1 = (uint64_t) &GPF_stack[STACK_SIZE - 8];
    ts_segment.interrupt_st2 = (uint64_t) &DF_stack[STACK_SIZE - 8];
    ts_segment.interrupt_st3 = (uint64_t) &PF_stack[STACK_SIZE - 8];
    ts_segment.interrupt_st4 = (uint64_t) &SYS_stack[STACK_SIZE - 8];

    // io_map needs to point to io bit map
    ts_segment.io_map = (uint64_t) &io_bitmap - (uint64_t) &ts_segment;
    memset((void *) &io_bitmap, 0, IOPB_SIZE);
    io_bitmap.ones = 0xFF;

    
    // initialize tss descriptor to all zeroes
    memset((void *) &ts_descriptor, 0, TSS_DESC_SIZE);

    uint64_t ts_address = (uint64_t) &ts_segment;
    ts_descriptor.limit_0_15 = TSS_SEGMENT_SIZE - 1;
    ts_descriptor.base_0_23 = ts_address & ((1<<24) - 1);
    ts_descriptor.base_24_31 = (ts_address >> 24) & ((1<<9) - 1);
    ts_descriptor.base_32_63 = (ts_address >> 32) & (0x100000000 - 1);
 
    ts_descriptor.type = 9;//0b1001;
    ts_descriptor.privilege = 0;
    ts_descriptor.present = 1;

    TSS_descriptor *ts_ptr = (TSS_descriptor*) &tss;
    *ts_ptr = ts_descriptor;
}

void IRQ_init(void) {

    unsigned char a1, a2;
    
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, PIC_INIT);
    io_wait();

    outb(PIC2_COMMAND, PIC_INIT);
    io_wait();

    //gives offsets
    outb(PIC1_DATA, PIC1_START);
    io_wait();
    outb(PIC2_DATA, PIC2_START);
    io_wait();

    //telling master which one is the slave
    outb(PIC1_DATA, IRQ2);
    io_wait();
    
    //telling slave its cascade identity
    outb(PIC2_DATA, 2); 
    io_wait();

    //gives environment information
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char irq) {
    uint16_t port;
    uint8_t value;

    if (irq < IRQS_PER_PIC) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= IRQS_PER_PIC;
    }

    value = inb(port) | 1 << irq;
    outb(port, value);
}

void IRQ_clear_mask(unsigned char irq) {
    uint16_t port;
    uint8_t value;

    if (irq < IRQS_PER_PIC) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= IRQS_PER_PIC;
    }

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

int IRQ_get_mask(int IRQline) { 
    return IRQline;
}

void IRQ_end_of_interrupt(int irq) {

    if (irq > IRQS_PER_PIC) {
        //if IRQ came from slave PIC, then command must be issues to both PICS
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);

}

void IRQ_set_handler(int irq, irq_handler_t handler, void *arg) {
    IRQT *new = &irq_table[irq];
    
    new->handler = handler;
    new->arg = arg;
}


void error_code_print(int error) {
    int mask = 0b11;
    
    int table = mask & (error >> 1);

    if (table == 0b00) {
        printk("    occured in GDT");
    }
    else if (table == 0b01) {
        printk("    occured in IDT");
    }
    else {
        printk("    occured in LDT");
    }

    printk(" at index %d\n", error >> 3);
}


