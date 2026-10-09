//This assumes that the file is almost sorted, but some words are misplaced. For example "aa" is equivelant "å", so they may be placed together in the word list, even though they shouldn't be.


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"


void print_non_paired_chars(const utf_8* char_map, const utf_8* char_partners, int size) {
    for(int i = 0; i < size; i++) {
        if(!is_null_terminator(char_map[i]) && are_equal(char_map[i], char_partners[i])) {
            print_utf_8(char_map[i]);
        }
    }
    printf("\n");
}

bool read_utf_8_pair(utf_8* u_1, utf_8* u_2) {
    char c;
    utf_8* current = u_1;
    int len = 0;
    bool last_was_space = true;
    bool done = false;
    while((c = fgetc(stdin)) != '\n') {
        if(done)
            continue;
        if(isspace(c)) {
            if(last_was_space)
                continue;
            if(current == u_1) {
                last_was_space = true;
                current = u_2;
                len = 0;
                continue;
            }
            done = true;
        }
        last_was_space = false;
        if(len < 4)
            current->chars[len] = c;
        len++;
    }
    return current == u_2 && len > 0;
}

bool make_capitalization_pair(utf_8 u, utf_8 U, utf_8* char_map, utf_8* char_partners, int size) {
    int i = index_of(u, char_map, size);
    int I = index_of(U, char_map, size);
    if(i < 0 && I < 0)
        return false;
    
    insert_to_hash_map(char_map, size, u);
    insert_to_hash_map(char_map, size, U);
    char_partners[i] = U;
    char_partners[I] = u;
    return 0;
}

void print_capitalization_pairs(const utf_8* char_map, const utf_8* char_partners, int size) {
    printf("Capitalization pairs:\n");    
    for(int i = 0; i < size; i++) {
        //The index comparison is a fancy cheat code to guarantee every pair is printed only once
        if(!are_equal(char_map[i], char_partners[i]) && index_of(char_partners[i], char_map, size) > i) {
            print_utf_8(char_map[i]);
            printf(" ");
            print_utf_8(char_partners[i]);
            printf("\n");
        }
    }
}

void find_capitalization_pairs(utf_8* char_map, utf_8* char_partners, int size) {
    memcpy(char_partners, char_map, size * sizeof(*char_map));
    
    for(char c = 'a', C = 'A'; c < 'z'; c++, C++) {
        make_capitalization_pair(to_utf_8(c), to_utf_8(C), char_map, char_partners, size);
    }


    printf("Found following foreign characters:\n");
    print_non_paired_chars(char_map, char_partners, size);
    printf("If any of them are the same letter, but capital and not, please write them one at a time like\nÅ å\nIf there are no pairs, press enter.\n");

    utf_8 u, U;
    while(read_utf_8_pair(&u, &U)) {
        int res = make_capitalization_pair(u, U, char_map, char_partners, size);
        if(res != 0) {
            print_utf_8(res < 0 ? u : U);
            printf(" is not in character list\n");
        }
        print_non_paired_chars(char_map, char_partners, size);
    }
    print_capitalization_pairs(char_map, char_partners, size);
}



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
    utf_8 char_partners[char_map_size] = {};
    
    utf_8 u;
    const char* current = buf_1;
    while(current - buf_1 < size && !is_null_terminator(u = read_utf_8_from_stream(&current))) {
        if(is_space_utf_8(u))
            continue;
        int hash_index = insert_to_hash_map(char_map, char_map_size, u);
        if(hash_index < 0) {
            printf("Hash map full\n");
            return 1;
        }
    }
    
    find_capitalization_pairs(char_map, char_partners, char_map_size);
    
    return 0;
}
