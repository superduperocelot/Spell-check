#include <stdio.h>
#include <string.h> //memcmp
#include <ctype.h> //isspace
#include "utf-8.h"

short utf_8_size(char c) {
    int one_zero   = !((c & 0b10000000) ^ 0b00000000); //First byte is 0;
    int two_ones   = !((c & 0b11100000) ^ 0b11000000); //First 2 bytes are 1
    int three_ones = !((c & 0b11110000) ^ 0b11100000); //First 3 bytes are 1
    int four_ones  = !((c & 0b11110000) ^ 0b11110000); //First 4 bytes are 1

    return one_zero + 2 * two_ones + 3 * three_ones + 4 * four_ones;
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
