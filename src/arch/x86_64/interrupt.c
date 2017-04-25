#include "interrupt.h"
#include "vga.h"

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

#define IDT_ENTRY_SIZE 2

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

struct TSS ts_segment;
struct TSS_descriptor ts_descriptor;

// GLOBAL DESCRIPTOR TABLE
struct GDT_entry GDT[NUM_GDT_ENTRIES];



// segment is KERNEL_SEGMENT or TSS_SEGMENT. tss_st_num must be 0 if KERNEL_SEGMENT 
void set_handler_in_IDT(IDT_entry *entry, uint64_t address, int segment, int tss_st_num) {

    uint64_t mask = (1 << 16) - 1;

    entry->offset_15_0 = address & mask;

    entry->offset_31_16 = (address >> 16) & mask;

    entry->offset_63_32 = (address >> 32) & (0x100000000 - 1);
    
    entry->type = INTERRUPT_TYPE;

    entry->DPL = HARDWARE;

    entry->present = 1;

    entry->selector = segment;

    entry->IST = tss_st_num;
}

void idt_init() {
/*
    int loop = 1;
    while(loop) {
    }
*/
    // initialize PIC in IRQ_init and initialize TSS 
    IRQ_init();
    TSS_init();

    // set masks for interrupt lines so no interrupts occur
    int i = 0;
    for (i = 0; i < 16; i++) {
        IRQ_set_mask(i);
    }

    // initialize IDT
    memset(&IDT_table, 0, NUM_INTERRUPTS * IDT_ENTRY_SIZE);

    // load IDT into special register
    lidt(&IDT_table, NUM_INTERRUPTS * IDT_ENTRY_SIZE);

    // ste all IDT entries in IDT_table to jump to irq_handler0, and fill irq_table (c-side) with dummy handlers
    for (i = 0; i < 256; i++) {
        set_handler_in_IDT(&IDT_table[i], (uint64_t) &irq_handler0, KERNEL_SEGMENT, 0);
        IRQ_set_handler(i, dummy_handler, NULL);
    }
   
    // set handlers that get their own stack

    // set gpf handler, still pointing to dummy handler though (TO DO: change KERNEL_SEGMENT to TSS_SEGMENT and change stack num
    set_handler_in_IDT(&IDT_table[13], (uint64_t) &irq_gpf_handler, KERNEL_SEGMENT, 0);
    IRQ_set_handler(13, gpf_handler, NULL);

    // set handlers that dont get their own stack

    // set keyboard handler
    set_handler_in_IDT(&IDT_table[33], (uint64_t) &irq_handler33, KERNEL_SEGMENT, 0);
    IRQ_set_handler(33, keyboard_handler, NULL);

    // set invalid tss handler
    set_handler_in_IDT(&IDT_table[10], (uint64_t) &irq_handler10, KERNEL_SEGMENT,0);
    IRQ_set_handler(10, invalid_tss_handler, NULL);

    IRQ_clear_mask(1);
    
    printk("idt initialized");
}


void dummy_handler(int num, int error, void *arg) {
    printk("dummy handler called\n");
}

void keyboard_handler(int num, int error, void *arg) {
    printk("keyboard handler called\n");
}

void invalid_tss_handler(int num, int error, void *arg) {
    printk("invalid tss handler called\n");
}

void gpf_handler(int num, int error, void *arg) {
    printk("gpf handler called with error %d\n", error);
}

//interrupt entry point
void irq_c_handler(int num, int error) {

    IRQT *target = &irq_table[num];

    printk("IRQ num is: %d\n", num);

    target->handler(num, error, NULL); 
    IRQ_end_of_interrupt(num);
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

void TSS_init() {


/*
    int loop = 1;
    while(loop) {
    }
*/

    // intialize tss segment to all zeroes
    memset((void *) &ts_segment, 0, sizeof(TSS));

    // fill in fields of tss
    //ts_segment.interrupt_st1 = (uint64_t) &DF_stack - STACK_SIZE;
    //ts_segment.interrupt_st2 = (uint64_t) &GPF_stack - STACK_SIZE;
    //ts_segment.interrupt_st3 = (uint64_t) &PF_stack - STACK_SIZE;

    //need to do i/o field?

  
    
    // initializse tss descriptor to all zeroes
    memset((void *) &ts_descriptor, 0, sizeof(ts_descriptor));

    uint64_t ts_address = (uint64_t) &ts_segment;
    ts_descriptor.limit_0_15 = ts_address + sizeof(ts_segment) - 1;
    ts_descriptor.base_0_23 = ts_address & ((1<<24) - 1);
    ts_descriptor.base_24_31 = (ts_address >> 24) & ((1<<9) - 1);
    ts_descriptor.base_32_63 = (ts_address >> 32) & (0x100000000 - 1);
 
    ts_descriptor.type = 0b1001;
    ts_descriptor.privilege = 0;
    ts_descriptor.present = 1;

    // initialize GDT

    memset((void *) &GDT, 0, sizeof(GDT_entry) * NUM_GDT_ENTRIES);

    /* GDT[0] is null */

    /* GDT[1] is CS */

    uint64_t temp = 1;
    uint64_t cs = (temp<<43) | (temp<<44) | (temp<<47) | (temp<<53);
    uint64_t *cs_ptr = (uint64_t *) &GDT[1];

    *cs_ptr = cs;

    /* GDT[2]-[3] is TSS */
    TSS_descriptor *ts_ptr = (TSS_descriptor*) &GDT[2];
    *ts_ptr = ts_descriptor;

    lgdt(&GDT,  sizeof(GDT_entry) * NUM_GDT_ENTRIES);

    /* load TSS */
    //ltr(0x10);


}
 
