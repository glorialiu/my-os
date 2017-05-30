#include "ps2.h"
#include "vga.h"
#include "kmalloc.h"
#include "process.h"
#include "types.h"

#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)

#define DISABLE_PORT1 0xAD
#define DISABLE_PORT2 0xA7
#define READ_RAM_0 0x20
#define WRITE_CONFIG 0x60
#define ENABLE_PORT1 0xAE

#define KEYBOARD_RESET 0xFF
#define SET_SCAN_1 0xF1
#define SET_SCAN_2 0xF2
#define ENABLE_SCANNING 0xF4
#define SELFTEST_PASSED 0xAA

#define ACK 0xFA
#define RESEND 0xFE


#define LSHIFT 42
#define RSHIFT 54

#define MAX_ATTEMPTS 3

#define INVALID -1

#define DEFAULT 10
#define WAITING_FOR_ACK 11
#define SCANNING_ENABLING_REQUEST_SENT 12
#define WAITING_FOR_SCAN_CODE 13
#define SCANNING_ENABLED 14

#define KBD_BUFFER_LEN 4096

int SHIFT_ON = FALSE;
int CNTRL_ON = FALSE;

typedef struct kbdCommand {
    uint8_t command;
    struct kbdCommand *next;
} kbdCommand;

typedef struct CommandQueue {
    kbdCommand *head;
    int state;
} CommandQueue;

struct CommandQueue *cmdQ;

struct ProcessQueue *kbdPQ;

char PCBuffer[KBD_BUFFER_LEN];

//initialization

void cmd_queue_init() {

    kbdPQ = malloc(sizeof(ProcessQueue));
    PROC_init_queue(kbdPQ);

    cmdQ= malloc(sizeof(CommandQueue));
    cmdQ->head = NULL;
    



    //temporarily setting this for debugging
    //cmdQ->state = WAITING_FOR_SCAN_CODE;


    

    //initialize state machine state
    cmdQ->state = DEFAULT;

    //add all init commands to queue
    add_to_queue(KEYBOARD_RESET);
    //KEYBOARD_RESET
        //- need to get "self test passed"

    add_to_queue(SET_SCAN_1);
    add_to_queue(ENABLE_SCANNING);

    
}

//call this in kbd_isr
void kbd_isr() {

    uint8_t data = inb(PS2_DATA);

    /*
    int loop = 1;
    while (loop) {

    }*/

    if (cmdQ->state != WAITING_FOR_SCAN_CODE) {
        //what if we get an ACK when we aren't waiting for one? what does that mean
        if (data == ACK || data == SELFTEST_PASSED) {

            if (cmdQ->state == WAITING_FOR_ACK) {
                cmdQ->state = DEFAULT;
                remove_queue_head();
            }
            
            else if (cmdQ->state == SCANNING_ENABLING_REQUEST_SENT) {
                cmdQ->state = WAITING_FOR_SCAN_CODE;
                remove_queue_head();
                }
        }

        //if queue is not empty, send the command
        if (cmdQ->head) {
            send_queue_head();
        }           
    }
    else {
        //data is the scan code

     /*   
    int loop =  1;
    while(loop ) {

    }*/
        char key = get_scancode(data); // stuff with this needs to be fixed.. like shift and tab etc
        write_kbd_buffer(key);
        PROC_unblock_head(kbdPQ);      
    }
}

void remove_queue_head() {

    kbdCommand *head = cmdQ->head;

    if (cmdQ->head) {

        cmdQ->head = cmdQ->head->next;

        free(head);
    }
    else {
        printk("well this is bad. trying to remove a null head.\n");
    }

}

//dont remove the head here!
void send_queue_head() {

    outb(PS2_CMD, cmdQ->head->command);
    
    if (cmdQ->head == SCANNING_ENABLED) {
        cmdQ->state = SCANNING_ENABLING_REQUEST_SENT;
    }
    else {
        cmdQ->state = WAITING_FOR_ACK;
    }

}


char read_kbd_buffer() {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    
    if (kbdPQ->read == kbdPQ->write) {

        if (interrupts) {
            sti();
        } 
        return;
    }

    char newChar = *kbdPQ->read++;

    // if head is past end, loop around
    if (kbdPQ->read >= &kbdPQ->buffer[KBD_BUFFER_LEN]) {
        kbdPQ->read = &kbdPQ->buffer[0];
    }   

    if (interrupts) {
        sti();
    }

    return newChar;
}

