#include <stdio.h>
#include <ctype.h>
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"



unsigned int hash(utf_8 u, int size) {
    unsigned int hash_sum = 0;
    for(unsigned char i = 0; i < utf_8_size(u.chars[0]); i++) {
        hash_sum += u.chars[i];
    }
    hash_sum %= size;
    return hash_sum;
}

unsigned int rehash(unsigned int n, int size) {
    return (n + 131) % size;
}

//Index where u should be placed, or already is
int index_for(utf_8 u, const utf_8* hash_map, int size) {
    unsigned int attempts = 0, i = hash(u, size);
    //While the space is non-empty and not equal to u
    while(!is_null(hash_map[i]) && !are_equal(u, hash_map[i])) {
        i = rehash(i, size);
        if(attempts++ >= size)
            return -1;
    }
    return i;
}

//Index where i is
int index_of(utf_8 u, const utf_8* hash_map, int size) {
    int i = index_for(u, hash_map, size);
    return i >= 0 && !is_null(hash_map[i]) ? i : -1;
}

int insert_to_hash_map(utf_8* hash_map, int size, utf_8 u) {
    int i = index_for(u, hash_map, size);
    if(i > 0)
        hash_map[i] = u;
    return i;
}

void print_hash_map(const utf_8* hash_map, int size, int* char_counts) {
    for(int i = 0; i < size; i++) {
        if(!is_null(hash_map[i])) {
            printf(U_FMT ": %d\n", U_ARG(hash_map[i]), char_counts[i]);
        }
    }
}
