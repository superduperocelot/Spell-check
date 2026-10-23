#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h> //read
#include <ctype.h>
#include "utf-8_reader.h"

unsigned char read_utf_8_internal(const char* c, utf_8* out) {
    unsigned char size = utf_8_size(*c);
    *out = UTF_8_NULL;

    for(unsigned char i = 0; i < size; i++) {
        (*out).chars[i] = c[i];
    }
    return size;
}

utf_8 to_utf_8(char c) {
    utf_8 u = UTF_8_NULL;
    u.chars[0] = c;    
    return u;
}

utf_8 read_utf_8(const char* c) {
    utf_8 u;
    read_utf_8_internal(c, &u);
    return u;
}


utf_8 read_utf_8_from_stream(const char** c) {
    utf_8 u;
    (*c) += read_utf_8_internal(*c, &u);
    return u;
}

reader_info new_reader_info(char* buf, size_t capacity) {
    return (reader_info) {buf, buf, capacity, 0};
}


bool read_utf_8_from_fd(int fd, reader_info* r, utf_8* out) {
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
    *out = read_utf_8_from_stream(&r->current);
    return true;
}


bool read_utf_8_pair(utf_8* u_1, utf_8* u_2) {
    char c;
    utf_8* current = u_1;
    int len = 0;
    bool last_was_space = true;
    bool done = false;
    while((c = fgetc(stdin)) != '\n') {
        if(done)
            continue;
        if(isspace(c)) {
            if(last_was_space)
                continue;
            if(current == u_1) {
                last_was_space = true;
                current = u_2;
                len = 0;
                continue;
            }
            done = true;
        }
        last_was_space = false;
        if(len < 4)
            current->chars[len] = c;
        len++;
    }
    return current == u_2 && len > 0;
}


//Possibly deprecated

bool load_word(const char* str, utf_8* word) {
    int i = 0;
    do {
        if(i >= MAX_WORD_LENGTH)
            return false;
        word[i] = read_utf_8_from_stream(&str);
    } while(!is_null(word[i++]));

    return true;
}

void print_word(utf_8* word) {
    for(int i = 0; !is_null(word[i]); i++){
        printf(U_FMT, U_ARG(word[i]));
    }
}
