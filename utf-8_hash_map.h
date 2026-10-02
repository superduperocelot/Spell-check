#ifndef UTF_8_HASH_MAP
#define UTF_8_HASH_MAP 

#include "utf-8.h"

int index_of(utf_8_char u, const utf_8_char* hash_map, int size);

int insert_to_hash_map(utf_8_char* hash_map, int size, utf_8_char u);

void print_hash_map(const utf_8_char* hash_map, int size, int* char_counts);

#endif
