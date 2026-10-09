#ifndef UTF_8_READER
#define UTF_8_READER 

#include "utf-8.h"

typedef struct {
    char* const buf;
    const char* current;
    const size_t capacity;
    size_t size;
} reader_info;

//Possibly deprecated
#define MAX_WORD_LENGTH 128

utf_8 to_utf_8(char c);

utf_8 read_utf_8(const char* c);

utf_8 read_utf_8_from_stream(const char** c);

bool load_word(const char* str, utf_8* word);

reader_info new_reader_info(char* buf, size_t capacity);

bool read_utf_8_from_fd(int fd, reader_info* r, utf_8* out);

void print_word(utf_8* word);


#endif
