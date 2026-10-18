#include <stdio.h>
#include "utf-8.h"
#include "utf-8_reader.h"
#include "utf-8_hash_map.h"
#include "case.h"

#define EMPTY_INDEX -1

#define MIN_ARR_AND_INDEX_MAP(c) (c).min_index_map, (c).min_arr, (c).size
#define MAJ_ARR_AND_INDEX_MAP(c) (c).maj_index_map, (c).maj_arr, (c).size

case_map_t new_case_map(int* min_index_map, int* maj_index_map, utf_8* min_arr, utf_8* maj_arr, int size) {
    for(int i = 0; i < size; i++) {
        min_index_map[i] = EMPTY_INDEX;
        maj_index_map[i] = EMPTY_INDEX;
    }
    return (case_map_t) {min_index_map, maj_index_map, min_arr, maj_arr, 0, size};
}



int ref_index_for_single_case(utf_8 u, const int* index_map, const utf_8* char_arr, int size) {
    unsigned int attempts = 0, n = hash(u, size);
    int char_index = index_map[n];
    //While the space is non-empty and not equal to u
    while(char_index != EMPTY_INDEX && !are_equal(u, char_arr[char_index])) {
        if(attempts++ >= size)
            return -1;
        n = rehash(n, size);
        char_index = index_map[n];
    }
    return n;
}


int ref_index_of_single_case(utf_8 u, const int* index_map, const utf_8* char_arr, int size) {
    int i = ref_index_for_single_case(u, index_map, char_arr, size);
    return i >= 0 && index_map[i] != EMPTY_INDEX ? i : -1;
}


//The index of a single case char in the min array or maj array
int index_of_single_case(utf_8 u, const int* index_map, const utf_8* char_arr, int size) {
    int ref_i = ref_index_of_single_case(u, index_map, char_arr, size);
    if(ref_i > 0)
        return index_map[ref_i];
    return EMPTY_INDEX;
}


//THe index of a char in the case table
int index_of_case(utf_8 u, case_map_t case_map) {
    int i = index_of_single_case(u, MIN_ARR_AND_INDEX_MAP(case_map));
    int I = index_of_single_case(u, MAJ_ARR_AND_INDEX_MAP(case_map));
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



int pop_from_index_map(int ref_i, int* index_map, const utf_8* char_arr, int size) {
    if(index_map[ref_i] == EMPTY_INDEX)
        return EMPTY_INDEX;
    
    int i = index_map[ref_i];

    int ref_next = rehash(ref_i, size);
    int next = index_map[ref_next];
    bool next_is_rehashed = next != EMPTY_INDEX && hash(char_arr[next], size) != ref_next;

    if(next_is_rehashed)
        index_map[ref_i] = pop_from_index_map(ref_next, index_map, char_arr, size);
    else
        index_map[ref_i] = EMPTY_INDEX;

    return i;        
}


void remove_from_index_map_by_index(int i, int* index_map, const utf_8* char_arr, int size) {
    utf_8 u = char_arr[i];
    int ref_i = ref_index_of_single_case(u, index_map, char_arr, size);
    pop_from_index_map(ref_i, index_map, char_arr, size);
}


void move_in_index_map_by_index(int src, int dst, int* index_map, utf_8* char_arr, int size) {
    utf_8 u = char_arr[src];
    int ref_i = ref_index_of_single_case(u, index_map, char_arr, size);

    char_arr[dst] = u;
    index_map[ref_i] = dst;
}


void remove_from_case_map_by_index(int i, case_map_t* case_map) {
    remove_from_index_map_by_index(i, MIN_ARR_AND_INDEX_MAP(*case_map));
    remove_from_index_map_by_index(i, MAJ_ARR_AND_INDEX_MAP(*case_map));

    //The last pair in the array is moved to fill the empty space at i
    int last_i = --(case_map->count);
    if(case_map->count == 0)
        return;

    move_in_index_map_by_index(last_i, i, MIN_ARR_AND_INDEX_MAP(*case_map));
    move_in_index_map_by_index(last_i, i, MAJ_ARR_AND_INDEX_MAP(*case_map));
}



bool add_pair_to_case_map(utf_8 u, utf_8 U, case_map_t* case_map) {
    if(pair_in_case_map(u, U, *case_map))
        return true;

    int i = index_of_case(u, *case_map);
    if(i >= 0) remove_from_case_map_by_index(i, case_map);
    
    int I = index_of_case(U, *case_map);
    if(I >= 0) remove_from_case_map_by_index(I, case_map);
    

    int ref_i = ref_index_for_single_case(u, MIN_ARR_AND_INDEX_MAP(*case_map));
    int ref_I = ref_index_for_single_case(U, MAJ_ARR_AND_INDEX_MAP(*case_map));
    
    if(ref_i < 0 || ref_I < 0) //No space
        return false;

    case_map->min_index_map[ref_i] = case_map->count;
    case_map->maj_index_map[ref_I] = case_map->count;
    case_map->min_arr[case_map->count] = u;
    case_map->maj_arr[case_map->count] = U;
    case_map->count++;
    return true;
}



void print_chars_not_in_case_map(case_map_t case_map, const utf_8* char_map) {
    printf("Non-paired characters:\n");
    for(int i = 0; i < case_map.size; i++) {
        if(!is_null(char_map[i]) && !in_case_map(char_map[i], case_map))
            printf(U_FMT, U_ARG(char_map[i]));
    }
    printf("\n");
}


bool add_remaining_chars(case_map_t* case_map, const utf_8* char_map) {
    for(int i = 0; i < case_map->size; i++) {
        if(!is_null(char_map[i]) && !in_case_map(char_map[i], *case_map))
            if(!add_pair_to_case_map(char_map[i], char_map[i], case_map))
                return false;
    }
    return true;
}


void print_case_map(case_map_t case_map) {
    printf("Paired characters:\n");    
    for(int i = 0; i < case_map.count; i++) {
        printf(U_FMT, U_ARG(case_map.min_arr[i]));
    }
    printf("\n");
    for(int i = 0; i < case_map.count; i++) {
        printf(U_FMT, U_ARG(case_map.maj_arr[i]));
    }
    printf("\n");
}


enum case_error_code fill_case_map(case_map_t* case_map, const utf_8* char_map) {
    for(char c = 'a', C = 'A'; c <= 'z'; c++, C++) {
        if (!add_pair_to_case_map(to_utf_8(c), to_utf_8(C), case_map))
            return CASE_OUT_OF_MEMORY;
    }

    print_chars_not_in_case_map(*case_map, char_map);
    printf("If any of the following unknown characters are different capitalizations of the same letter, please write them one at a time like this:\nå Å\nWhen there are no more pairs, press enter.\n\n");
    print_case_map(*case_map);
    printf("\n");
    print_chars_not_in_case_map(*case_map, char_map);

    utf_8 u, U;
    while(read_utf_8_pair(&u, &U)) {
        int i = index_of(u, char_map, case_map->size);
        int I = index_of(U, char_map, case_map->size);
        
        if(i < 0 && I < 0) {
            printf(U_FMT " and " U_FMT " are not in character list\n", U_ARG(u), U_ARG(U));
            continue;
        }
        
        if (!add_pair_to_case_map(u, U, case_map))
            return CASE_OUT_OF_MEMORY;

        print_case_map(*case_map);
        printf("\n");
        print_chars_not_in_case_map(*case_map, char_map);
    }
    if (!add_remaining_chars(case_map, char_map))
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
