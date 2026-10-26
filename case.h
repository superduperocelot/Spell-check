#ifndef CASE
#define CASE
#include "utf-8.h"

enum case_error_code {
    CASE_OK,
    CASE_OUT_OF_MEMORY
};

#define CASE_MAP_SIZE 255
#define INDEX_MAP_SIZE CASE_MAP_SIZE * 2

typedef struct {
    unsigned char index_map[INDEX_MAP_SIZE];
    utf_8 char_arr[CASE_MAP_SIZE];
} single_case_map_t;

typedef struct {
    union {
        single_case_map_t min_case_map;
        struct {
            unsigned char min_index_map[INDEX_MAP_SIZE];
            utf_8 min_arr[CASE_MAP_SIZE];
        };
    };
    
    union {
        single_case_map_t maj_case_map;
        struct {
            unsigned char maj_index_map[INDEX_MAP_SIZE];
            utf_8 maj_arr[CASE_MAP_SIZE];            
        };
    };
    unsigned char count;
} case_map_t;

case_map_t new_case_map();
enum case_error_code fill_case_map(case_map_t* case_map, const utf_8* char_map, int char_map_size);
unsigned char index_of_case(utf_8 u, const case_map_t* case_map);

enum case_error_code print_case_error (enum case_error_code code);

#endif
