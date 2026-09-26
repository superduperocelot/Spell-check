#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "utf-8_converter.h"


#define HASH_MAP_SIZE 443
int found_char_count = 0;
utf_8_char hash_map[HASH_MAP_SIZE];
int char_counters[HASH_MAP_SIZE];

unsigned int hash(utf_8_char u) {
    unsigned int hash_sum = 0;
    for(short i = 0; i < utf_8_size(u.chars[0]); i++) {
        hash_sum += u.chars[i];    
    }
    hash_sum %= HASH_MAP_SIZE;
    return hash_sum;
}

unsigned int rehash(unsigned int n) {
    return (n + 101) % HASH_MAP_SIZE;
}

bool insert_to_hash_map(utf_8_char u) {
    if(isspace(u.chars[0]))
        return true;

    unsigned int attempts = 0, i = hash(u);
    //While the space is non-empty and not equal to u
    while(!is_null_terminator(hash_map[i]) && !are_equal(u, hash_map[i])) {
        i = rehash(i);
        if(attempts++ >= HASH_MAP_SIZE)
            return false;
    }
    
    found_char_count += is_null_terminator(hash_map[i]);
    hash_map[i] = u;
    char_counters[i]++;
    return true;
}

void print_hash_map() {
    for(int i = 0; i < HASH_MAP_SIZE; i++) {
        if(!is_null_terminator(hash_map[i])) {
            print_utf_8_char(hash_map[i]);
            printf(": %d\n", char_counters[i]);
        }
    }
}


int main (int argc, char **argv) {
    if (argc < 2) {
        printf("No filename provided\n");
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) {
        printf("Error opening file\n");
        return 1;
    }
    const int buf_size = 256;
    char buf[buf_size];
    reader_info r = new_reader_info(buf, buf_size);
    utf_8_char u;

    while(read_utf_8_char_from_fd(fd, &r, &u)) {
        if(!insert_to_hash_map(u)) {
            printf("Found aditional character \'");
            print_utf_8_char(u);
            printf("\' but hashmap is full.\n");
            return 1;
        }
    }

    close(fd);
    printf("Found %d characters:\n", found_char_count);
    print_hash_map();
    return 0;
}
