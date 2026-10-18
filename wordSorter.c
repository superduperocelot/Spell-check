//This assumes that the file is almost sorted, but some words are misplaced. For example "aa" is equivelant "å", so they may be placed together in the word list, even though they shouldn't be.


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"
#include "case.h"




int main(int argc, char **argv) {
    //--Input validation--
    if (argc < 2) {
        printf("No filename provided\n");
        return 1;
    }
    int fd = open(argv[1], O_RDWR);
    if(fd == -1) {
        printf("Error opening file\n");
        return 1;
    }
    
    //--Buffer setup--
    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    
    char* buf_1 = malloc(size * 2);
    if(!buf_1) {
        printf("Not enough memory. Needs %llu bytes for file.\n", (long long unsigned) size);
        return 1;
    }
    char* buf_2 = buf_1 + size;
    
    if(read(fd, buf_1, size) != size) {
        printf("Error reading from file\n");
        return 1;
    }
    
    const int char_map_size = 411;
    utf_8 char_map[char_map_size] = {};


    
    utf_8 u;
    const char* current = buf_1;
    //Save memory by writing into the beginning of buf    
    while(current - buf_1 < size && !is_null(u = read_utf_8_from_stream(&current))) {
        if(is_space_utf_8(u))
            continue;
        int hash_index = insert_to_hash_map(char_map, char_map_size, u);
        if(hash_index < 0) {
            printf("Hash map full\n");
            return 1;
        }
    }
    
    int min_index_map[char_map_size];
    int maj_index_map[char_map_size];
    utf_8 min_arr[char_map_size] = {};
    utf_8 maj_arr[char_map_size] = {};
    case_map_t case_map = new_case_map(min_index_map, maj_index_map, min_arr, maj_arr, char_map_size);
    
    enum case_error_code status = fill_case_map(&case_map, char_map);
    if(status != CASE_OK)
        return print_case_error(status);
    
    
    return 0;
}
