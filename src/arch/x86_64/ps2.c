#include "ps2.h"
#include "vga.h"


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

int SHIFT_ON = FALSE;
int CNTRL_ON = FALSE;

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