char getc() {
    int interrupts = FALSE;
    
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    while(kbdPQ->read == kbdPQ->write) {
        PROC_block_on(kbdPQ, TRUE);
        cli();
    }

    char newChar = read_kbd_buffer(&newChar);

    if ((newChar != 0xA) && (newChar < 32 || newChar > 127)) {
        newChar = NULL;
    }


    if (interrupts) {
        sti();
    }

    return newChar;
}

void write_kbd_buffer(char toAdd) {

    int interrupts = FALSE;
    if (are_interrupts_enabled()) {
        interrupts = TRUE;
        cli();
    }

    //head is what we read from (consumer)
    //tail is what we write to (producer)

    if (kbdPQ->read - 1 == kbdPQ->write || (kbdPQ->read == kbdPQ->buffer && kbdPQ->write == kbdPQ->buffer + KBD_BUFFER_LEN - 1)) {
        //its full

        if (interrupts) {
            sti();
        } 
        return; //??
    }

    *kbdPQ->write++ = toAdd;

    if (kbdPQ->write >= kbdPQ->buffer + KBD_BUFFER_LEN) {
        kbdPQ->write = &kbdPQ->buffer[0];
    }      

    if (interrupts) {
        sti();
    } 
}


void add_to_queue(int cmd) {
    
    kbdCommand *new = malloc(sizeof(kbdCommand));

    new->command = cmd;
    new->next = NULL;
}


void poll_and_write(int port, int data) {

    uint8_t status_int;
    status_reg *status;

    status_int = inb(PS2_STATUS);

    status = (status_reg*) &status_int;

    while (status->output_status) {
        status_int = inb(PS2_STATUS);
        status = (status_reg *) &status_int;
    }

    outb(port, data);
}

uint8_t poll_and_read(int port) {

    uint8_t status_int;
    status_reg *status;

    status_int = inb(PS2_STATUS);

    status = (status_reg*) &status_int;

    while (status->input_status) {
        status_int = inb(PS2_STATUS);
        status = (status_reg *) &status_int;
    }

    return inb(port);
}


void ps2_initialize(void) {

    uint8_t config_int;
    ps2_config* config;

    poll_and_write(PS2_CMD, DISABLE_PORT1);   
    poll_and_write(PS2_CMD, DISABLE_PORT2);
    poll_and_write(PS2_CMD, READ_RAM_0);

    config_int = poll_and_read(PS2_DATA);

    config = (ps2_config*) &config_int;
 
    config->port1_interrupt = ON;
    config->port1_clock = ON; 

    config->port2_interrupt = OFF;
    config->port2_clock = OFF;

    poll_and_write(PS2_CMD, WRITE_CONFIG);
    poll_and_write(PS2_DATA, config_int); 

    poll_and_write(PS2_CMD, ENABLE_PORT1);
    //port 2 is not enabled again, do we even have to do this right now?

    keyboard_initialize(); 
}

void write_to_keyboard(int cmd) {

    uint8_t result;
    int attempts = 0;
    
    poll_and_write(PS2_DATA, cmd);

    result = poll_and_read(PS2_DATA);

    while (result != ACK && attempts < MAX_ATTEMPTS) {
        poll_and_write(PS2_DATA, cmd);
        result = poll_and_read(PS2_DATA);   
        attempts++;
    }

}

void keyboard_initialize(void) {
    
    poll_and_write(PS2_DATA, KEYBOARD_RESET);

    int result = INVALID;
    while (result != SELFTEST_PASSED) {
        result = poll_and_read(PS2_DATA);
    }

    write_to_keyboard(SET_SCAN_1);

    write_to_keyboard(ENABLE_SCANNING); 
}


char ps2_poll_read(void) {

    uint8_t status = inb(PS2_STATUS);

    while(!(status & PS2_STATUS_OUTPUT)) {
        status = inb(PS2_STATUS);
    }
    return inb(PS2_DATA);
}

int get_char(void) {

    uint8_t c = ps2_poll_read();

    return get_scancode(c);

}

