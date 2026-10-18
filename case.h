#ifndef CASE
#define CASE
#include "utf-8.h"

enum case_error_code {
    CASE_OK,
    CASE_OUT_OF_MEMORY
};

typedef struct {
    int* min_index_map;
    int* maj_index_map;
    utf_8* min_arr;
    utf_8* maj_arr;
    int count;
    const int size;
} case_map_t;

case_map_t new_case_map(int* min_index_map, int* maj_index_map, utf_8* min_arr, utf_8* maj_arr, int size);
enum case_error_code fill_case_map(case_map_t* case_map, const utf_8* char_map);
int index_of_case(utf_8 u, case_map_t case_map);

enum case_error_code print_case_error (enum case_error_code code);

#endif
