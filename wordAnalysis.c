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
    for(short i = 0; i < bytes_in_utf_8_char(u.chars[0]); i++) {
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
    size_t bytes_read = 0;
    size_t total_progress = 0;
    
    while((bytes_read = read(fd, buf, buf_size - 3)) > 0) {
        size_t bytes_processed = 0;
        
        while (bytes_processed < bytes_read) {
            int bytes_in_char = bytes_in_utf_8_char(buf[bytes_processed]);
            size_t unprocessed_byte_count = bytes_read - bytes_processed;

            if(bytes_in_char > unprocessed_byte_count) {
                //Not all bytes for the char has been read yet
                size_t missing_byte_count = bytes_in_char - unprocessed_byte_count;
                if(read(fd, buf + bytes_read, missing_byte_count) != missing_byte_count) {
                    printf("Error reading a character\n");
                    break;
                }
                bytes_read += missing_byte_count;
            }
            utf_8_char u;
            memcpy(&u.chars, buf + bytes_processed, bytes_in_char);
            if(!insert_to_hash_map(u)) {
                printf("Found aditional character \'");
                print_utf_8_char(u);
                printf("\' but hashmap is full.\n");
                return 1;
            }
            bytes_processed += bytes_in_char;
        }
        
        total_progress += bytes_read;
        if((total_progress - bytes_read) / 1000000 != total_progress / 1000000) {
            printf("%lu mB\n", total_progress/1000000);
        }
    }
    close(fd);
    printf("Found %d characters:\n", found_char_count);
    print_hash_map();
    return 0;
}
