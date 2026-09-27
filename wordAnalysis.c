#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"


int count_char(int* char_counts, int i, int* unique_char_count) {
    *unique_char_count += char_counts[i] == 0;
    return char_counts[i]++;
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

    char buf[256];
    reader_info r = new_reader_info(buf, sizeof(buf)/sizeof(*buf));
    utf_8_char u;


    const int hash_map_size = 443;
    utf_8_char hash_map[hash_map_size] = {};

    int char_counts[hash_map_size] = {};
    int unique_char_count;


    while(read_utf_8_char_from_fd(fd, &r, &u)) {
        if(isspace(u.chars[0]))
            continue;
        
        int i = insert_to_hash_map(hash_map, hash_map_size, u);
        if(i == -1) {
            printf("Found aditional character \'");
            print_utf_8_char(u);
            printf("\' but hashmap is full.\n");
            return 1;
        }
        count_char(char_counts, i, &unique_char_count);
    }

    close(fd);
    printf("Found %d characters:\n", unique_char_count);
    print_hash_map(hash_map, hash_map_size, char_counts);
    return 0;
}
