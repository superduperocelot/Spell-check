#ifndef UTF_8_HASH_MAP
#define UTF_8_HASH_MAP 

#include "utf-8.h"

short index_for(utf_8 u, const utf_8* hash_map, short size);
short index_of(utf_8 u, const utf_8* hash_map, short size);

short insert_to_hash_map(utf_8* hash_map, short size, utf_8 u);

void print_hash_map(const utf_8* hash_map, short size, int* char_counts);

unsigned short hash(utf_8 u, short size);
unsigned short rehash(unsigned short n, short size);

#endif
