#ifndef UTF_8_HASH_MAP
#define UTF_8_HASH_MAP 

#include "utf-8.h"

int index_for(utf_8 u, const utf_8* hash_map, int size);
int index_of(utf_8 u, const utf_8* hash_map, int size);

int insert_to_hash_map(utf_8* hash_map, int size, utf_8 u);

void print_hash_map(const utf_8* hash_map, int size, int* char_counts);

unsigned int hash(utf_8 u, int size);
unsigned int rehash(unsigned int n, int size);

#endif
