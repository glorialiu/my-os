#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)

#define DISABLE_PORT1 0xAD
#define DISABLE_PORT2 0xA7
#define READ_RAM_0 0x20


void poll_and_write(int port, int data) {

    uint8_t status;

    status = inb(PS2_STATUS);

    (status_reg) status;

    while (status.output_status) {
        status = (status_reg) inb(PS2_STATUS);
    }

    outb(port, data);
}

uint8_t poll_and_read(int port) {
    uint8_t status;

    status = inb(PS2_STATUS);

    (status_reg) status;

    while (status.input_status) {
        status = (status_reg) inb(PS2_STATUS);
    }
    return inb(port);
}


void ps2_initialize(void) {

    ps2_config config;

    poll_and_write(PS2_CMD, DISABLE_PORT1);   
    poll_and_write(PS2_CMD, DISABLE_PORT2);
    poll_and_write(PS2_CMD, READ_RAM_0);

    config = (ps2_config)  poll_and_read(PS2_DATA);
    
}


char ps2_poll_read(void) {

    char status = inb(PS2_STATUS);

    while(!(status & PS2_STATUS_OUTPUT)) {
        status = inb(PS2_STATUS);
    }
    return inb(PS2_DATA);
}
