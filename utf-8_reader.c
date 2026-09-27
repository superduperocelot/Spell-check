#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h> //read
#include "utf-8_reader.h"


utf_8_char read_utf_8_char(const char** c) {
    utf_8_char u;
    short chars_to_read = utf_8_size(**c);
    if(chars_to_read == 0)
        return (utf_8_char) {{'\0', '\0', '\0', '\0'}};

    for(short i = 0; i < chars_to_read; i++) {
        if(**c == '\0')
            return (utf_8_char) {{'\0', '\0', '\0', '\0'}};
        u.chars[i] = **c;
        (*c)++;
    }
    return u;
}

reader_info new_reader_info(char* buf, size_t capacity) {
    return (reader_info) {buf, buf, capacity, 0};
}


bool read_utf_8_char_from_fd(int fd, reader_info* r, utf_8_char* out) {
    size_t processed_byte_count = r->current - r->buf;
    if(processed_byte_count >= r->size) {
        //No more bytes to read in buf. Try to read more from fd
        //-3 is extra space in case the final char was only partially read
        r->size = read(fd, r->buf, r->capacity - 3);
        if(r->size <= 0)
            return false;
        r->current = r->buf;
    }
    
    int u_size = utf_8_size(*r->current);
    if(u_size > r->size - processed_byte_count) {
        //We need to read more bytes for the char, than there are unread bytes in buf
        size_t missing_byte_count = u_size - (r->size - processed_byte_count);
        assert(missing_byte_count >= 1 && missing_byte_count <= 3);
        if(read(fd, r->buf + r->size, missing_byte_count) != missing_byte_count) {
            printf("Error reading a character\n");
            return false;
        }
        r->size += missing_byte_count;
    }
    const char* p = r->current;
    *out = read_utf_8_char(&r->current);
    return true;
}

//Possibly deprecated

bool load_word(const char* str, utf_8_char* word) {
    int i = 0;
    do {
        if(i >= MAX_WORD_LENGTH)
            return false;
        word[i] = read_utf_8_char(&str);
    } while(!is_null_terminator(word[i++]));

    return true;
}

void print_word(utf_8_char* word) {
    for(int i = 0; !is_null_terminator(word[i]); i++){
        print_utf_8_char(word[i]);
    }
}
