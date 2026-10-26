#include <stdio.h>
#include "utf-8.h"
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"
#include "case.h"

#define EMPTY_INDEX 255

#define MIN_ARR_AND_INDEX_MAP(c) (c).min_index_map, (c).min_arr
#define MAJ_ARR_AND_INDEX_MAP(c) (c).maj_index_map, (c).maj_arr

case_map_t new_case_map() {
    case_map_t case_map;
    case_map.count = 0;
    for(int i = 0; i < CASE_MAP_SIZE; i++) {
        case_map.min_index_map[i] = EMPTY_INDEX;
        case_map.maj_index_map[i] = EMPTY_INDEX;
    }
    return case_map;
}



short ref_index_for_single_case(utf_8 u, const unsigned char* index_map, const utf_8* char_arr) {
    unsigned short attempts = 0, n = hash(u, INDEX_MAP_SIZE);
    unsigned char char_index = index_map[n];
    //While the space is non-empty and not equal to u
    while(char_index != EMPTY_INDEX && !are_equal(u, char_arr[char_index])) {
        if(attempts++ >= CASE_MAP_SIZE)
            return -1;
        n = rehash(n, INDEX_MAP_SIZE);
        char_index = index_map[n];
    }
    return n;
}


short ref_index_of_single_case(utf_8 u, const unsigned char* index_map, const utf_8* char_arr) {
    short i = ref_index_for_single_case(u, index_map, char_arr);
    return i >= 0 && index_map[i] != EMPTY_INDEX ? i : -1;
}


//The index of a single case char in the min array or maj array
unsigned char index_of_single_case(utf_8 u, const unsigned char* index_map, const utf_8* char_arr) {
    short ref_i = ref_index_of_single_case(u, index_map, char_arr);
    if(ref_i > 0)
        return index_map[ref_i];
    return EMPTY_INDEX;
}


//The index of a char in the case table
unsigned char index_of_case(utf_8 u, case_map_t case_map) {
    unsigned char i = index_of_single_case(u, MIN_ARR_AND_INDEX_MAP(case_map));
    unsigned char I = index_of_single_case(u, MAJ_ARR_AND_INDEX_MAP(case_map));
    if(i != EMPTY_INDEX)
        return i;
    if(I != EMPTY_INDEX)
        return I;
    return EMPTY_INDEX;
}



bool in_case_map(utf_8 u, case_map_t case_map) {
    return index_of_case(u, case_map) != EMPTY_INDEX;
}


bool pair_in_case_map(utf_8 u, utf_8 U, case_map_t case_map) {
    return index_of_single_case(u, MIN_ARR_AND_INDEX_MAP(case_map)) != EMPTY_INDEX &&
           index_of_single_case(U, MAJ_ARR_AND_INDEX_MAP(case_map)) != EMPTY_INDEX;
}



unsigned char pop_from_index_map(short ref_i, unsigned char* index_map, const utf_8* char_arr) {
    if(index_map[ref_i] == EMPTY_INDEX)
        return EMPTY_INDEX;
    
    unsigned char i = index_map[ref_i];

    short ref_next = rehash(ref_i, CASE_MAP_SIZE);
    unsigned char next = index_map[ref_next];
    bool next_is_rehashed = next != EMPTY_INDEX && hash(char_arr[next], CASE_MAP_SIZE) != ref_next;

    if(next_is_rehashed)
        index_map[ref_i] = pop_from_index_map(ref_next, index_map, char_arr);
    else
        index_map[ref_i] = EMPTY_INDEX;

    return i;        
}


void remove_from_index_map_by_index(unsigned char i, unsigned char* index_map, const utf_8* char_arr) {
    utf_8 u = char_arr[i];
    short ref_i = ref_index_of_single_case(u, index_map, char_arr);
    pop_from_index_map(ref_i, index_map, char_arr);
}


void move_in_index_map_by_index(unsigned char src, unsigned char dst, unsigned char* index_map, utf_8* char_arr) {
    utf_8 u = char_arr[src];
    short ref_i = ref_index_of_single_case(u, index_map, char_arr);

    char_arr[dst] = u;
    index_map[ref_i] = dst;
}


