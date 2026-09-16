#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "utf-8_converter.h"



int main(void) {
    utf_8_char word[MAX_WORD_LENGTH];
    char str[512];
    scanf("%s", str);
    load_word(str, word);
    print_word(word);
    
    return 0;
}
