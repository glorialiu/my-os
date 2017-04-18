#include <stdint.h>
#include "inline_asm.h"

#define INVALID_CHAR -1

#define TRUE 1
#define FALSE 0

#define ON 1
#define OFF 0

extern char ps2_poll_read(void);
extern void ps2_initialize(void);

void keyboard_initialize(void);
int get_scancode(uint8_t);
int get_char();
int shift_adjustment(int);
int cntrl_adjustment(int);


typedef struct status_reg {
    uint8_t output_status:1;
    uint8_t input_status:1;
    uint8_t system_flag:1;
    uint8_t command:1;
    uint8_t unknown1:1;
    uint8_t unknown2:1;
    uint8_t timeout_err:1;
    uint8_t parity_err:1;
} __attribute__((packed)) status_reg;

typedef struct ps2_config {
    uint8_t port1_interrupt:1;
    uint8_t port2_interrupt:1;
    uint8_t system_flag:1;
    uint8_t zero:1;
    uint8_t port1_clock:1;
    uint8_t port2_clock:1;
    uint8_t port1_translation:1;
    uint8_t zero1:1;

} __attribute__((packed)) ps2_config;
