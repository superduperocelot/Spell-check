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

#define MAX_WORD_LEN 128


bool parse_file(int fd, short* temp_index_buf, utf_8* char_map, size_t char_map_size, size_t* word_size_counts, size_t* char_count) {
    *char_count = 0;

    short word_len = 0;
    bool skip_word = false;

    utf_8 u;
    reader_info reader = new_reader_info((char[256]){}, 256);
    while(read_utf_8_from_fd(fd, &reader, &u)) {
        if(are_equal(u, to_utf_8('\n'))) {
            if(!skip_word) {
                temp_index_buf[(*char_count)++] = word_len; //Make the word end with its length instead of \n
                word_size_counts[word_len]++;
            }
            word_len = 0;
            skip_word = false;
            continue;
        }

        if(are_equal(u, to_utf_8('\b')) || skip_word)
            continue;

        if(is_space_utf_8(u)) {
            (*char_count) -= word_len;
            skip_word = true;
            continue;
        }

        
        short hash_index = insert_to_hash_map(char_map, char_map_size, u);
        if(hash_index < 0) {
            printf("Hash map full\n");
            return false;
        }

        temp_index_buf[(*char_count)++] = hash_index;
        word_len++;
        if(word_len > MAX_WORD_LEN) {
            printf("Word is too large. Skipping it.\n");
            skip_word = true;
        }
    }
    return true;
}


unsigned char* make_real_index_buf(const short* temp_index_buf, size_t char_count, const utf_8* char_map, case_map_t* case_map) {
    unsigned char* real_index_buf = malloc(char_count);
    if(!real_index_buf) return NULL;
    for(size_t i = 0; i < char_count; i++) {
        real_index_buf[i] = index_of_case(char_map[temp_index_buf[i]], case_map);
    }
    return real_index_buf;
}


int find_largest_word_size(const size_t* word_size_counts) {
    int largest_word_size = 0;
    for(int i = MAX_WORD_LEN - 1; i >= 0; i--) {
        if(word_size_counts[i] != 0) {
            largest_word_size = i;
            break;
        }
    }
    return largest_word_size;
}


bool make_word_lists(unsigned char*** word_lists_out, const size_t* word_size_counts) {
    int largest_word_size = find_largest_word_size(word_size_counts);
    if(largest_word_size == 0) return false;

    *word_lists_out = malloc(sizeof(unsigned char*) * (largest_word_size)+1);
    if(!word_lists_out) return false;

    for(int i = 0; i <= largest_word_size; i++) {
        int word_count = word_size_counts[i];
        if(word_count == 0) {
            (*word_lists_out)[i] = NULL;
            continue;
        }
        (*word_lists_out)[i] = malloc(word_count * i);
        if((*word_lists_out)[i] == NULL)
            return false;
    }
    return true;
}


void fill_word_lists(unsigned char** word_lists, const size_t* word_size_counts, const short* temp_index_buf, size_t char_count, const utf_8* char_map, case_map_t* case_map) {
    int largest_word_size = find_largest_word_size(word_size_counts);
    size_t word_list_element_counts[largest_word_size + 1] = {};
    
    for(long long int i = char_count - 1; i >= 0;) {
        short word_size = temp_index_buf[i--];
        unsigned char* word_list = word_lists[word_size];
        size_t last_index = word_size_counts[word_size] * word_size - 1;
        
        for(int j = 0; j < word_size; j++, i--, word_list_element_counts[word_size]++) {
            unsigned char real_index = index_of_case(char_map[temp_index_buf[i]], case_map);
            word_list[last_index - word_list_element_counts[word_size]] = real_index;
        }
    }
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
    size_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    short* temp_index_buf = malloc(file_size * sizeof(short)); //Buffer for hash map indices representing utf_8 chars in the file. It is likely slightly too large
    //HANDLE MALLOC FAIL

    
    const int char_map_size = 521;
    utf_8 char_map[char_map_size] = {};

    size_t char_count;

    size_t word_size_counts[MAX_WORD_LEN] = {0};

    if(!parse_file(fd, temp_index_buf, char_map, char_map_size, word_size_counts, &char_count)) return 1;
    close(fd);
    
    case_map_t case_map = new_case_map();
    enum case_error_code status = fill_case_map(&case_map, char_map, char_map_size);
    if(status != CASE_OK)
        return print_case_error(status);
    
    
    unsigned char** word_lists;
    if(!make_word_lists(&word_lists, word_size_counts)) {
        printf("Out of memory\n");
        return 1;
    }
    printf("filling\n");
    fill_word_lists(word_lists, word_size_counts, temp_index_buf, char_count, char_map, &case_map);
    
    printf("Done\n");
    for(int i = 0; i < word_size_counts[1] * 1; i++) {
        printf(U_FMT, U_ARG(case_map.min_arr[word_lists[1][i]]));
        if(i % 1 == 0) printf("\n");
    }
        
    return 0;
}
