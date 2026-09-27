#include <stdio.h>
#include <ctype.h>
#include "utf-8_reader.h"



unsigned int hash(utf_8_char u, int hash_map_size) {
    unsigned int hash_sum = 0;
    for(short i = 0; i < utf_8_size(u.chars[0]); i++) {
        hash_sum += u.chars[i];    
    }
    hash_sum %= hash_map_size;
    return hash_sum;
}

unsigned int rehash(unsigned int n, int hash_map_size) {
    return (n + 101) % hash_map_size;
}

int insert_to_hash_map(utf_8_char* hash_map, int hash_map_size, utf_8_char u) {
    unsigned int attempts = 0, i = hash(u, hash_map_size);
    //While the space is non-empty and not equal to u
    while(!is_null_terminator(hash_map[i]) && !are_equal(u, hash_map[i])) {
        i = rehash(i, hash_map_size);
        if(attempts++ >= hash_map_size)
            return -1;
    }
    hash_map[i] = u;
    return i;
}

void print_hash_map(utf_8_char* hash_map, int hash_map_size, int* char_counts) {
    for(int i = 0; i < hash_map_size; i++) {
        if(!is_null_terminator(hash_map[i])) {
            print_utf_8_char(hash_map[i]);
            printf(": %d\n", char_counts[i]);
        }
    }
}
