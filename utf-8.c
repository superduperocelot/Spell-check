#include <stdio.h>
#include <string.h> //memcmp
#include <ctype.h> //isspace
#include "utf-8.h"

short utf_8_size(char c) {
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

bool is_null(utf_8 u) {
    return u.chars[0] == '\0';
}

bool is_space_utf_8(utf_8 u) {
    return isspace(u.chars[0]);
}

bool are_equal(utf_8 u_1, utf_8 u_2) {
    return utf_8_size(u_1.chars[0]) == utf_8_size(u_2.chars[0]) &&
        memcmp(&u_1, &u_2, utf_8_size(u_1.chars[0])) == 0;
}