int get_scancode(uint8_t input) {
    
    int result;

    switch (input) {
        case 2:
            result = '1';
            break;
        case 3:
            result = '2';
            break;
        case 4:
            result = '3';
            break;
        case 5:
            result = '4';
            break;
        case 6:
            result = '5';
            break;
        case 7:
            result = '6';
            break;
        case 8:
            result = '7';
            break;
        case 9:
            result = '8';
            break;
        case 10:
            result = '9';
            break;
        case 11:
            result = '0';
            break;
        case 12:
            result = '-';
            break;
        case 13:
            result = '=';
            break;
        case 15:
            //tab button
            result = INVALID_CHAR;
            printk("    ");
            break;
        case 16:
            result = 'q';
            break;
        case 17:
            result = 'w';
            break;
        case 18:
            result = 'e';
            break;
        case 19:
            result = 'r';
            break;
        case 20:
            result = 't';
            break;
        case 21:
            result = 'y';
            break;
        case 22:
            result = 'u';
            break;
        case 23:
            result = 'i';
            break;
        case 24:
            result = 'o';
            break;
        case 25:
            result = 'p';
            break;
        case 26:
            result = '[';
            break;
        case 27:
            result = ']';
            break;
        case 28:
            result = '\n';
            break;
        case 30:
            result = 'a';
            break;
        case 31:
            result = 's';
            break;
        case 32:
            result = 'd';
            break;
        case 33:
            result = 'f';
            break;
        case 34:
            result = 'g';
            break;
        case 35:
            result = 'h';
            break;
        case 36:
            result = 'j';
            break;
        case 37:
            result = 'k';
            break;
        case 38:
            result = 'l';
            break;
        case 39:
            result = ';';
            break;
        case 40:
            result = '\'';
            break;
        case 43:
            result = '\\';
            break;
        case 44:
            result = 'z' ;
            break;
        case 45:
            result = 'x';
            break;
        case 46:
            result = 'c';
            break;
        case 47:
            result = 'v';
            break;
        case 48:
            result = 'b';
            break;
        case 49:
            result = 'n';
            break;
        case 50:
            result = 'm';
            break;
        case 51:
            result = ',';
            break;
        case 52:
            result = '.';
            break;
        case 53:
            result = '/';
            break;
        case 57:
            result = ' ';
            break;

        case 58:
            SHIFT_ON = ~SHIFT_ON;
            result = INVALID_CHAR;
            break;
        case LSHIFT:
            //left shift pressed
            SHIFT_ON = ~SHIFT_ON;
            result = INVALID_CHAR;
            break;
        case RSHIFT:
            //right shift pressed
            SHIFT_ON = ~SHIFT_ON;
            result = INVALID_CHAR;
            break;
        case 170:
            //left shift released
            SHIFT_ON = ~SHIFT_ON;
            result = INVALID_CHAR;
            break;
        case 182:
            //right shift released
            SHIFT_ON = ~SHIFT_ON;
            result = INVALID_CHAR;
            break;
        case 14:
            //delete pressed
            backspace();
            result = INVALID_CHAR;
            break;
        case 29:
            //left control pressed
            CNTRL_ON = TRUE;
            result = INVALID_CHAR;
            break;
        case 157:
            //left control released
            CNTRL_ON = FALSE;
            result = INVALID_CHAR;
            break;
        default:
            return INVALID_CHAR;
            break;
    }

    if (CNTRL_ON && result != INVALID_CHAR) {
        result = cntrl_adjustment(result);
    }
    else if (SHIFT_ON && result != INVALID_CHAR) {
        result = shift_adjustment(result);
    }

    return result;
}

int cntrl_adjustment(int result) {
    
    switch(result) {
        case 'r':
            new_text_color();
            result = INVALID_CHAR;
            break;
    }

    return result;
}

int shift_adjustment(int beforeShift) {
    int afterShift;

    if (beforeShift >= 97 && beforeShift <= 125) {
        return beforeShift - 32;
    }
    
    switch (beforeShift) {
        case '1':
            afterShift = '!';
            break;
        case '2':
            afterShift = '@';
            break;
        case '3':
            afterShift = '#';
            break;
        case '4':
            afterShift = '$';
            break;
        case '5':
            afterShift = '%';
            break;
        case '6':
            afterShift = '^';
            break;
        case '7':
            afterShift = '&';
            break;
        case '8':
            afterShift = '*';
            break;
        case '9':
            afterShift = '(';
            break;
        case '0':
            afterShift = ')';
            break;
        case '-':
            afterShift = '_';
            break;
        case '=':
            afterShift = '+';
            break;
        case ';':
            afterShift = ':';
            break;
        case '\'':
            afterShift = '\"';
            break;        
        case '[':
            afterShift = '{';
            break;
        case ']':
            afterShift = '}';
            break;
        case '\\':
            afterShift = '|';
            break;
        case ',':
            afterShift = '<';
            break;
        case '.':
            afterShift = '>';
            break;
        case '/':
            afterShift = '?';
            break;
        default:
            afterShift = beforeShift;
            break;
    }
    
    return afterShift;
}


