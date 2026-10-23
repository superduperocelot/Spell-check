#ifndef UTF_8
#define UTF_8

#define UTF_8_NULL ((utf_8) {{'\0', '\0', '\0', '\0'}})

#define U_FMT "%.*s"
#define U_ARG(u) utf_8_size((u).chars[0]), (u).chars


typedef struct {
    char chars[4];
} utf_8;


unsigned char utf_8_size(char c);

bool is_space_utf_8(utf_8 u);

bool is_null(utf_8 u);

bool are_equal(utf_8 u_1, utf_8 u_2);

#endif