void remove_from_case_map_by_index(unsigned char i, case_map_t* case_map) {
    remove_from_index_map_by_index(i, MIN_ARR_AND_INDEX_MAP(*case_map));
    remove_from_index_map_by_index(i, MAJ_ARR_AND_INDEX_MAP(*case_map));

    //The last pair in the array is moved to fill the empty space at i
    unsigned char last_i = --(case_map->count);
    if(case_map->count == 0)
        return;

    move_in_index_map_by_index(last_i, i, MIN_ARR_AND_INDEX_MAP(*case_map));
    move_in_index_map_by_index(last_i, i, MAJ_ARR_AND_INDEX_MAP(*case_map));
}



bool add_pair_to_case_map(utf_8 u, utf_8 U, case_map_t* case_map) {
    if(pair_in_case_map(u, U, *case_map))
        return true;

    unsigned char i = index_of_case(u, *case_map);
    if(i != EMPTY_INDEX) remove_from_case_map_by_index(i, case_map);
    
    unsigned char I = index_of_case(U, *case_map);
    if(I != EMPTY_INDEX) remove_from_case_map_by_index(I, case_map);
    

    short ref_i = ref_index_for_single_case(u, MIN_ARR_AND_INDEX_MAP(*case_map));
    short ref_I = ref_index_for_single_case(U, MAJ_ARR_AND_INDEX_MAP(*case_map));
    
    if(ref_i < 0 || ref_I < 0) //No space
        return false;

    case_map->min_index_map[ref_i] = case_map->count;
    case_map->maj_index_map[ref_I] = case_map->count;
    case_map->min_arr[case_map->count] = u;
    case_map->maj_arr[case_map->count] = U;
    case_map->count++;
    return true;
}



void print_chars_not_in_case_map(case_map_t case_map, const utf_8* char_map, int char_map_size) {
    printf("Non-paired characters:\n");
    for(int i = 0; i < char_map_size; i++) {
        if(!is_null(char_map[i]) && !in_case_map(char_map[i], case_map))
            printf(U_FMT, U_ARG(char_map[i]));
    }
    printf("\n");
}


bool add_remaining_chars(case_map_t* case_map, const utf_8* char_map, int char_map_size) {
    for(int i = 0; i < char_map_size; i++) {
        if(!is_null(char_map[i]) && !in_case_map(char_map[i], *case_map))
            if(!add_pair_to_case_map(char_map[i], char_map[i], case_map))
                return false;
    }
    return true;
}


void print_case_map(case_map_t case_map) {
    printf("Paired characters:\n");    
    for(unsigned short i = 0; i < case_map.count; i++) {
        printf(U_FMT, U_ARG(case_map.min_arr[i]));
    }
    printf("\n");
    for(unsigned short i = 0; i < case_map.count; i++) {
        printf(U_FMT, U_ARG(case_map.maj_arr[i]));
    }
    printf("\n");
}


enum case_error_code fill_case_map(case_map_t* case_map, const utf_8* char_map, int char_map_size) {
    for(char c = 'a', C = 'A'; c <= 'z'; c++, C++) {
        if (!add_pair_to_case_map(to_utf_8(c), to_utf_8(C), case_map))
            return CASE_OUT_OF_MEMORY;
    }

    printf("If any of the following unknown characters are different capitalizations of the same letter, please write them one at a time like this:\nå Å\nWhen there are no more pairs, press enter.\n\n");
    print_case_map(*case_map);
    printf("\n");
    print_chars_not_in_case_map(*case_map, char_map, char_map_size);

    utf_8 u, U;
    while(read_utf_8_pair(&u, &U)) {
        int i = index_of(u, char_map, char_map_size);
        int I = index_of(U, char_map, char_map_size);
        
        if(i < 0 && I < 0) {
            printf(U_FMT " and " U_FMT " are not in character list\n", U_ARG(u), U_ARG(U));
            continue;
        }
        
        if (!add_pair_to_case_map(u, U, case_map))
            return CASE_OUT_OF_MEMORY;

        print_case_map(*case_map);
        printf("\n");
        print_chars_not_in_case_map(*case_map, char_map, char_map_size);
    }
    if (!add_remaining_chars(case_map, char_map, char_map_size))
        return CASE_OUT_OF_MEMORY;
    return CASE_OK;
}

enum case_error_code print_case_error (enum case_error_code code) {
    switch(code) {
        case CASE_OK:
            break;
        case CASE_OUT_OF_MEMORY:
            printf("Out of memory\n");
            break;
        default:
            printf("Unrecognized error: %d\n", (int) code);
    }
    return code;
}
