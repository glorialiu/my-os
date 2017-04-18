#include <stdarg.h>


#define GREEN 2
#define BLUE 3
#define RED 4
#define PINK 5
#define ORANGE 6
#define LIGHT_GREY 7
#define GREY 8
#define OCEAN_BLUE 9
#define LIME_GREEN 10
#define BRIGHT_BLUE 11
#define CORAL 12
#define BRIGHT_PINK 13
#define YELLOW 14
#define WHITE 15


extern void VGA_clear(void);
extern void VGA_display_char(char);
extern void VGA_display_str(const char *);

extern void change_text_color(int);
extern void new_text_color(void);

extern void backspace(void);

extern int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
extern void print_uint(unsigned int);
extern void print_int(int);
extern void print_char(char);
extern void print_str(const char *);
extern void print_uchar(unsigned char);
extern void print_short(short);
extern void print_long_hex(long);
extern void print_long(long);
extern void print_quad(long long);
