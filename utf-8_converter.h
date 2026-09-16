#ifndef UTF_8_CONVERTER
#define UTF_8_CONVERTER 

typedef struct {
    char chars[4];
} utf_8_char;

#define MAX_WORD_LENGTH 128

short bytes_in_utf_8_char(char c);

utf_8_char read_utf_8_char(const char** c);

bool is_null_terminator(utf_8_char u);

bool load_word(const char* str, utf_8_char* word);

void print_utf_8_char(utf_8_char u);

void print_word(utf_8_char* word);


#endif
