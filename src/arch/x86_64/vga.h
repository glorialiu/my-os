#include <stdarg.h>

extern void VGA_clear(void);
extern void VGA_display_char(char);
extern void VGA_display_str(const char *);

extern int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
extern void print_char(char);
extern void print_str(const char *);
extern void print_uchar(unsigned char);
extern void print_short(short);
extern void print_long_hex(long);
extern void print_long(long);
