#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "utf-8_converter.h"


#define HASH_MAP_SIZE 443
int found_char_count = 0;
utf_8_char found_chars[HASH_MAP_SIZE];
int char_counters[HASH_MAP_SIZE];

unsigned int hash(utf_8_char u) {
    unsigned int hash_sum = 0;
    for(short i = 0; i < bytes_in_utf_8_char(u.chars[0]); i++) {
        hash_sum += u.chars[i];    
    }
    hash_sum %= HASH_MAP_SIZE;
    return hash_sum;
}

unsigned int rehash(unsigned int n) {
    return (n + 1) % HASH_MAP_SIZE;
}

void insert_to_hash_map(utf_8_char u) {
    if(isspace(u.chars[0]))
        return;

    unsigned int i = hash(u);
    //While the space is non-empty and not equal to u
    while(!is_null_terminator(found_chars[i]) && !are_equal(u, found_chars[i])) {
        i = rehash(i);
    }
    found_char_count += is_null_terminator(found_chars[i]);
    found_chars[i] = u;
    char_counters[i]++;
}

void print_hash_map() {
    for(int i = 0; i < HASH_MAP_SIZE; i++) {
        if(!is_null_terminator(found_chars[i])) {
            print_utf_8_char(found_chars[i]);
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
    utf_8_char u;
    int bytes_in_buf = 0;
    int counter = 0;
    while(read(fd, u.chars + bytes_in_buf, 1) > 0 && found_char_count < HASH_MAP_SIZE) {
        int bytes_to_read = bytes_in_utf_8_char(u.chars[0]);
        bytes_in_buf++;
        if(bytes_in_buf >= bytes_to_read) {
            insert_to_hash_map(u);
            bytes_in_buf = 0;
        }
        if(++counter % 100000 == 0)
            printf("%d\n", counter);
    }
    close(fd);
    printf("Found %d characters:\n", found_char_count);
    print_hash_map();
    return 0;
}
