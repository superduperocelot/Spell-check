#ifndef UTF_8
#define UTF_8

typedef struct {
    char chars[4];
} utf_8_char;

short utf_8_size(char c);

bool is_null_terminator(utf_8_char u);

bool are_equal(utf_8_char u_1, utf_8_char u_2);

void print_utf_8_char(utf_8_char u);

#endif
