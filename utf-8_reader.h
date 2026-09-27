#ifndef UTF_8_CONVERTER
#define UTF_8_CONVERTER 

#include "utf-8.h"

typedef struct {
    char* const buf;
    const char* current;
    const size_t capacity;
    size_t size;
} reader_info;

//Possibly deprecated
#define MAX_WORD_LENGTH 128

utf_8_char read_utf_8_char(const char** c);

bool load_word(const char* str, utf_8_char* word);

reader_info new_reader_info(char* buf, size_t capacity);

bool read_utf_8_char_from_fd(int fd, reader_info* r, utf_8_char* out);

void print_word(utf_8_char* word);


#endif
