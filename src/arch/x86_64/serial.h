
#define BUFFER_LEN 4096
extern void SER_init(void);
extern void SER_write(const char *buff, int len);

extern void serial_irq_handler(int num, int err, void *arg);

typedef struct State {
    char buffer[BUFFER_LEN];
    char *head, *tail;
} State;


void write_byte(char c);
void producer_add_char(char toAdd, struct State *state);
void consumer_next(struct State *state);
void init_state(void);
