#include <stdio.h>
#include <ctype.h>
#include "utf-8_reader.h"



unsigned int hash(utf_8_char u, int size) {
    unsigned int hash_sum = 0;
    for(short i = 0; i < utf_8_size(u.chars[0]); i++) {
        hash_sum += u.chars[i];    
    }
    hash_sum %= size;
    return hash_sum;
}

unsigned int rehash(unsigned int n, int size) {
    return (n + 101) % size;
}

int index_of(utf_8_char u, const utf_8_char* hash_map, int size) {
    unsigned int attempts = 0, i = hash(u, size);
    //While the space is non-empty and not equal to u
    while(!is_null_terminator(hash_map[i]) && !are_equal(u, hash_map[i])) {
        i = rehash(i, size);
        if(attempts++ >= size)
            return -1;
    }
    return i;
}

int insert_to_hash_map(utf_8_char* hash_map, int size, utf_8_char u) {
    int i = index_of(u, hash_map, size);
    if(i > 0)
        hash_map[i] = u;
    return i;
}

void print_hash_map(const utf_8_char* hash_map, int size, int* char_counts) {
    for(int i = 0; i < size; i++) {
        if(!is_null_terminator(hash_map[i])) {
            print_utf_8_char(hash_map[i]);
            printf(": %d\n", char_counts[i]);
        }
    }
}
