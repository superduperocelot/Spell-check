#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utf-8_converter.h"


short bytes_in_utf_8_char(char c) {
    if(!((c & 0b10000000) ^ 0b00000000)) //First byte is 0;
        return 1;
    if(!((c & 0b11100000) ^ 0b11000000)) //First 2 bytes are 1
        return 2;
    if(!((c & 0b11110000) ^ 0b11100000)) //First 3 bytes are 1
        return 3;
    if(!((c & 0b11110000) ^ 0b11110000)) //First 4 bytes are 1
        return 4;
    return 0;
}

utf_8_char read_utf_8_char(const char** c) {
    utf_8_char u;
    short chars_to_read = bytes_in_utf_8_char(**c);
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

bool is_null_terminator(utf_8_char u) {
    return u.chars[0] == '\0';
}

bool are_equal(utf_8_char u_1, utf_8_char u_2) {
    return bytes_in_utf_8_char(u_1.chars[0]) == bytes_in_utf_8_char(u_2.chars[0]) &&
           memcmp(&u_1, &u_2, bytes_in_utf_8_char(u_1.chars[0])) == 0;
}

bool load_word(const char* str, utf_8_char* word) {
    int i = 0;
    do {
        if(i >= MAX_WORD_LENGTH)
            return false;
        word[i] = read_utf_8_char(&str);
    } while(!is_null_terminator(word[i++]));

    return true;
}

void print_utf_8_char(utf_8_char u) {
    printf("%.*s", bytes_in_utf_8_char(u.chars[0]), u.chars);
}

void print_word(utf_8_char* word) {
    for(int i = 0; !is_null_terminator(word[i]); i++){
        print_utf_8_char(word[i]);
    }
}
