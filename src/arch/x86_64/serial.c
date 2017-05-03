#include "serial.h"
#include "inline_asm.h"
#include "vga.h"


#define COM_PORT1 0x3f8  
#define IIR_PORT COM_PORT1+2 
#define LSR_PORT COM_PORT1+5
#define THB_WRITE COM_PORT1
 
#define LINE 3
#define TX 1

#define TRUE 1
#define FALSE 0

int idle = TRUE;


State curState;


void init_state() {
    curState.head = &curState.buffer[0];
    curState.tail = &curState.buffer[0];
    idle = FALSE;
}


void consumer_next(struct State *state) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    
    if (state->head == state->tail) {
        return;
    }

    write_byte(*state->head++);

    // if head is past end, loop around
    if (state->head >= &state->buffer[BUFFER_LEN]) {
        state->head = &state->buffer[0];
    }   

    if (interrupts) {
        sti();
    }

}

void producer_add_char(char toAdd, struct State *state) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    if (state->head - 1 == state->tail || (state->head == state->buffer && state->tail == state->buffer + BUFFER_LEN - 1)) {
        //its full
        //printk("BUFFER FULL\n");
        return; //??
    }

    *state->tail++ = toAdd;
    //printk("%c added to buffer\n", toAdd);

    if (state->tail >= state->buffer + BUFFER_LEN) {
        state->tail = &state->buffer[0];
    }      

    if (interrupts) {
        sti();
    } 
}

void SER_init(void) {

   init_state();

   //outb(COM_PORT1 + 1, 0x00);    // Disable all interrupts

   //outb(COM_PORT1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  // outb(COM_PORT1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  // outb(COM_PORT1 + 1, 0x00);    //                  (hi byte)

   outb(COM_PORT1 + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(COM_PORT1 + 1, 0x02);   //enable TX

   //outb(COM_PORT1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   //outb(COM_PORT1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set

}

void SER_write(const char *buff, int len) {
    int i = 0;
    
    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    
    for (i = 0; i < len; i++) {
        // if its full just drop the input
        //printk("about to call producer add on %c\n", buff[i]);
        producer_add_char(buff[i], &curState); 
    }

    consumer_next(&curState);

    if (interrupts) {
        sti();
    }
}

int is_transmit_empty() {
   return inb(COM_PORT1 + 5) & 0x20;
}

void write_byte(char c) {
    //printk("trying to write %c\n", c);
    if (idle == FALSE && is_transmit_empty()) {
        idle = TRUE;
    }
    if (idle == TRUE) {
       // printk("writing byte %c\n", c);
        outb(THB_WRITE, c);
    }
}



void serial_irq_handler(int num, int err, void *arg) {

    //printk("IRQ NUMBER: %d\n", num);
    
    uint16_t iir = inb(IIR_PORT);


    if ((iir & 1) == 0) {
        if (((iir >> 1) & 7) == LINE) {
            inb(LSR_PORT);
        }
        else if(((iir >> 1) & 7) == TX) {
            // COM data port is ready for more data
            //printk("com port ready for more data\n");
            // set to bit to idle
            idle = TRUE;

            consumer_next(&curState);
        }
    }

}



