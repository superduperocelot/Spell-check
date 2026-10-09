#ifndef UTF_8
#define UTF_8

typedef struct {
    char chars[4];
} utf_8;

short utf_8_size(char c);

bool is_space_utf_8(utf_8 u);

bool is_null_terminator(utf_8 u);

bool are_equal(utf_8 u_1, utf_8 u_2);

void print_utf_8(utf_8 u);

#endif
